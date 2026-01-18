#include "productspage.h"
#include "productdialog.h"
#include "thememanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QPixmap>
#include <QFile>
#include <QPushButton>
#include <QFrame>

ProductsPage::ProductsPage(const QString &userRole, int userId, QWidget *parent) : QFrame(parent), userRole(userRole), userId(userId)
{
    setObjectName("productsPage");
    setupDatabase();
    setupUI();
    applyStyles();
    loadProducts();
    
    if (userRole == "VENDEUR") {
        orderDialog = new OrderDialog(userId, this);
        connect(orderDialog, &OrderDialog::orderSaved, [this]() { emit orderValidated(); });
    } else {
        orderDialog = nullptr;
    }
}

void ProductsPage::setupDatabase()
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS PRODUITS ("
               "id_produit INTEGER PRIMARY KEY AUTOINCREMENT, "
               "nom_produit TEXT NOT NULL, "
               "description TEXT, "
               "photo_produit VARCHAR(255), "
               "prix_vente REAL NOT NULL, "
               "prix_achat REAL, "
               "stock INTEGER NOT NULL DEFAULT 0, "
               "seuil_alerte INTEGER DEFAULT 5, "
               "date_creation DATETIME DEFAULT CURRENT_TIMESTAMP)");
}

void ProductsPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(28);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *icon = new QLabel(this);
    icon->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #8b5cf6, stop:1 #6366f1);"
        "border-radius: 14px;"
        "min-width: 52px; max-width: 52px;"
        "min-height: 52px; max-height: 52px;"
    );

    QLabel *title = new QLabel("Gestion des Produits", this);
    title->setObjectName("titleH1");
    ThemeManager& theme = ThemeManager::instance();
    title->setStyleSheet(QString(
        "font-size: 32px;"
        "font-weight: 700;"
        "color: %1;"
        "letter-spacing: -0.5px;"
    ).arg(theme.textColor().name()));

    headerLayout->addWidget(icon);
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(16);

    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Rechercher par nom ou description...");
    searchInput->setMinimumHeight(48);
    searchInput->setStyleSheet(QString(
        "QLineEdit {"
        "   border: 1px solid %1;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 15px;"
        "   background: %2;"
        "   color: %3;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid %4;"
        "   outline: none;"
        "   background: %5;"
        "}"
        "QLineEdit::placeholder {"
        "   color: %6;"
        "}"
    ).arg(theme.borderColor().name(),
          theme.inputBackground().name(),
          theme.textColor().name(),
          theme.primaryColor().name(),
          theme.surfaceAltColor().name(),
          theme.textTertiaryColor().name()));
    
    connect(searchInput, &QLineEdit::textChanged, this, &ProductsPage::onSearchTextChanged);

    searchLayout->addWidget(searchInput, 1);
    mainLayout->addLayout(searchLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    if (userRole == "VENDEUR") {
        btnOrder = new QPushButton("🛒 Commander", this);
        btnOrder->setMinimumHeight(48);
        btnOrder->setCursor(Qt::PointingHandCursor);
        btnOrder->setStyleSheet(QString(
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
            "   stop:0 %1, stop:1 %2);"
            "   color: white;"
            "   border: none;"
            "   border-radius: 12px;"
            "   padding: 12px 32px;"
            "   font-size: 15px;"
            "   font-weight: 600;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
            "   stop:0 %2, stop:1 %3);"
            "}"
            "QPushButton:pressed {"
            "   background: %3;"
            "}"
        ).arg(theme.warningColor().name(), 
              theme.primaryColor().name(),
              theme.primaryHoverColor().name()));
        connect(btnOrder, &QPushButton::clicked, this, &ProductsPage::onOrderProduct);
        buttonLayout->addWidget(btnOrder);
    } else {
        btnAdd = new QPushButton("+ Ajouter", this);
        btnAdd->setMinimumHeight(48);
        btnAdd->setCursor(Qt::PointingHandCursor);
        btnAdd->setStyleSheet(QString(
            "QPushButton {"
            "   background: %1;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 12px;"
            "   padding: 12px 32px;"
            "   font-size: 15px;"
            "   font-weight: 600;"
            "}"
            "QPushButton:hover {"
            "   background: %2;"
            "}"
            "QPushButton:pressed {"
            "   background: %3;"
            "}"
        ).arg(theme.primaryColor().name(),
              theme.primaryHoverColor().name(),
              theme.primaryPressedColor().name()));
        connect(btnAdd, &QPushButton::clicked, this, &ProductsPage::onAddProduct);
        buttonLayout->addWidget(btnAdd);
    }

    btnRefresh = new QPushButton("🔄 Actualiser", this);
    btnRefresh->setMinimumHeight(48);
    btnRefresh->setCursor(Qt::PointingHandCursor);
    btnRefresh->setStyleSheet(QString(
        "QPushButton {"
        "   background: %1;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 12px 32px;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: %2;"
        "}"
        "QPushButton:pressed {"
        "   background: %3;"
        "}"
    ).arg(theme.primaryColor().name(),
          theme.primaryHoverColor().name(),
          theme.primaryPressedColor().name()));
    connect(btnRefresh, &QPushButton::clicked, this, &ProductsPage::loadProducts);

    buttonLayout->addWidget(btnRefresh);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(QString(
        "QScrollArea {"
        "   border: none;"
        "   background: transparent;"
        "}"
        "QScrollBar:vertical {"
        "   background: %1;"
        "   width: 12px;"
        "   border-radius: 6px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: %2;"
        "   border-radius: 6px;"
        "   min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: %3;"
        "}"
    ).arg(theme.surfaceAltColor().name(),
          theme.primaryColor().name(),
          theme.primaryHoverColor().name()));

    productsContainer = new QWidget();
    productsContainer->setStyleSheet("background: transparent;");
    productsGrid = new QGridLayout(productsContainer);
    productsGrid->setSpacing(24);
    productsGrid->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(productsContainer);
    mainLayout->addWidget(scrollArea);
}

void ProductsPage::applyStyles()
{
    ThemeManager& theme = ThemeManager::instance();
    setStyleSheet(QString(
        "#productsPage {"
        "   background: %1;"
        "}"
    ).arg(theme.backgroundColor().name()));
}

QWidget* ProductsPage::createProductCard(int productId, const QString &nom, const QString &description,
                                       const QString &imagePath, double prixVente, int stock, int seuilAlerte)
{
    ThemeManager& theme = ThemeManager::instance();
    
    QWidget *card = new QWidget();
    card->setFixedSize(280, 360);
    
    QString cardGradient;
    QString cardGradientHover;
    QString shadowStyle;
    
    if (theme.currentTheme() == ThemeManager::LightMode) {
        // Mode clair - gradients modernes avec ombres douces
        cardGradient = "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ffffff, stop:1 #fafbfc)";
        cardGradientHover = "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #fcfdfe, stop:1 #f5f7fa)";
        shadowStyle = "0px 4px 16px rgba(0, 0, 0, 0.08)";
    } else {
        // Mode sombre - gradients vibrants avec style professionnel
        cardGradient = "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1a2540, stop:1 #0f172a)";
        cardGradientHover = "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1f2d4a, stop:1 #151f36)";
        shadowStyle = "0px 4px 24px rgba(0, 0, 0, 0.32)";
    }
    
    card->setStyleSheet(
        QString("QWidget {"
        "   background: %1;"
        "   border: 1px solid %2;"
        "   border-radius: 16px;"
        "   outline: none;"
        "}"
        "QWidget:hover {"
        "   background: %3;"
        "   border: 1px solid %4;"
        "   outline: none;"
        "}").arg(
            cardGradient, 
            (theme.currentTheme() == ThemeManager::LightMode) ? "#e5e7eb" : "#2a3f5f",
            cardGradientHover,
            (theme.currentTheme() == ThemeManager::LightMode) ? "#d1d5db" : "#3a5080"
        )
    );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(8, 8, 8, 8);
    cardLayout->setSpacing(0);

    QWidget *innerCard = new QWidget();
    QString innerBg = (theme.currentTheme() == ThemeManager::LightMode) 
        ? "white" 
        : theme.surfaceAltColor().name();
    innerCard->setStyleSheet(QString(
        "background: %1;"
        "border-radius: 12px;"
        "border: none;"
    ).arg(innerBg));
    
    QVBoxLayout *innerLayout = new QVBoxLayout(innerCard);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(0);

    QLabel *imageLabel = new QLabel(innerCard);
    imageLabel->setFixedSize(264, 160);
    imageLabel->setStyleSheet(
        "border-top-left-radius: 12px;"
        "border-top-right-radius: 12px;"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #f0f4ff, stop:0.5 #e8f2ff, stop:1 #f3e8ff);"
    );

    if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
        QPixmap pixmap(imagePath);
        imageLabel->setPixmap(pixmap.scaled(264, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setAlignment(Qt::AlignCenter);
    } else {
        imageLabel->setText("📦");
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setStyleSheet(
            "border-top-left-radius: 12px;"
            "border-top-right-radius: 12px;"
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "stop:0 #f0f4ff, stop:0.5 #e8f2ff, stop:1 #f3e8ff);"
            "color: #cbd5e0;"
            "font-size: 48px;"
        );
    }

    innerLayout->addWidget(imageLabel);

    QWidget *contentWidget = new QWidget();
    QString contentBgColor = (theme.currentTheme() == ThemeManager::LightMode) 
        ? "white" 
        : theme.surfaceColor().name();
    contentWidget->setStyleSheet(QString(
        "background: %1; border-bottom-left-radius: 12px; border-bottom-right-radius: 12px;").arg(contentBgColor));
    
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(16, 16, 16, 14);
    contentLayout->setSpacing(8);

    QLabel *nameLabel = new QLabel(nom, contentWidget);
    
    QString nameColor = (theme.currentTheme() == ThemeManager::LightMode) 
        ? "#1f2937"
        : theme.textColor().name();
    
    nameLabel->setStyleSheet(
        QString("font-size: 15px;"
        "font-weight: 700;"
        "color: %1;"
        "margin: 0;"
        "line-height: 1.4;").arg(nameColor)
    );
    nameLabel->setWordWrap(true);
    nameLabel->setMaximumHeight(40);
    contentLayout->addWidget(nameLabel);

    QLabel *priceLabel = new QLabel(QString("€%1").arg(QString::number(prixVente, 'f', 2)), contentWidget);
    priceLabel->setStyleSheet(
        QString("font-size: 20px;"
        "font-weight: 800;"
        "color: %1;"
        "margin: 2px 0px 0px 0px;").arg(nameColor)
    );
    contentLayout->addWidget(priceLabel);

    QString stockColor = stock <= seuilAlerte ? "#dc2626" : (stock == 0 ? "#dc2626" : "#10b981");
    QString stockBg = stock <= seuilAlerte ? 
        "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fee2e2, stop:1 #fecaca)" : 
        (stock == 0 ? "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fee2e2, stop:1 #fecaca)" : 
         "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #d1fae5, stop:1 #a7f3d0)");
    QString stockText = stock == 0 ? "Rupture" : QString("%1 en stock").arg(stock);

    QWidget *stockWidget = new QWidget(contentWidget);
    stockWidget->setStyleSheet(QString(
        "background: %1;"
        "border-radius: 8px;"
        "padding: 6px 12px;"
    ).arg(stockBg));
    stockWidget->setMaximumWidth(130);
    
    QHBoxLayout *stockLayout = new QHBoxLayout(stockWidget);
    stockLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *stockLabel = new QLabel(stockText, stockWidget);
    stockLabel->setStyleSheet(QString(
        "font-size: 13px;"
        "font-weight: 600;"
        "color: %1;"
        "margin: 0;"
        "background: transparent;"
        "letter-spacing: 0.3px;"
    ).arg(stockColor));
    stockLayout->addWidget(stockLabel);
    
    contentLayout->addWidget(stockWidget);

    QLabel *descLabel = new QLabel(description, contentWidget);
    
    QString descColor = (theme.currentTheme() == ThemeManager::LightMode)
        ? "#6b7280"
        : theme.textSecondaryColor().name();
    
    descLabel->setStyleSheet(
        QString("font-size: 12px;"
        "color: %1;"
        "margin: 2px 0px 0px 0px;"
        "line-height: 1.5;").arg(descColor)
    );
    descLabel->setWordWrap(true);
    descLabel->setMaximumHeight(60);
    contentLayout->addWidget(descLabel);

    contentLayout->addStretch();

    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setContentsMargins(0, 4, 0, 0);
    actionLayout->setSpacing(6);

    if (userRole != "VENDEUR") {
        QPushButton *editBtn = new QPushButton("✏️", contentWidget);
        editBtn->setFixedSize(32, 32);
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setToolTip("Modifier");
        editBtn->setStyleSheet(
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #667eea, stop:1 #764ba2);"
            "   color: white;"
            "   border: none;"
            "   border-radius: 8px;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   padding: 0px;"
            "   outline: none;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #5568d3, stop:1 #6a3a8a);"
            "   outline: none;"
            "}"
            "QPushButton:pressed {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #4556b8, stop:1 #5a2a7a);"
            "   outline: none;"
            "}"
        );
        connect(editBtn, &QPushButton::clicked, this, [this, productId]() { onEditProduct(productId); });

        QPushButton *deleteBtn = new QPushButton("🗑️", contentWidget);
        deleteBtn->setFixedSize(32, 32);
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setToolTip("Supprimer");
        deleteBtn->setStyleSheet(
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #f56565, stop:1 #e53e3e);"
            "   color: white;"
            "   border: none;"
            "   border-radius: 8px;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   padding: 0px;"
            "   outline: none;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #e53e3e, stop:1 #c53030);"
            "   outline: none;"
            "}"
            "QPushButton:pressed {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #c53030, stop:1 #742a2a);"
            "   outline: none;"
            "}"
        );
        connect(deleteBtn, &QPushButton::clicked, this, [this, productId]() { onDeleteProduct(productId); });

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(deleteBtn);
    } else {
        QWidget *quantityWidget = new QWidget(contentWidget);
        QHBoxLayout *quantityLayout = new QHBoxLayout(quantityWidget);
        quantityLayout->setContentsMargins(0, 0, 0, 0);
        quantityLayout->setSpacing(6);

        QPushButton *minusBtn = new QPushButton("−", quantityWidget);
        minusBtn->setFixedSize(32, 32);
        minusBtn->setCursor(Qt::PointingHandCursor);
        minusBtn->setStyleSheet(
            "QPushButton {"
            "   background: #e5e7eb;"
            "   color: #374151;"
            "   border: none;"
            "   border-radius: 8px;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   outline: none;"
            "}"
            "QPushButton:hover {"
            "   background: #d1d5db;"
            "   outline: none;"
            "}"
            "QPushButton:pressed {"
            "   background: #9ca3af;"
            "   outline: none;"
            "}"
        );

        QLabel *quantityLabel = new QLabel("0", quantityWidget);
        quantityLabel->setStyleSheet(
            "font-size: 14px;"
            "font-weight: 700;"
            "color: #1f2937;"
            "min-width: 40px;"
            "text-align: center;"
        );
        quantityLabel->setAlignment(Qt::AlignCenter);

        QPushButton *plusBtn = new QPushButton("+", quantityWidget);
        plusBtn->setFixedSize(32, 32);
        plusBtn->setCursor(Qt::PointingHandCursor);
        plusBtn->setStyleSheet(
            "QPushButton {"
            "   background: #e5e7eb;"
            "   color: #374151;"
            "   border: none;"
            "   border-radius: 8px;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   outline: none;"
            "}"
            "QPushButton:hover {"
            "   background: #d1d5db;"
            "   outline: none;"
            "}"
            "QPushButton:pressed {"
            "   background: #9ca3af;"
            "   outline: none;"
            "}"
        );

        quantityLayout->addWidget(minusBtn);
        quantityLayout->addWidget(quantityLabel);
        quantityLayout->addWidget(plusBtn);

        connect(minusBtn, &QPushButton::clicked, this, [this, productId, quantityLabel]() {
            if (orderDialog) {
                orderDialog->removeProduct(productId, 1);
                int currentQty = quantityLabel->text().toInt();
                if (currentQty > 0) {
                    quantityLabel->setText(QString::number(currentQty - 1));
                }
            }
        });

        connect(plusBtn, &QPushButton::clicked, this, [this, productId, quantityLabel, stock, nom, prixVente]() {
            if (orderDialog) {
                int currentQty = quantityLabel->text().toInt();
                if (currentQty + 1 <= stock) {
                    orderDialog->addProduct(productId, nom, prixVente, currentQty + 1);
                    quantityLabel->setText(QString::number(currentQty + 1));
                } else {
                    QMessageBox::warning(this, "Stock insuffisant", 
                                       QString("Il n'y a pas assez de stock pour ce produit. Stock disponible : %1").arg(stock));
                }
            }
        });

        contentLayout->addWidget(quantityWidget);

        actionLayout->addStretch();
    }

    contentLayout->addLayout(actionLayout);
    innerLayout->addWidget(contentWidget);
    
    cardLayout->addWidget(innerCard);

    return card;
}

void ProductsPage::loadProducts()
{
    QLayoutItem *child;
    while ((child = productsGrid->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QString filter = "1=1";
    QString searchText = searchInput->text();
    if (!searchText.isEmpty()) {
        filter += QString(" AND (nom_produit LIKE '%%1%' OR description LIKE '%%1%')").arg(searchText);
    }

    QSqlQuery query;
    query.prepare(QString("SELECT id_produit, nom_produit, description, photo_produit, prix_vente, stock, seuil_alerte FROM PRODUITS WHERE %1 ORDER BY date_creation DESC").arg(filter));

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des produits: " + query.lastError().text());
        return;
    }

    int row = 0;
    int col = 0;
    const int maxCols = 3;

    while (query.next()) {
        int productId = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString description = query.value(2).toString();
        QString imagePath = query.value(3).toString();
        double prixVente = query.value(4).toDouble();
        int stock = query.value(5).toInt();
        int seuilAlerte = query.value(6).toInt();

        QWidget *card = createProductCard(productId, nom, description, imagePath, prixVente, stock, seuilAlerte);
        productsGrid->addWidget(card, row, col);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }

    while (col > 0 && col < maxCols) {
        QWidget *spacer = new QWidget();
        spacer->setFixedSize(280, 360);
        productsGrid->addWidget(spacer, row, col);
        col++;
    }
}

void ProductsPage::onAddProduct()
{
    ProductDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        loadProducts();
    }
}

void ProductsPage::onEditProduct(int productId)
{
    ProductDialog dialog(this, productId);
    if (dialog.exec() == QDialog::Accepted) {
        loadProducts();
    }
}

void ProductsPage::onDeleteProduct(int productId)
{
    QSqlQuery query;
    query.prepare("SELECT nom_produit FROM PRODUITS WHERE id_produit = :id");
    query.bindValue(":id", productId);
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Erreur", "Produit introuvable.");
        return;
    }
    QString nom = query.value(0).toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirmation",
        QString("Voulez-vous vraiment supprimer le produit '%1' ?").arg(nom),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        query.prepare("DELETE FROM PRODUITS WHERE id_produit = :id");
        query.bindValue(":id", productId);
        if (query.exec()) {
            QMessageBox::information(this, "Succès", "Produit supprimé avec succès.");
            loadProducts();
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression: " + query.lastError().text());
        }
    }
}

void ProductsPage::onSearchTextChanged(const QString &text)
{
    loadProducts();
}

void ProductsPage::onOrderProduct()
{
    if (orderDialog) {
        if (!orderDialog->checkStocks()) {
            QMessageBox::warning(this, "Stock insuffisant", 
                               "Un ou plusieurs produits dans votre commande n'ont pas assez de stock disponible.");
            return;
        }
        orderDialog->resetUI();
        orderDialog->exec();
    }
}
