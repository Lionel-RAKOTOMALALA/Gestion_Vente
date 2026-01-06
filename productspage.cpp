#include "productspage.h"
#include "productdialog.h"
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
    
    // Initialiser le modal de commande seulement pour les vendeurs
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
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *icon = new QLabel(this);
    icon->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #667eea, stop:1 #764ba2);"
        "border-radius: 12px;"
        "min-width: 48px; max-width: 48px;"
        "min-height: 48px; max-height: 48px;"
    );

    QLabel *title = new QLabel("Gestion des Produits", this);
    title->setObjectName("titleH1");
    title->setStyleSheet(
        "font-size: 32px;"
        "font-weight: 700;"
        "color: #1a202c;"
        "letter-spacing: -0.5px;"
    );

    headerLayout->addWidget(icon);
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(16);

    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Rechercher par nom ou description...");
    searchInput->setMinimumHeight(48);
    searchInput->setStyleSheet(
        "QLineEdit {"
        "   border: 1px solid #e2e8f0;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 15px;"
        "   background: white;"
        "   color: #2d3748;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #667eea;"
        "   outline: none;"
        "}"
        "QLineEdit::placeholder {"
        "   color: #a0aec0;"
        "}"
    );
    connect(searchInput, &QLineEdit::textChanged, this, &ProductsPage::onSearchTextChanged);

    searchLayout->addWidget(searchInput, 1);
    mainLayout->addLayout(searchLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    if (userRole == "VENDEUR") {
        btnOrder = new QPushButton("🛒 Commander", this);
        btnOrder->setMinimumHeight(48);
        btnOrder->setCursor(Qt::PointingHandCursor);
        btnOrder->setStyleSheet(
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
            "   stop:0 #f59e0b, stop:1 #d97706);"
            "   color: white;"
            "   border: none;"
            "   border-radius: 12px;"
            "   padding: 12px 32px;"
            "   font-size: 15px;"
            "   font-weight: 600;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
            "   stop:0 #d97706, stop:1 #b45309);"
            "}"
            "QPushButton:pressed {"
            "   background: #b45309;"
            "}"
        );
        connect(btnOrder, &QPushButton::clicked, this, &ProductsPage::onOrderProduct);
        buttonLayout->addWidget(btnOrder);
    } else {
        btnAdd = new QPushButton("+ Ajouter", this);
        btnAdd->setMinimumHeight(48);
        btnAdd->setCursor(Qt::PointingHandCursor);
        btnAdd->setStyleSheet(
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #3b82f6, stop:1 #2563eb);"
            "   color: white;"
            "   border: none;"
            "   border-radius: 12px;"
            "   padding: 12px 32px;"
            "   font-size: 15px;"
            "   font-weight: 600;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #2563eb, stop:1 #1d4ed8);"
            "}"
            "QPushButton:pressed {"
            "   background: #1d4ed8;"
            "}"
        );
        connect(btnAdd, &QPushButton::clicked, this, &ProductsPage::onAddProduct);
        buttonLayout->addWidget(btnAdd);
    }

    btnRefresh = new QPushButton("Actualiser", this);
    btnRefresh->setMinimumHeight(48);
    btnRefresh->setCursor(Qt::PointingHandCursor);
    btnRefresh->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #3b82f6, stop:1 #2563eb);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 12px 32px;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #2563eb, stop:1 #1d4ed8);"
        "}"
        "QPushButton:pressed {"
        "   background: #1d4ed8;"
        "}"
    );
    connect(btnRefresh, &QPushButton::clicked, this, &ProductsPage::loadProducts);

    buttonLayout->addWidget(btnRefresh);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    // Zone de scroll pour les cartes de produits
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "   border: none;"
        "   background: transparent;"
        "}"
        "QScrollBar:vertical {"
        "   background: #f7fafc;"
        "   width: 12px;"
        "   border-radius: 6px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "   stop:0 #667eea, stop:1 #764ba2);"
        "   border-radius: 6px;"
        "   min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "   stop:0 #5568d3, stop:1 #6b3f8f);"
        "}"
    );

    productsContainer = new QWidget();
    productsContainer->setStyleSheet("background: transparent;");
    productsGrid = new QGridLayout(productsContainer);
    productsGrid->setSpacing(20);
    productsGrid->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(productsContainer);
    mainLayout->addWidget(scrollArea);
}

void ProductsPage::applyStyles()
{
    setStyleSheet(
        "#productsPage {"
        "   background: #f7fafc;"
        "}"
    );
}

QWidget* ProductsPage::createProductCard(int productId, const QString &nom, const QString &description,
                                       const QString &imagePath, double prixVente, int stock, int seuilAlerte)
{
    QWidget *card = new QWidget();
    card->setFixedSize(280, 360);
    card->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #667eea, stop:0.5 #764ba2, stop:1 #f093fb);"
        "   border: none;"
        "   border-radius: 16px;"
        "}"
        "QWidget:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #5568d3, stop:0.5 #6b3f8f, stop:1 #e082e8);"
        "}"
    );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(3, 3, 3, 3);
    cardLayout->setSpacing(0);

    QWidget *innerCard = new QWidget();
    innerCard->setStyleSheet(
        "background: white;"
        "border-radius: 13px;"
    );
    QVBoxLayout *innerLayout = new QVBoxLayout(innerCard);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(0);

    QLabel *imageLabel = new QLabel(innerCard);
    imageLabel->setFixedSize(274, 160);
    imageLabel->setStyleSheet(
        "border-top-left-radius: 13px;"
        "border-top-right-radius: 13px;"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #fef3f4, stop:1 #f3e7ff);"
    );

    if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
        QPixmap pixmap(imagePath);
        imageLabel->setPixmap(pixmap.scaled(274, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setAlignment(Qt::AlignCenter);
    } else {
        imageLabel->setText("📦");
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setStyleSheet(
            "border-top-left-radius: 13px;"
            "border-top-right-radius: 13px;"
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "stop:0 #fef3f4, stop:1 #f3e7ff);"
            "color: #cbd5e0;"
            "font-size: 48px;"
        );
    }

    innerLayout->addWidget(imageLabel);

    QWidget *contentWidget = new QWidget();
    contentWidget->setStyleSheet("background: white; border-bottom-left-radius: 13px; border-bottom-right-radius: 13px;");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(16, 14, 16, 14);
    contentLayout->setSpacing(6);

    QLabel *nameLabel = new QLabel(nom, contentWidget);
    nameLabel->setStyleSheet(
        "font-size: 16px;"
        "font-weight: 700;"
        "color: #1a202c;"
        "margin: 0;"
    );
    nameLabel->setWordWrap(true);
    nameLabel->setMaximumHeight(40);
    contentLayout->addWidget(nameLabel);

    QLabel *priceLabel = new QLabel(QString("€%1").arg(QString::number(prixVente, 'f', 2)), contentWidget);
    priceLabel->setStyleSheet(
        "font-size: 18px;"
        "font-weight: 800;"
        "color: #1a202c;"
        "margin: 0;"
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
        "padding: 4px 10px;"
    ).arg(stockBg));
    stockWidget->setMaximumWidth(120);
    
    QHBoxLayout *stockLayout = new QHBoxLayout(stockWidget);
    stockLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *stockLabel = new QLabel(stockText, stockWidget);
    stockLabel->setStyleSheet(QString(
        "font-size: 12px;"
        "font-weight: 600;"
        "color: %1;"
        "margin: 0;"
        "background: transparent;"
    ).arg(stockColor));
    stockLayout->addWidget(stockLabel);
    
    contentLayout->addWidget(stockWidget);

    QLabel *descLabel = new QLabel(description, contentWidget);
    descLabel->setStyleSheet(
        "font-size: 12px;"
        "color: #718096;"
        "margin: 0;"
        "line-height: 1.3;"
    );
    descLabel->setWordWrap(true);
    descLabel->setMaximumHeight(60); // Augmenter la hauteur pour voir plus de texte
    contentLayout->addWidget(descLabel);

    contentLayout->addStretch();

    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setContentsMargins(4, 4, 4, 4);
    actionLayout->setSpacing(6);

    if (userRole != "VENDEUR") {
        QPushButton *editBtn = new QPushButton("✏️", contentWidget);
        editBtn->setFixedSize(24, 24);
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setToolTip("Modifier");
        editBtn->setStyleSheet(
            "QPushButton {"
            "   background: #3b82f6;"
            "   color: white;"
            "   border: 2px solid #3b82f6;"
            "   border-radius: 6px;"
            "   font-size: 12px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background: #2563eb;"
            "   border-color: #2563eb;"
            "   transform: scale(1.1);"
            "}"
            "QPushButton:pressed {"
            "   background: #1d4ed8;"
            "   border-color: #1d4ed8;"
            "}"
        );
        connect(editBtn, &QPushButton::clicked, this, [this, productId]() { onEditProduct(productId); });

        QPushButton *deleteBtn = new QPushButton("🗑️", contentWidget);
        deleteBtn->setFixedSize(24, 24);
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setToolTip("Supprimer");
        deleteBtn->setStyleSheet(
            "QPushButton {"
            "   background: #ef4444;"
            "   color: white;"
            "   border: 2px solid #ef4444;"
            "   border-radius: 6px;"
            "   font-size: 12px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background: #dc2626;"
            "   border-color: #dc2626;"
            "   transform: scale(1.1);"
            "}"
            "QPushButton:pressed {"
            "   background: #b91c1c;"
            "   border-color: #b91c1c;"
            "}"
        );
        connect(deleteBtn, &QPushButton::clicked, this, [this, productId]() { onDeleteProduct(productId); });

        actionLayout->addStretch();
        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(deleteBtn);
    } else {
        // Pour les vendeurs, ajouter des contrôles de quantité et bouton commander
        QWidget *quantityWidget = new QWidget(contentWidget);
        QHBoxLayout *quantityLayout = new QHBoxLayout(quantityWidget);
        quantityLayout->setContentsMargins(0, 0, 0, 0);
        quantityLayout->setSpacing(4);

        QPushButton *minusBtn = new QPushButton("-", quantityWidget);
        minusBtn->setFixedSize(24, 24);
        minusBtn->setCursor(Qt::PointingHandCursor);
        minusBtn->setStyleSheet(
            "QPushButton {"
            "   background: #e5e7eb;"
            "   color: #374151;"
            "   border: none;"
            "   border-radius: 6px;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background: #d1d5db;"
            "}"
            "QPushButton:pressed {"
            "   background: #9ca3af;"
            "}"
        );

        QLabel *quantityLabel = new QLabel("0", quantityWidget);
        quantityLabel->setStyleSheet(
            "font-size: 14px;"
            "font-weight: bold;"
            "color: #1f2937;"
            "min-width: 30px;"
            "text-align: center;"
        );
        quantityLabel->setAlignment(Qt::AlignCenter);

        QPushButton *plusBtn = new QPushButton("+", quantityWidget);
        plusBtn->setFixedSize(24, 24);
        plusBtn->setCursor(Qt::PointingHandCursor);
        plusBtn->setStyleSheet(
            "QPushButton {"
            "   background: #e5e7eb;"
            "   color: #374151;"
            "   border: none;"
            "   border-radius: 6px;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background: #d1d5db;"
            "}"
            "QPushButton:pressed {"
            "   background: #9ca3af;"
            "}"
        );

        quantityLayout->addWidget(minusBtn);
        quantityLayout->addWidget(quantityLabel);
        quantityLayout->addWidget(plusBtn);

        // Connecter les signaux pour gérer la quantité dans le panier
        connect(minusBtn, &QPushButton::clicked, this, [this, productId, nom, prixVente, quantityLabel]() {
            if (orderDialog) {
                orderDialog->removeProduct(productId, 1);
                // Mettre à jour le label avec la quantité actuelle dans le panier
                // Pour simplifier, on peut recalculer ou garder une référence
                int currentQty = quantityLabel->text().toInt();
                if (currentQty > 0) {
                    quantityLabel->setText(QString::number(currentQty - 1));
                }
            }
        });

        connect(plusBtn, &QPushButton::clicked, this, [this, productId, nom, prixVente, quantityLabel, stock]() {
            if (orderDialog) {
                int currentQty = quantityLabel->text().toInt();
                if (currentQty + 1 <= stock) {
                    orderDialog->addProduct(productId, nom, prixVente, 1);
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
