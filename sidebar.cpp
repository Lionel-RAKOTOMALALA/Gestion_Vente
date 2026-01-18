#include "sidebar.h"
#include "thememanager.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QSignalMapper>
#include <QLabel>

Sidebar::Sidebar(const QString &userRole, QWidget *parent) : QWidget(parent)
{
    setObjectName("sidebar");
    setFixedWidth(250); // Largeur augmentée pour un meilleur espacement

    layout = new QVBoxLayout(this);
    layout->setSpacing(4); // Espacement réduit pour un look plus compact
    layout->setContentsMargins(12, 20, 12, 20); // Marges améliorées

    QLabel *appTitle = new QLabel("GESTION VENTE", this);
    appTitle->setObjectName("sidebarTitle");
    appTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(appTitle);
    layout->addSpacing(30); // Espace après le titre

    // Appliquer les styles initaux du sidebar
    QString sidebarStyle = QString(
        "#sidebar { "
        "background-color: %1; "
        "border-right: 1px solid %2; "
        "}"
        "#sidebarTitle { "
        "font-size: 18px; "
        "font-weight: bold; "
        "color: %3; "
        "margin: 16px 0; "
        "}"
        "#sidebarButton { "
        "background-color: transparent; "
        "color: %3; "
        "border: 2px solid transparent; "
        "border-radius: 8px; "
        "padding: 10px 12px; "
        "font-size: 13px; "
        "text-align: left; "
        "font-weight: 500; "
        "}"
        "#sidebarButton:hover { "
        "background-color: %4; "
        "border: 2px solid %5; "
        "}"
        "#sidebarButton:checked { "
        "background-color: %5; "
        "color: white; "
        "border: 2px solid %5; "
        "}"
        "#themeButton { "
        "background-color: %4; "
        "color: %3; "
        "border: 2px solid %5; "
        "border-radius: 8px; "
        "padding: 10px 12px; "
        "font-size: 13px; "
        "font-weight: bold; "
        "}"
        "#themeButton:hover { "
        "background-color: %5; "
        "color: white; "
        "}"
        "#logoutButton { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ef4444, stop:1 #dc2626); "
        "color: white; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 10px 12px; "
        "font-weight: bold; "
        "font-size: 13px; "
        "}"
        "#logoutButton:hover { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #dc2626, stop:1 #991b1b); "
        "}"
    ).arg(
        ThemeManager::instance().backgroundColor().name(),
        ThemeManager::instance().borderColor().name(),
        ThemeManager::instance().textColor().name(),
        ThemeManager::instance().surfaceColor().name(),
        ThemeManager::instance().primaryColor().name()
    );
    
    setStyleSheet(sidebarStyle);

    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mappedInt(int)), this, SIGNAL(pageChanged(int)));

    // Boutons de navigation avec emojis comme icônes
    QStringList pages;
    if (userRole != "VENDEUR") {
        pages = {"📊  Dashboard", "👥  Utilisateurs", "🤝  Clients", "📦  Produits", "📋  Commandes", "💳  Paiements", "💰  Caisse"};
    } else {
        pages = {"📊  Dashboard", "🤝  Clients", "📦  Produits", "📋  Commandes", "💳  Paiements"};
    }
    
    for (int i = 0; i < pages.size(); ++i) {
        QPushButton *btn = new QPushButton(pages[i], this);
        btn->setObjectName("sidebarButton");
        btn->setCheckable(true); // Rendre les boutons checkable pour l'état actif
        btn->setProperty("page", i); // Propriété pour identifier le bouton
        layout->addWidget(btn);

        connect(btn, &QPushButton::clicked, [this, btn, i]() {
            // Désactiver tous les autres boutons
            for (int j = 0; j < layout->count(); ++j) {
                if (QPushButton *otherBtn = qobject_cast<QPushButton*>(layout->itemAt(j)->widget())) {
                    if (otherBtn->objectName() == "sidebarButton") {
                        otherBtn->setChecked(false);
                    }
                }
            }
            btn->setChecked(true);
        });

        connect(btn, SIGNAL(clicked()), mapper, SLOT(map()));
        mapper->setMapping(btn, i);
    }

    layout->addStretch(); // Pousse les boutons vers le haut

    // Ajouter le bouton de déconnexion en bas
    QPushButton *logoutBtn = new QPushButton("🚪 Déconnexion", this);
    logoutBtn->setObjectName("logoutButton");
    logoutBtn->setMinimumHeight(48);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "   stop:0 #ef4444, stop:1 #dc2626);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 14px;"
        "   font-weight: 600;"
        "   margin: 8px 0;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "   stop:0 #dc2626, stop:1 #b91c1c);"
        "}"
        "QPushButton:pressed {"
        "   background: #b91c1c;"
        "}"
    );
    connect(logoutBtn, &QPushButton::clicked, this, &Sidebar::logoutRequested);
    layout->addWidget(logoutBtn);

    if (QPushButton *firstBtn = qobject_cast<QPushButton*>(layout->itemAt(2)->widget())) {
        firstBtn->setChecked(true);
    }
}

void Sidebar::updateTheme()
{
    ThemeManager& theme = ThemeManager::instance();
    
    QString sidebarStyle = QString(
        "#sidebar {"
        "   background-color: %1;"
        "   border-right: 1px solid %2;"
        "}"
        "#sidebarTitle {"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   color: %3;"
        "   margin: 16px 0;"
        "}"
        "#sidebarButton {"
        "   background-color: transparent;"
        "   color: %3;"
        "   border: 2px solid transparent;"
        "   border-radius: 8px;"
        "   padding: 10px 12px;"
        "   font-size: 13px;"
        "   text-align: left;"
        "   font-weight: 500;"
        "}"
        "#sidebarButton:hover {"
        "   background-color: %4;"
        "   border: 2px solid %5;"
        "}"
        "#sidebarButton:checked {"
        "   background-color: %5;"
        "   color: white;"
        "   border: 2px solid %5;"
        "}"
        "#themeButton {"
        "   background-color: %4;"
        "   color: %3;"
        "   border: 2px solid %5;"
        "   border-radius: 8px;"
        "   padding: 10px 12px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "#themeButton:hover {"
        "   background-color: %5;"
        "   color: white;"
        "}"
        "#logoutButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %6, stop:1 %7);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 12px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "#logoutButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %7, stop:1 %8);"
        "}"
    ).arg(
        theme.backgroundColor().name(),
        theme.borderColor().name(),
        theme.textColor().name(),
        theme.surfaceColor().name(),
        theme.primaryColor().name(),
        theme.dangerColor().name(),
        theme.dangerHoverColor().name(),
        theme.dangerPressedColor().name()
    );
    
    setStyleSheet(sidebarStyle);
}
