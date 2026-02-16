#include "orderdialog.h"
#include "stockmovementpage.h"
#include "dashboardpage.h"
#include "paymentdialog.h"
#include "receiptgenerator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QSpinBox>
#include <QScrollArea>

OrderDialog::OrderDialog(int userId, QWidget *parent, StockMovementPage *stockPage, DashboardPage *dashPage) :
    QDialog(parent), totalAmount(0.0), currentUserId(userId), stockMovementPage(stockPage), dashboardPage(dashPage)
{
    setWindowTitle("Nouvelle commande");
    setModal(true);
    setMinimumWidth(700);
    setMinimumHeight(500);
    setStyleSheet("background: #0f172a; color: #f1f5f9;");

    // Positionner le modal à droite de la fenêtre parente
    if (parent) {
        QPoint parentPos = parent->mapToGlobal(QPoint(0, 0));
        QSize parentSize = parent->size();
        move(parentPos.x() + parentSize.width() + 10, parentPos.y());
    }

    createTablesIfNotExist();
    setupUI();
}

OrderDialog::~OrderDialog()
{
}

OrderDialog::OrderDialog(int userId, const QString &commandeId, QWidget *parent, StockMovementPage *stockPage, DashboardPage *dashPage) :
    QDialog(parent), totalAmount(0.0), currentUserId(userId), isEditMode(true), editCommandeId(commandeId), stockMovementPage(stockPage), dashboardPage(dashPage)
{
    setWindowTitle("Modifier commande");
    setModal(true);
    setMinimumWidth(700);
    setMinimumHeight(500);
    setStyleSheet("background: #0f172a; color: #f1f5f9;");

    // Positionner le modal à droite de la fenêtre parente
    if (parent) {
        QPoint pos = parent->mapToGlobal(QPoint(parent->width() - width() - 20, 20));
        move(pos);
    }

    createTablesIfNotExist();
    setupUI();
    loadOrderForEdit(commandeId);
}

void OrderDialog::createTablesIfNotExist()
{
    QSqlQuery query;

    // Table CLIENTS
    query.exec("CREATE TABLE IF NOT EXISTS CLIENTS ("
               "id_client INTEGER PRIMARY KEY AUTOINCREMENT, "
               "nom TEXT NOT NULL, "
               "prenom TEXT, "
               "telephone TEXT, "
               "email TEXT, "
               "adresse TEXT, "
               "date_creation DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // Table COMMANDES
    query.exec("CREATE TABLE IF NOT EXISTS COMMANDES ("
               "id_commande INTEGER PRIMARY KEY AUTOINCREMENT, "
               "id_client INTEGER NOT NULL, "
               "id_user INTEGER NOT NULL, "
               "date_commande DATETIME DEFAULT CURRENT_TIMESTAMP, "
               "statut TEXT DEFAULT 'EN_COURS' CHECK(statut IN ('EN_COURS', 'PAYEE', 'ANNULEE')), "
               "total REAL DEFAULT 0, "
               "FOREIGN KEY(id_client) REFERENCES CLIENTS(id_client), "
               "FOREIGN KEY(id_user) REFERENCES USERS(id_user))");

    // Table DETAILS_COMMANDE
    query.exec("CREATE TABLE IF NOT EXISTS DETAILS_COMMANDE ("
               "id_detail INTEGER PRIMARY KEY AUTOINCREMENT, "
               "id_commande INTEGER NOT NULL, "
               "id_produit INTEGER NOT NULL, "
               "quantite INTEGER NOT NULL, "
               "prix_unitaire REAL NOT NULL, "
               "total REAL NOT NULL, "
               "FOREIGN KEY(id_commande) REFERENCES COMMANDES(id_commande), "
               "FOREIGN KEY(id_produit) REFERENCES PRODUITS(id_produit))");

    // Table PAIEMENTS
    query.exec("CREATE TABLE IF NOT EXISTS PAIEMENTS ("
               "id_paiement INTEGER PRIMARY KEY AUTOINCREMENT, "
               "id_commande INTEGER NOT NULL, "
               "montant REAL NOT NULL, "
               "date_paiement DATETIME DEFAULT CURRENT_TIMESTAMP, "
               "statut TEXT DEFAULT 'VALIDE' CHECK(statut IN ('VALIDE', 'ANNULE')), "
               "FOREIGN KEY(id_commande) REFERENCES COMMANDES(id_commande))");
}

void OrderDialog::setupUI()
{
    stackedWidget = new QStackedWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);

    setupClientForm();
    setupOrderSummary();
    setupPaymentForm();

    // Commencer par l'étape 1 (client)
    stackedWidget->setCurrentIndex(0);
}

void OrderDialog::setupClientForm()
{
    clientWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(clientWidget);

    QLabel *title = new QLabel("Informations client", clientWidget);
    title->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 20px;");
    layout->addWidget(title);

    QFormLayout *formLayout = new QFormLayout();

    nomEdit = new QLineEdit(clientWidget);
    nomEdit->setPlaceholderText("Nom du client");
    formLayout->addRow("Nom *:", nomEdit);

    prenomEdit = new QLineEdit(clientWidget);
    prenomEdit->setPlaceholderText("Prénom du client");
    formLayout->addRow("Prénom:", prenomEdit);

    telephoneEdit = new QLineEdit(clientWidget);
    telephoneEdit->setPlaceholderText("Numéro de téléphone");
    formLayout->addRow("Téléphone:", telephoneEdit);

    emailEdit = new QLineEdit(clientWidget);
    emailEdit->setPlaceholderText("Adresse email");
    formLayout->addRow("Email:", emailEdit);

    adresseEdit = new QTextEdit(clientWidget);
    adresseEdit->setPlaceholderText("Adresse complète");
    adresseEdit->setMaximumHeight(80);
    formLayout->addRow("Adresse:", adresseEdit);

    layout->addLayout(formLayout);
    // Products section removed from client form - products are selected in the order flow

    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    cancelBtn = new QPushButton("Annuler", clientWidget);
    connect(cancelBtn, &QPushButton::clicked, this, &OrderDialog::onCancelOrder);
    buttonLayout->addWidget(cancelBtn);

    nextBtn = new QPushButton("Suivant", clientWidget);
    nextBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px 16px; border: none; border-radius: 4px; }");
    connect(nextBtn, &QPushButton::clicked, this, &OrderDialog::onNextStep);
    buttonLayout->addWidget(nextBtn);

    layout->addLayout(buttonLayout);

    stackedWidget->addWidget(clientWidget);
}

void OrderDialog::setupOrderSummary()
{
    orderWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(orderWidget);

    QLabel *title = new QLabel("Récapitulatif de la commande", orderWidget);
    title->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 20px; color: #f1f5f9;");
    layout->addWidget(title);

    // Table pour afficher les produits
    orderTable = new QTableWidget(orderWidget);
    orderTable->setColumnCount(5);
    orderTable->setHorizontalHeaderLabels({"Produit", "Prix unitaire", "Quantité", "Total", "Actions"});
    orderTable->setColumnWidth(0, 150);
    orderTable->setColumnWidth(1, 120);
    orderTable->setColumnWidth(2, 100);
    orderTable->setColumnWidth(3, 100);
    orderTable->setColumnWidth(4, 120);
    orderTable->verticalHeader()->setVisible(false);
    orderTable->verticalHeader()->setDefaultSectionSize(70);
    orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    orderTable->setStyleSheet(
        "QTableWidget {"
        "   background: #0f172a;"
        "   color: #e2e8f0;"
        "   gridline-color: #334155;"
        "   border: none;"
        "}"
        "QTableWidget::item {"
        "   color: #f1f5f9;"
        "   padding: 8px;"
        "   border: none;"
        "}"
        "QHeaderView::section {"
        "   background: #1e293b;"
        "   color: #e2e8f0;"
        "   padding: 8px;"
        "   border: none;"
        "   border-right: 1px solid #334155;"
        "   font-weight: bold;"
        "}"
    );

    layout->addWidget(orderTable);

    // Total
    totalLabel = new QLabel("Total: 0.00 Ar", orderWidget);
    totalLabel->setStyleSheet("font-weight: bold; font-size: 16px; margin: 10px 0; color: #f1f5f9;");
    layout->addWidget(totalLabel);

    layout->addStretch();

    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    previousBtn = new QPushButton("Précédent", orderWidget);
    previousBtn->setMinimumHeight(48);
    previousBtn->setMinimumWidth(120);
    previousBtn->setCursor(Qt::PointingHandCursor);
    previousBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #667eea;"
        "   border: 2px solid #667eea;"
        "   border-radius: 10px;"
        "   font-weight: 700;"
        "   font-size: 14px;"
        "   padding: 8px 20px;"
        "}"
        "QPushButton:hover {"
        "   background: #667eea;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background: #5568d3;"
        "}"
    );
    connect(previousBtn, &QPushButton::clicked, this, &OrderDialog::onPreviousStep);
    buttonLayout->addWidget(previousBtn);

    QPushButton *cancelBtn2 = new QPushButton("Annuler", orderWidget);
    cancelBtn2->setMinimumHeight(48);
    cancelBtn2->setMinimumWidth(120);
    cancelBtn2->setCursor(Qt::PointingHandCursor);
    cancelBtn2->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #e53e3e;"
        "   border: 2px solid #e53e3e;"
        "   border-radius: 10px;"
        "   font-weight: 700;"
        "   font-size: 14px;"
        "   padding: 8px 20px;"
        "}"
        "QPushButton:hover {"
        "   background: #e53e3e;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background: #c53030;"
        "}"
    );
    connect(cancelBtn2, &QPushButton::clicked, this, &OrderDialog::onCancelOrder);
    buttonLayout->addWidget(cancelBtn2);

    validateBtn = new QPushButton("Continuer vers paiement", orderWidget);
    validateBtn->setMinimumHeight(48);
    validateBtn->setMinimumWidth(140);
    validateBtn->setCursor(Qt::PointingHandCursor);
    validateBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #059669);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 10px;"
        "   font-weight: 700;"
        "   font-size: 14px;"
        "   padding: 8px 24px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #059669, stop:1 #047857);"
        "}"
        "QPushButton:pressed {"
        "   background: #047857;"
        "}"
    );
    connect(validateBtn, &QPushButton::clicked, this, &OrderDialog::onContinueToPayment);
    buttonLayout->addWidget(validateBtn);

    layout->addLayout(buttonLayout);

    stackedWidget->addWidget(orderWidget);
}

void OrderDialog::setupPaymentForm()
{
    paymentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(paymentWidget);

    QLabel *title = new QLabel("💳 Formulaire de Paiement", paymentWidget);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 20px; color: #f1f5f9;");
    layout->addWidget(title);

    // Montant total à payer
    QLabel *totalDescLabel = new QLabel("Montant de la commande:", paymentWidget);
    totalDescLabel->setStyleSheet("color: #cbd5e1; font-weight: 600; font-size: 14px;");
    
    paymentTotalLabel = new QLabel(paymentWidget);
    paymentTotalLabel->setStyleSheet(
        "QLabel {"
        "   background: linear-gradient(135deg, #10b981 0%, #059669 100%);"
        "   color: white;"
        "   padding: 15px;"
        "   border-radius: 8px;"
        "   font-size: 22px;"
        "   font-weight: bold;"
        "   text-align: center;"
        "}"
    );
    layout->addWidget(totalDescLabel);
    layout->addWidget(paymentTotalLabel);
    layout->addSpacing(15);

    // Formulaire de paiement
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Mode de paiement
    QLabel *methodLabel = new QLabel("Mode de paiement:", paymentWidget);
    methodLabel->setStyleSheet("color: #cbd5e1; font-weight: 600;");
    paymentMethodCombo = new QComboBox(paymentWidget);
    paymentMethodCombo->addItem("💵 Espèces");
    paymentMethodCombo->addItem("💳 Carte Bancaire");
    paymentMethodCombo->addItem("📱 Mobile Payment");
    paymentMethodCombo->addItem("🏦 Chèque");
    paymentMethodCombo->setMinimumHeight(40);
    paymentMethodCombo->setStyleSheet(
        "QComboBox {"
        "   border: 2px solid #475569;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "   font-size: 14px;"
        "   background: #1e293b;"
        "   color: #f1f5f9;"
        "}"
        "QComboBox:focus {"
        "   border: 2px solid #10b981;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
        "QComboBox::down-arrow {"
        "   image: none;"
        "}"
    );
    formLayout->addRow(methodLabel, paymentMethodCombo);

    // Montant à payer
    QLabel *amountLabel = new QLabel("Montant à payer:", paymentWidget);
    amountLabel->setStyleSheet("color: #cbd5e1; font-weight: 600;");
    paymentAmountSpinBox = new QDoubleSpinBox(paymentWidget);
    paymentAmountSpinBox->setMinimum(0);
    paymentAmountSpinBox->setMaximum(999999.99);
    paymentAmountSpinBox->setDecimals(2);
    paymentAmountSpinBox->setSingleStep(0.01);
    paymentAmountSpinBox->setMinimumHeight(40);
    paymentAmountSpinBox->setStyleSheet(
        "QDoubleSpinBox {"
        "   border: 2px solid #475569;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "   font-size: 14px;"
        "   background: #1e293b;"
        "   color: #f1f5f9;"
        "}"
        "QDoubleSpinBox:focus {"
        "   border: 2px solid #10b981;"
        "}"
        "QAbstractSpinBox::up-button, QAbstractSpinBox::down-button {"
        "   background-color: #334155;"
        "   border: none;"
        "   color: #f1f5f9;"
        "}"
    );
    // Connecter le signal pour mettre à jour le statut dynamiquement
    connect(paymentAmountSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &OrderDialog::updatePaymentStatus);
    formLayout->addRow(amountLabel, paymentAmountSpinBox);

    layout->addLayout(formLayout);

    // Bouton utiliser le total
    paymentUseFullAmountBtn = new QPushButton("↔️ Utiliser le montant total", paymentWidget);
    paymentUseFullAmountBtn->setMinimumHeight(38);
    paymentUseFullAmountBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #0ea5e9;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   font-weight: 600;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0284c7;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #0c63e4;"
        "}"
    );
    connect(paymentUseFullAmountBtn, &QPushButton::clicked, this, [this]() {
        paymentAmountSpinBox->setValue(totalAmount);
    });
    layout->addWidget(paymentUseFullAmountBtn);

    layout->addSpacing(10);

    // Statut du paiement
    paymentStatusLabel = new QLabel(paymentWidget);
    paymentStatusLabel->setStyleSheet(
        "QLabel {"
        "   background: #1e293b;"
        "   color: #f1f5f9;"
        "   padding: 12px;"
        "   border-radius: 6px;"
        "   border-left: 4px solid #10b981;"
        "   font-weight: 600;"
        "}"
    );
    layout->addWidget(paymentStatusLabel);

    layout->addStretch();

    // Boutons d'action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    previousPaymentBtn = new QPushButton("Précédent", paymentWidget);
    previousPaymentBtn->setMinimumHeight(48);
    previousPaymentBtn->setMinimumWidth(120);
    previousPaymentBtn->setCursor(Qt::PointingHandCursor);
    previousPaymentBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #667eea;"
        "   border: 2px solid #667eea;"
        "   border-radius: 10px;"
        "   font-weight: 700;"
        "   font-size: 14px;"
        "   padding: 8px 20px;"
        "}"
        "QPushButton:hover {"
        "   background: #667eea;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background: #5568d3;"
        "}"
    );
    connect(previousPaymentBtn, &QPushButton::clicked, this, &OrderDialog::onPreviousFromPayment);
    buttonLayout->addWidget(previousPaymentBtn);

    QPushButton *cancelPaymentBtn = new QPushButton("Annuler", paymentWidget);
    cancelPaymentBtn->setMinimumHeight(48);
    cancelPaymentBtn->setMinimumWidth(120);
    cancelPaymentBtn->setCursor(Qt::PointingHandCursor);
    cancelPaymentBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #e53e3e;"
        "   border: 2px solid #e53e3e;"
        "   border-radius: 10px;"
        "   font-weight: 700;"
        "   font-size: 14px;"
        "   padding: 8px 20px;"
        "}"
        "QPushButton:hover {"
        "   background: #e53e3e;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background: #c53030;"
        "}"
    );
    connect(cancelPaymentBtn, &QPushButton::clicked, this, &OrderDialog::onCancelOrder);
    buttonLayout->addWidget(cancelPaymentBtn);

    confirmPaymentBtn = new QPushButton("✓ Confirmer le paiement", paymentWidget);
    confirmPaymentBtn->setMinimumHeight(48);
    confirmPaymentBtn->setMinimumWidth(180);
    confirmPaymentBtn->setCursor(Qt::PointingHandCursor);
    confirmPaymentBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #059669);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 10px;"
        "   font-weight: 700;"
        "   font-size: 15px;"
        "   padding: 8px 30px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #059669, stop:1 #047857);"
        "}"
        "QPushButton:pressed {"
        "   background: #047857;"
        "}"
    );
    connect(confirmPaymentBtn, &QPushButton::clicked, this, &OrderDialog::onConfirmPayment);
    buttonLayout->addWidget(confirmPaymentBtn);

    layout->addLayout(buttonLayout);

    stackedWidget->addWidget(paymentWidget);
}

void OrderDialog::addProduct(int productId, const QString &productName, double unitPrice, int quantity)
{
    if (orderItems.contains(productId)) {
        orderItems[productId].quantity += quantity;
        orderItems[productId].total = orderItems[productId].unitPrice * orderItems[productId].quantity;
    } else {
        OrderItem item;
        item.productId = productId;
        item.productName = productName;
        item.unitPrice = unitPrice;
        item.quantity = quantity;
        item.total = unitPrice * quantity;
        orderItems[productId] = item;
    }

    updateTotal();
    updateTable();
}

void OrderDialog::removeProduct(int productId, int quantity)
{
    if (orderItems.contains(productId)) {
        orderItems[productId].quantity -= quantity;
        if (orderItems[productId].quantity <= 0) {
            orderItems.remove(productId);
        } else {
            orderItems[productId].total = orderItems[productId].unitPrice * orderItems[productId].quantity;
        }
        updateTotal();
        updateTable();
    }
}

double OrderDialog::getTotal() const
{
    return totalAmount;
}

void OrderDialog::onNextStep()
{
    // Validation des champs obligatoires
    if (nomEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de validation", "Le nom du client est obligatoire.");
        nomEdit->setFocus();
        return;
    }

    // Validation du numéro de téléphone s'il est fourni
    QString telephone = telephoneEdit->text().trimmed();
    if (!telephone.isEmpty() && telephone.length() < 8) {
        QMessageBox::warning(this, "Erreur de validation", "Le numéro de téléphone doit contenir au moins 8 chiffres.");
        telephoneEdit->setFocus();
        return;
    }

    // Validation de l'email s'il est fourni
    QString email = emailEdit->text().trimmed();
    if (!email.isEmpty() && !email.contains("@")) {
        QMessageBox::warning(this, "Erreur de validation", "L'adresse email n'est pas valide.");
        emailEdit->setFocus();
        return;
    }

    if (orderItems.isEmpty()) {
        QMessageBox::warning(this, "Panier vide", "Votre panier est vide. Ajoutez des produits avant de continuer.");
        return;
    }

    // Passer à l'étape 2
    stackedWidget->setCurrentIndex(1);
    updateTable();
}

void OrderDialog::onPreviousStep()
{
    // Revenir à l'étape 1
    stackedWidget->setCurrentIndex(0);
}

void OrderDialog::onValidateOrder()
{
    // Cette fonction est obsolète, elle est remplacée par onContinueToPayment() puis onConfirmPayment()
    // Garder pour compatibilité au cas où elle serait appelée ailleurs
}

void OrderDialog::onCancelOrder()
{
    orderItems.clear();
    totalAmount = 0.0;
    reject();
}

void OrderDialog::onRemoveItem(int row)
{
    // Trouver le productId à partir de la ligne
    int productId = -1;
    for (auto it = orderItems.begin(); it != orderItems.end(); ++it) {
        if (row == 0) {
            productId = it.key();
            break;
        }
        row--;
    }

    if (productId != -1) {
        orderItems.remove(productId);
        updateTotal();
        updateTable();
    }
}

void OrderDialog::updateTotal()
{
    totalAmount = 0.0;
    for (const OrderItem &item : orderItems) {
        totalAmount += item.total;
    }
    if (totalLabel) {
        totalLabel->setText(QString("Total: %1 Ar").arg(QString::number(totalAmount, 'f', 2)));
    }
}

void OrderDialog::updateTable()
{
    if (!orderTable) return;

    orderTable->setRowCount(orderItems.size());
    int row = 0;

    for (auto it = orderItems.begin(); it != orderItems.end(); ++it) {
        const OrderItem &item = it.value();

        // Nom du produit
        orderTable->setItem(row, 0, new QTableWidgetItem(item.productName));

        // Prix unitaire
        orderTable->setItem(row, 1, new QTableWidgetItem(QString::number(item.unitPrice, 'f', 2) + " Ar"));

        // Quantité
        orderTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.quantity)));

        // Total pour cet article
        orderTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.total, 'f', 2) + " Ar"));

        // Bouton de suppression
        QWidget *actionWidget = new QWidget();
        actionWidget->setStyleSheet("background: transparent;");
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->setSpacing(0);
        actionLayout->setAlignment(Qt::AlignCenter);
        
        QPushButton *removeBtn = new QPushButton("🗑️");
        removeBtn->setFixedSize(32, 32);
        removeBtn->setCursor(Qt::PointingHandCursor);
        removeBtn->setStyleSheet(
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #f56565, stop:1 #e53e3e);"
            "   color: white;"
            "   border: none;"
            "   font-size: 14px;"
            "   font-weight: 700;"
            "   outline: none;"
            "   padding: 0px;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #e53e3e, stop:1 #c53030);"
            "}"
            "QPushButton:pressed {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "   stop:0 #c53030, stop:1 #742a2a);"
            "}"
        );
        connect(removeBtn, &QPushButton::clicked, [this, row]() {
            onRemoveItem(row);
        });
        
        actionLayout->addWidget(removeBtn);
        
        orderTable->setCellWidget(row, 4, actionWidget);

        row++;
    }
}

int OrderDialog::saveClientAndOrder(double paidAmount)
{
    QSqlQuery query;

    // 1. Insérer le client
    query.prepare("INSERT INTO CLIENTS (nom, prenom, telephone, email, adresse) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(nomEdit->text().trimmed());
    query.addBindValue(prenomEdit->text().trimmed());
    query.addBindValue(telephoneEdit->text().trimmed());
    query.addBindValue(emailEdit->text().trimmed());
    query.addBindValue(adresseEdit->toPlainText().trimmed());

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la création du client: " + query.lastError().text());
        return -1;
    }

    int clientId = query.lastInsertId().toInt();

    // 2. Déterminer le statut du paiement
    QString paymentStatus = "EN_COURS"; // Par défaut
    if (paidAmount >= totalAmount) {
        paymentStatus = "PAYEE";
    }

    // 3. Insérer la commande avec le statut déterminé
    query.prepare("INSERT INTO COMMANDES (id_client, id_user, total, statut) "
                  "VALUES (?, ?, ?, ?)");
    query.addBindValue(clientId);
    query.addBindValue(currentUserId);
    query.addBindValue(totalAmount);
    query.addBindValue(paymentStatus);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la création de la commande: " + query.lastError().text());
        return -1;
    }

    int commandeId = query.lastInsertId().toInt();

    // 4. Insérer les détails de la commande
    for (const OrderItem &item : orderItems) {
        query.prepare("INSERT INTO DETAILS_COMMANDE (id_commande, id_produit, quantite, prix_unitaire, total) "
                      "VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(commandeId);
        query.addBindValue(item.productId);
        query.addBindValue(item.quantity);
        query.addBindValue(item.unitPrice);
        query.addBindValue(item.total);

        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout des détails de commande: " + query.lastError().text());
            return -1;
        }
    }

    // 5. Mettre à jour le stock des produits
    for (const OrderItem &item : orderItems) {
        query.prepare("UPDATE PRODUITS SET stock = stock - ? WHERE id_produit = ?");
        query.addBindValue(item.quantity);
        query.addBindValue(item.productId);

        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la mise à jour du stock: " + query.lastError().text());
            return -1;
        }
    }

    // 6. Insérer les mouvements de stock pour chaque produit vendu
    for (const OrderItem &item : orderItems) {
        QSqlQuery mq;
        mq.prepare("INSERT INTO STOCK_MOVEMENTS (id_product, type, quantite, motif, id_user) VALUES (?, ?, ?, ?, ?)");
        mq.addBindValue(item.productId);
        mq.addBindValue(QString("SORTIE"));
        mq.addBindValue(item.quantity);
        mq.addBindValue(QString("Vente - Commande %1").arg(commandeId));
        mq.addBindValue(currentUserId);
        if (!mq.exec()) {
            qDebug() << "Failed to insert stock movement for order:" << mq.lastError().text();
        }
    }
    
    // Mettre à jour les statistiques du mouvement de stock
    if (stockMovementPage) {
        stockMovementPage->refreshStatistics();
    }
    
    // Mettre à jour le tableau de bord
    if (dashboardPage) {
        dashboardPage->onDataChanged();
    }

    // 7. Insérer le paiement avec le montant réellement payé
    query.prepare("INSERT INTO PAIEMENTS (id_commande, montant, statut) "
                  "VALUES (?, ?, 'VALIDE')");
    query.addBindValue(commandeId);
    query.addBindValue(paidAmount);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'enregistrement du paiement: " + query.lastError().text());
        return -1;
    }

    // 6. Le statut de la commande a déjà été défini correctement lors de l'insertion
    // (pas besoin de UPDATE supplémentaire)

    return commandeId;
}

bool OrderDialog::checkStocks()
{
    QSqlQuery query;
    for (auto it = orderItems.begin(); it != orderItems.end(); ++it) {
        query.prepare("SELECT stock FROM PRODUITS WHERE id_produit = ?");
        query.addBindValue(it.key());
        if (query.exec() && query.next()) {
            int stock = query.value(0).toInt();
            if (it.value().quantity > stock) {
                return false;
            }
        } else {
            return false; // Erreur ou produit non trouvé
        }
    }
    return true;
}

void OrderDialog::loadOrderForEdit(const QString &commandeId)
{
    // Charger les informations de la commande
    QSqlQuery query;
    query.prepare("SELECT c.date_commande, cl.nom, cl.prenom, cl.telephone, cl.email, cl.adresse, c.total "
                  "FROM COMMANDES c "
                  "LEFT JOIN CLIENTS cl ON c.id_client = cl.id_client "
                  "WHERE c.id_commande = ?");
    query.addBindValue(commandeId.toInt());
    
    if (query.exec() && query.next()) {
        // Remplir les champs client
        clientNom = query.value("nom").toString();
        clientPrenom = query.value("prenom").toString();
        clientTelephone = query.value("telephone").toString();
        clientEmail = query.value("email").toString();
        clientAdresse = query.value("adresse").toString();
        
        // Mettre à jour l'interface client
        nomEdit->setText(clientNom);
        prenomEdit->setText(clientPrenom);
        telephoneEdit->setText(clientTelephone);
        emailEdit->setText(clientEmail);
        adresseEdit->setText(clientAdresse);
        
        // Charger les détails de la commande
        QSqlQuery detailQuery;
        detailQuery.prepare("SELECT cd.id_produit, p.nom, p.prix_vente, cd.quantite, cd.total "
                           "FROM COMMANDE_DETAIL cd "
                           "LEFT JOIN PRODUITS p ON cd.id_produit = p.id_produit "
                           "WHERE cd.id_commande = ?");
        detailQuery.addBindValue(commandeId.toInt());
        
        if (detailQuery.exec()) {
            while (detailQuery.next()) {
                int productId = detailQuery.value("id_produit").toInt();
                QString productName = detailQuery.value("nom").toString();
                double unitPrice = detailQuery.value("prix_vente").toDouble();
                int quantity = detailQuery.value("quantite").toInt();
                
                addProduct(productId, productName, unitPrice, quantity);
            }
        }
        
        // Passer directement à l'étape de récapitulatif
        stackedWidget->setCurrentWidget(orderWidget);
        updateTotal();
        updateTable();
    }
}

void OrderDialog::reset()
{
    orderItems.clear();
    totalAmount = 0.0;
    resetUI();
}

void OrderDialog::onContinueToPayment()
{
    // Validation des stocks
    if (!checkStocks()) {
        QMessageBox::warning(this, "Stock insuffisant", 
                           "Un ou plusieurs produits dans votre commande n'ont plus assez de stock disponible.");
        return;
    }

    // Mettre à jour le formulaire de paiement avec le montant
    paymentTotalLabel->setText(QString("%1 Ar").arg(QString::number(totalAmount, 'f', 2)));
    paymentAmountSpinBox->setValue(totalAmount);
    paymentAmountSpinBox->setMinimum(0);
    paymentAmountSpinBox->setMaximum(totalAmount * 2); // Permet de payer 2x le montant si besoin
    paymentMethodCombo->setCurrentIndex(0); // Espèces par défaut
    
    // Mettre à jour le statut du paiement
    updatePaymentStatus();

    // Passer à l'étape 3 (paiement)
    stackedWidget->setCurrentIndex(2);
}

void OrderDialog::updatePaymentStatus()
{
    double paidAmount = paymentAmountSpinBox->value();
    
    if (paidAmount == 0) {
        paymentStatusLabel->setText("⏳ Aucun montant saisi");
        paymentStatusLabel->setStyleSheet(
            "QLabel {"
            "   background: #1e293b;"
            "   color: #f1f5f9;"
            "   padding: 12px;"
            "   border-radius: 6px;"
            "   border-left: 4px solid #94a3b8;"
            "   font-weight: 600;"
            "}"
        );
    } else if (paidAmount == totalAmount) {
        paymentStatusLabel->setText(QString("✓ Paiement complet (%1 Ar)").arg(totalAmount, 0, 'f', 2));
        paymentStatusLabel->setStyleSheet(
            "QLabel {"
            "   background: #1e293b;"
            "   color: #f1f5f9;"
            "   padding: 12px;"
            "   border-radius: 6px;"
            "   border-left: 4px solid #10b981;"
            "   font-weight: 600;"
            "}"
        );
    } else if (paidAmount < totalAmount) {
        double remaining = totalAmount - paidAmount;
        paymentStatusLabel->setText(QString("⏳ Paiement partiel - Reste: %1 Ar").arg(remaining, 0, 'f', 2));
        paymentStatusLabel->setStyleSheet(
            "QLabel {"
            "   background: #1e293b;"
            "   color: #f1f5f9;"
            "   padding: 12px;"
            "   border-radius: 6px;"
            "   border-left: 4px solid #f59e0b;"
            "   font-weight: 600;"
            "}"
        );
    } else {
        double change = paidAmount - totalAmount;
        paymentStatusLabel->setText(QString("✓ Surplus: %1 Ar (Monnaie à rendre)").arg(change, 0, 'f', 2));
        paymentStatusLabel->setStyleSheet(
            "QLabel {"
            "   background: #1e293b;"
            "   color: #f1f5f9;"
            "   padding: 12px;"
            "   border-radius: 6px;"
            "   border-left: 4px solid #8b5cf6;"
            "   font-weight: 600;"
            "}"
        );
    }
}

void OrderDialog::onPreviousFromPayment()
{
    // Revenir à l'étape 2 (récapitulatif)
    stackedWidget->setCurrentIndex(1);
}

void OrderDialog::onConfirmPayment()
{
    qDebug() << "=== onConfirmPayment() called ===";
    
    // Validation du montant
    double paidAmount = paymentAmountSpinBox->value();
    
    if (paidAmount <= 0) {
        QMessageBox::warning(this, "Erreur", "Le montant payé doit être supérieur à 0 Ar");
        return;
    }
    
    // Valider le mode de paiement
    if (paymentMethodCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un mode de paiement");
        return;
    }
    
    QString paymentMethod = paymentMethodCombo->currentText();
    // Enlever l'emoji pour garder seulement le texte
    paymentMethod = paymentMethod.mid(paymentMethod.indexOf(" ") + 1);
    
    qDebug() << "Payment confirmed - Amount:" << paidAmount << "Ar - Method:" << paymentMethod;
    
    // Sauvegarder client, commande, détails et paiement avec le montant payé
    int commandeId = saveClientAndOrder(paidAmount);
    qDebug() << "saveClientAndOrder returned commandeId:" << commandeId;
    
    if (commandeId != -1) {
        // Déterminer le message basé sur le statut du paiement
        QString paymentStatus = (paidAmount >= totalAmount) ? "PAYEE" : "EN_COURS";
        QString displayPaymentStatus = (paidAmount >= totalAmount) ? "Payée" : "En cours";
        QString statusMessage;
        double remainingAmount = totalAmount - paidAmount;
        
        if (paidAmount >= totalAmount) {
            statusMessage = QString("✓ Commande PAYÉE - Montant reçu: %1 Ar").arg(paidAmount, 0, 'f', 2);
        } else {
            statusMessage = QString("⏳ Commande EN COURS - Montant reçu: %1 Ar - Reste: %2 Ar")
                .arg(paidAmount, 0, 'f', 2)
                .arg(remainingAmount, 0, 'f', 2);
        }
        
        // Générer le PDF de reçu
        qDebug() << "Generating receipt for command ID:" << commandeId;
        ReceiptData receiptData = ReceiptGenerator::getReceiptData(commandeId);
        qDebug() << "Receipt data retrieved";
        qDebug() << "  Client email:" << receiptData.client.email;
        qDebug() << "  Order ID:" << receiptData.order.id;
        qDebug() << "  Payment amount:" << receiptData.payment.montant;
        
        // Ajouter le montant restant à la structure
        receiptData.remainingAmount = remainingAmount;
        
        ReceiptGenerator generator;
        
        // Générer le PDF et récupérer le chemin
        QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QString pdfFilePath = QString("%1/Recu_Commande_%2_%3.pdf")
            .arg(documentsPath)
            .arg(commandeId)
            .arg(QDateTime::currentDateTime().toString("ddMMyyyy_hhmmss"));
        
        qDebug() << "PDF file path:" << pdfFilePath;
        generator.generatePDF(receiptData, pdfFilePath);
        qDebug() << "PDF generated successfully";
        
        // Envoyer le PDF par email si l'email client est disponible
        if (!receiptData.client.email.isEmpty()) {
            qDebug() << "Email address is not empty, attempting to send email to:" << receiptData.client.email;
            bool emailSent = generator.sendReceiptByEmail(receiptData.client.email, pdfFilePath, receiptData);
            qDebug() << "Email send result:" << (emailSent ? "SUCCESS" : "FAILED");
            
            if (emailSent) {
                QMessageBox::information(this, "Commande créée - " + displayPaymentStatus,
                                       statusMessage + QString("\n\nLe reçu a été généré et un email a été envoyé à %1.\n\nMode de paiement: %2").arg(receiptData.client.email).arg(paymentMethod));
            } else {
                qWarning() << "Email sending failed";
                QMessageBox::information(this, "Commande créée - " + displayPaymentStatus,
                                       statusMessage + QString("\n\nLe reçu a été généré et ouvert.\n\nMode de paiement: %1").arg(paymentMethod));
            }
        } else {
            qWarning() << "Email address is empty!";
            QMessageBox::information(this, "Commande créée - " + displayPaymentStatus,
                                   statusMessage + QString("\n\nLe reçu a été généré et ouvert.\n\nMode de paiement: %1\n\nAucun email envoyé - adresse email non disponible.").arg(paymentMethod));
        }
        
        emit orderSaved();
        reset();
        accept();
    } else {
        qWarning() << "Failed to save client and order";
    }
}

void OrderDialog::resetUI()
{
    nomEdit->clear();
    prenomEdit->clear();
    telephoneEdit->clear();
    emailEdit->clear();
    adresseEdit->clear();
    stackedWidget->setCurrentIndex(0);
}