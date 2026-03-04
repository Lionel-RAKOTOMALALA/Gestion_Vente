#include "productspage.h"
#include "productdialog.h"
#include "stockmovementpage.h"
#include "dashboardpage.h"
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
#include <QGraphicsEffect>

ProductsPage::ProductsPage(const QString &userRole, int userId, QWidget *parent, StockMovementPage *stockPage, DashboardPage *dashPage) : QFrame(parent), userRole(userRole), userId(userId), stockMovementPage(stockPage), dashboardPage(dashPage)
{
    setObjectName("productsPage");
    setupDatabase();
    setupUI();
    applyStyles();
    loadProducts();
    
    if (userRole == "VENDEUR") {
        orderDialog = new OrderDialog(userId, this, stockMovementPage, dashboardPage);
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
    mainLayout->setSpacing(32);
    mainLayout->setContentsMargins(48, 48, 48, 48);

    // Header avec icon
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *icon = new QLabel(this);
    icon->setStyleSheet(
        "background: #6366f1;"
        "border-radius: 16px;"
        "min-width: 64px; max-width: 64px;"
        "min-height: 64px; max-height: 64px;"
    );

    QLabel *title = new QLabel("Gestion des Produits", this);
    title->setObjectName("titleH1");
    ThemeManager& theme = ThemeManager::instance();
    title->setStyleSheet(QString(
        "font-size: 36px;"
        "font-weight: 800;"
        "color: %1;"
        "letter-spacing: -0.8px;"
    ).arg(theme.textColor().name()));

    QLabel *subtitle = new QLabel("Gérez votre catalogue de produits", this);
    subtitle->setStyleSheet(QString(
        "font-size: 15px;"
        "color: %1;"
        "font-weight: 400;"
    ).arg(theme.textSecondaryColor().name()));

    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(4);
    titleLayout->addWidget(title);
    titleLayout->addWidget(subtitle);
    titleLayout->addStretch();

    headerLayout->addWidget(icon);
    headerLayout->addLayout(titleLayout, 1);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    // Search et Actions
    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->setSpacing(16);

    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("🔍 Rechercher par nom ou description...");
    searchInput->setMinimumHeight(52);
    searchInput->setStyleSheet(QString(
        "QLineEdit {"
        "   border: 2px solid %1;"
        "   border-radius: 14px;"
        "   padding: 14px 24px;"
        "   font-size: 15px;"
        "   background: %2;"
        "   color: %3;"
        "   font-weight: 500;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid %4;"
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

    controlsLayout->addWidget(searchInput);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(14);

    if (userRole == "VENDEUR") {
        btnOrder = new QPushButton("🛒 Commander", this);
        btnOrder->setMinimumHeight(52);
        btnOrder->setMinimumWidth(160);
        btnOrder->setCursor(Qt::PointingHandCursor);
        btnOrder->setStyleSheet(QString(
            "QPushButton {"
            "   background: %1;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 14px;"
            "   padding: 14px 36px;"
            "   font-size: 15px;"
            "   font-weight: 700;"
            "}"
            "QPushButton:hover {"
            "   background: %2;"
            "   margin: 0px;"
            "}"
            "QPushButton:pressed {"
            "   background: %3;"
            "}"
        ).arg(theme.primaryColor().name(), 
              theme.primaryHoverColor().name(),
              theme.primaryHoverColor().name()));
        connect(btnOrder, &QPushButton::clicked, this, &ProductsPage::onOrderProduct);
        buttonLayout->addWidget(btnOrder);
    } else {
        btnAdd = new QPushButton("✨ Ajouter Produit", this);
        btnAdd->setMinimumHeight(52);
        btnAdd->setMinimumWidth(180);
        btnAdd->setCursor(Qt::PointingHandCursor);
        btnAdd->setStyleSheet(QString(
            "QPushButton {"
            "   background: %1;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 14px;"
            "   padding: 14px 36px;"
            "   font-size: 15px;"
            "   font-weight: 700;"
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
    btnRefresh->setMinimumHeight(52);
    btnRefresh->setMinimumWidth(140);
    btnRefresh->setCursor(Qt::PointingHandCursor);
    btnRefresh->setStyleSheet(QString(
        "QPushButton {"
        "   background: %1;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 14px;"
        "   padding: 14px 32px;"
        "   font-size: 15px;"
        "   font-weight: 700;"
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

    controlsLayout->addLayout(buttonLayout);
    mainLayout->addLayout(controlsLayout);

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
        "   width: 14px;"
        "   border-radius: 7px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: %2;"
        "   border-radius: 7px;"
        "   min-height: 40px;"
        "   margin: 4px 0px;"
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
    productsGrid->setSpacing(28);
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
    card->setFixedSize(260, 330);
    
    // Ombres professionnelles
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
    if (theme.currentTheme() == ThemeManager::LightMode) {
        shadowEffect->setBlurRadius(16);
        shadowEffect->setXOffset(0);
        shadowEffect->setYOffset(8);
        shadowEffect->setColor(QColor(0, 0, 0, 20));
    } else {
        shadowEffect->setBlurRadius(20);
        shadowEffect->setXOffset(0);
        shadowEffect->setYOffset(12);
        shadowEffect->setColor(QColor(0, 0, 0, 40));
    }
    card->setGraphicsEffect(shadowEffect);
    
    QString cardGradient;
    QString cardBorder;
    
    if (theme.currentTheme() == ThemeManager::LightMode) {
        cardGradient = "#ffffff";
        cardBorder = "1px solid #e5e7eb";
    } else {
        cardGradient = "#1f2937";
        cardBorder = "1px solid #374151";
    }
    
    card->setStyleSheet(
        QString("QWidget {"
        "   background: %1;"
        "   border: none;"
        "   border-radius: 18px;"
        "   outline: none;"
        "   padding: 0px;"
        "}").arg(cardGradient)
    );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    // Image section
    QLabel *imageLabel = new QLabel(card);
    imageLabel->setFixedSize(260, 130);
    imageLabel->setStyleSheet(
        "background: #f0f4ff;"
        "border-radius: 16px 16px 0px 0px;"
    );

    if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
        QPixmap pixmap(imagePath);
        imageLabel->setPixmap(pixmap.scaled(260, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setAlignment(Qt::AlignCenter);
    } else {
        imageLabel->setText("📦");
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setStyleSheet(
            "background: #f0f4ff;"
            "color: #cbd5e0;"
            "font-size: 48px;"
            "border-radius: 16px 16px 0px 0px;"
        );
    }

    cardLayout->addWidget(imageLabel);

    // Content section
    QWidget *contentWidget = new QWidget();
    QString contentBgColor = (theme.currentTheme() == ThemeManager::LightMode) 
        ? "#ffffff" 
        : theme.surfaceColor().name();
    contentWidget->setStyleSheet(QString(
        "background: %1; border: none;").arg(contentBgColor));
    
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(12, 10, 12, 8);
    contentLayout->setSpacing(4);

    // Product name
    QLabel *nameLabel = new QLabel(nom, contentWidget);
    QString nameColor = (theme.currentTheme() == ThemeManager::LightMode) 
        ? "#111827"
        : theme.textColor().name();
    
    nameLabel->setStyleSheet(
        QString("font-size: 15px;"
        "font-weight: 800;"
        "color: %1;"
        "margin: 0;"
        "line-height: 1.2;").arg(nameColor)
    );
    nameLabel->setWordWrap(true);
    nameLabel->setMaximumHeight(24);
    contentLayout->addWidget(nameLabel);

    // Price
    QLabel *priceLabel = new QLabel(QString("%1 Ar").arg(QString::number(prixVente, 'f', 2)), contentWidget);
    priceLabel->setMinimumHeight(32);
    priceLabel->setStyleSheet(
        QString("font-size: 24px;"
        "font-weight: 900;"
        "color: %1;"
        "margin: 2px 0px 4px 0px;"
        "line-height: 1.1;"
        "padding: 0px;").arg(theme.primaryColor().name())
    );
    priceLabel->setAlignment(Qt::AlignVCenter);
    contentLayout->addWidget(priceLabel);

    // Stock badge
    QString stockColor = stock <= seuilAlerte ? "#dc2626" : (stock == 0 ? "#dc2626" : "#10b981");
    QString stockBg = stock <= seuilAlerte ? 
        "#fecaca" : 
        (stock == 0 ? "#fecaca" : 
         "#bbf7d0");
    QString stockText = stock == 0 ? "⚠️ Rupture" : QString("✓ %1").arg(stock);
    QWidget *stockWidget = new QWidget(contentWidget);
    stockWidget->setStyleSheet(QString(
        "background: %1;"
        "border: none;"
        "border-radius: 6px;"
        "padding: 6px 10px;"
    ).arg(stockBg));
    stockWidget->setMaximumWidth(110);
    
    QHBoxLayout *stockLayout = new QHBoxLayout(stockWidget);
    stockLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *stockLabel = new QLabel(stockText, stockWidget);
    stockLabel->setStyleSheet(QString(
        "font-size: 12px;"
        "font-weight: 700;"
        "color: %1;"
        "margin: 0;"
        "background: transparent;"
    ).arg(stockColor));
    stockLayout->addWidget(stockLabel);
    
    contentLayout->addWidget(stockWidget);

    // Description
    QLabel *descLabel = new QLabel(description, contentWidget);
    QString descColor = (theme.currentTheme() == ThemeManager::LightMode)
        ? "#6b7280"
        : theme.textSecondaryColor().name();
    
    descLabel->setStyleSheet(
        QString("font-size: 12px;"
        "color: %1;"
        "margin: 2px 0px 0px 0px;"
        "line-height: 1.3;").arg(descColor)
    );
    descLabel->setWordWrap(true);
    descLabel->setMaximumHeight(48);
    contentLayout->addWidget(descLabel);

    contentLayout->addStretch();

    // Action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setContentsMargins(0, 4, 0, 0);
    actionLayout->setSpacing(8);

    if (userRole != "VENDEUR") {
        QPushButton *editBtn = new QPushButton("✏️ Éditer", contentWidget);
        editBtn->setFixedHeight(34);
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setToolTip("Modifier le produit");
        editBtn->setStyleSheet(
            "QPushButton {"
            "   background: #667eea;"
            "   color: white;"
            "   border: none;"
            "   font-size: 12px;"
            "   font-weight: 700;"
            "   padding: 0px 12px;"
            "   outline: none;"
            "   border-radius: 6px;"
            "}"
            "QPushButton:hover {"
            "   background: #5568d3;"
            "   outline: none;"
            "}"
            "QPushButton:pressed {"
            "   background: #4556b8;"
            "   outline: none;"
            "}"
        );
        connect(editBtn, &QPushButton::clicked, this, [this, productId]() { onEditProduct(productId); });

        QPushButton *deleteBtn = new QPushButton("🗑️ Supprimer", contentWidget);
        deleteBtn->setFixedHeight(34);
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setToolTip("Supprimer le produit");
        deleteBtn->setStyleSheet(
            "QPushButton {"
            "   background: #f56565;"
            "   color: white;"
            "   border: none;"
            "   font-size: 12px;"
            "   font-weight: 700;"
            "   padding: 0px 12px;"
            "   outline: none;"
            "   border-radius: 6px;"
            "}"
            "QPushButton:hover {"
            "   background: #e53e3e;"
            "   outline: none;"
            "}"
            "QPushButton:pressed {"
            "   background: #c53030;"
            "   outline: none;"
            "}"
        );
        connect(deleteBtn, &QPushButton::clicked, this, [this, productId]() { onDeleteProduct(productId); });

        actionLayout->addWidget(editBtn, 1);
        actionLayout->addWidget(deleteBtn, 1);
    } else {
        QWidget *quantityWidget = new QWidget(contentWidget);
        QHBoxLayout *quantityLayout = new QHBoxLayout(quantityWidget);
        quantityLayout->setContentsMargins(0, 0, 0, 0);
        quantityLayout->setSpacing(8);

        QPushButton *minusBtn = new QPushButton("−", quantityWidget);
        minusBtn->setFixedSize(28, 28);
        minusBtn->setCursor(Qt::PointingHandCursor);
        minusBtn->setStyleSheet(
            "QPushButton {"
            "   background: #e5e7eb;"
            "   color: #374151;"
            "   border: none;"
            "   border-radius: 6px;"
            "   font-size: 14px;"
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
        quantityLabel->setFixedHeight(28);
        quantityLabel->setMinimumWidth(40);
        QString qtyColor = (theme.currentTheme() == ThemeManager::LightMode) 
            ? "#1f2937"
            : theme.textColor().name();
        quantityLabel->setStyleSheet(
            QString("font-size: 13px;"
            "font-weight: 800;"
            "color: %1;"
            "text-align: center;").arg(qtyColor)
        );
        quantityLabel->setAlignment(Qt::AlignCenter);

        QPushButton *plusBtn = new QPushButton("+", quantityWidget);
        plusBtn->setFixedSize(28, 28);
        plusBtn->setCursor(Qt::PointingHandCursor);
        plusBtn->setStyleSheet(
            "QPushButton {"
            "   background: #e5e7eb;"
            "   color: #374151;"
            "   border: none;"
            "   border-radius: 6px;"
            "   font-size: 14px;"
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
        quantityLayout->addStretch();
        quantityLayout->addWidget(quantityLabel);
        quantityLayout->addStretch();
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
    cardLayout->addWidget(contentWidget);

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
        spacer->setFixedSize(270, 360);
        productsGrid->addWidget(spacer, row, col);
        col++;
    }
}

void ProductsPage::onAddProduct()
{
    ProductDialog dialog(this, -1, userId, stockMovementPage, dashboardPage);
    if (dialog.exec() == QDialog::Accepted) {
        loadProducts();
        if (dashboardPage) {
            dashboardPage->onDataChanged();
        }
    }
}

void ProductsPage::onEditProduct(int productId)
{
    ProductDialog dialog(this, productId, userId, stockMovementPage, dashboardPage);
    if (dialog.exec() == QDialog::Accepted) {
        loadProducts();
        if (dashboardPage) {
            dashboardPage->onDataChanged();
        }
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
            if (dashboardPage) {
                dashboardPage->onDataChanged();
            }
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
