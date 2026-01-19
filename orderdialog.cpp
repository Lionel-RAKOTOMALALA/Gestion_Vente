#include "orderdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

OrderDialog::OrderDialog(int userId, QWidget *parent) :
    QDialog(parent), totalAmount(0.0), currentUserId(userId), isEditMode(false)
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

OrderDialog::OrderDialog(int userId, const QString &commandeId, QWidget *parent) :
    QDialog(parent), totalAmount(0.0), currentUserId(userId), isEditMode(true), editCommandeId(commandeId)
{
    setWindowTitle("Modifier commande");
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
    loadOrderForEdit(commandeId);
    setupUI();
}

OrderDialog::~OrderDialog()
{
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

    // Table COMMANDE_DETAIL
    query.exec("CREATE TABLE IF NOT EXISTS COMMANDE_DETAIL ("
               "id_detail INTEGER PRIMARY KEY AUTOINCREMENT, "
               "id_commande INTEGER NOT NULL, "
               "id_produit INTEGER NOT NULL, "
               "quantite INTEGER NOT NULL, "
               "FOREIGN KEY(id_commande) REFERENCES COMMANDES(id_commande) ON DELETE CASCADE, "
               "FOREIGN KEY(id_produit) REFERENCES PRODUITS(id_produit))");
}

void OrderDialog::setupUI()
{
    stackedWidget = new QStackedWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);

    setupClientForm();
    setupOrderSummary();

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
    layout->addStretch();

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

    // Pré-remplir les champs si en mode édition
    if (isEditMode) {
        nomEdit->setText(clientNom);
        prenomEdit->setText(clientPrenom);
        telephoneEdit->setText(clientTelephone);
        emailEdit->setText(clientEmail);
        adresseEdit->setText(clientAdresse);
    }
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
    totalLabel = new QLabel("Total: 0.00 €", orderWidget);
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

    validateBtn = new QPushButton("Valider la commande", orderWidget);
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
    connect(validateBtn, &QPushButton::clicked, this, &OrderDialog::onValidateOrder);
    buttonLayout->addWidget(validateBtn);

    layout->addLayout(buttonLayout);

    stackedWidget->addWidget(orderWidget);
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
        QMessageBox::warning(this, "Champ obligatoire", "Le nom du client est obligatoire.");
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
    if (saveClientAndOrder()) {
        QMessageBox::information(this, "Commande validée",
                               QString("La commande a été créée avec succès pour un montant de %1 €.").arg(QString::number(totalAmount, 'f', 2)));
        accept();
    }
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
        totalLabel->setText(QString("Total: %1 €").arg(QString::number(totalAmount, 'f', 2)));
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
        orderTable->setItem(row, 1, new QTableWidgetItem(QString::number(item.unitPrice, 'f', 2) + " €"));

        // Quantité
        orderTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.quantity)));

        // Total pour cet article
        orderTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.total, 'f', 2) + " €"));

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

bool OrderDialog::saveClientAndOrder()
{
    QSqlQuery query;

    if (isEditMode) {
        // Mode édition: mettre à jour la commande existante

        // 1. Récupérer l'id_client de la commande existante
        query.prepare("SELECT id_client FROM COMMANDES WHERE id_commande = ?");
        query.addBindValue(editCommandeId.toInt());
        if (!query.exec() || !query.next()) {
            QMessageBox::critical(this, "Erreur", "Impossible de trouver la commande à modifier.");
            return false;
        }
        int clientId = query.value("id_client").toInt();

        // 2. Mettre à jour le client
        query.prepare("UPDATE CLIENTS SET nom = ?, prenom = ?, telephone = ?, email = ?, adresse = ? "
                      "WHERE id_client = ?");
        query.addBindValue(nomEdit->text().trimmed());
        query.addBindValue(prenomEdit->text().trimmed());
        query.addBindValue(telephoneEdit->text().trimmed());
        query.addBindValue(emailEdit->text().trimmed());
        query.addBindValue(adresseEdit->toPlainText().trimmed());
        query.addBindValue(clientId);

        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la mise à jour du client: " + query.lastError().text());
            return false;
        }

        // 3. Mettre à jour la commande
        query.prepare("UPDATE COMMANDES SET total = ? WHERE id_commande = ?");
        query.addBindValue(totalAmount);
        query.addBindValue(editCommandeId.toInt());

        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la mise à jour de la commande: " + query.lastError().text());
            return false;
        }

        // 4. Supprimer les anciens détails et en ajouter de nouveaux
        query.prepare("DELETE FROM COMMANDE_DETAIL WHERE id_commande = ?");
        query.addBindValue(editCommandeId.toInt());
        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression des anciens détails: " + query.lastError().text());
            return false;
        }

        // 5. Insérer les nouveaux détails
        for (const OrderItem &item : orderItems) {
            query.prepare("INSERT INTO COMMANDE_DETAIL (id_commande, id_produit, quantite) "
                          "VALUES (?, ?, ?)");
            query.addBindValue(editCommandeId.toInt());
            query.addBindValue(item.productId);
            query.addBindValue(item.quantity);

            if (!query.exec()) {
                QMessageBox::critical(this, "Erreur", "Erreur lors de la mise à jour des détails de commande: " + query.lastError().text());
                return false;
            }
        }

    } else {
        // Mode création: créer une nouvelle commande

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
            return false;
        }

        int clientId = query.lastInsertId().toInt();

        // 2. Insérer la commande
        query.prepare("INSERT INTO COMMANDES (id_client, id_user, total, statut) "
                      "VALUES (?, ?, ?, 'EN_COURS')");
        query.addBindValue(clientId);
        query.addBindValue(currentUserId);
        query.addBindValue(totalAmount);

        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la création de la commande: " + query.lastError().text());
            return false;
        }

        int commandeId = query.lastInsertId().toInt();

        // 3. Insérer les détails de la commande
        for (const OrderItem &item : orderItems) {
            query.prepare("INSERT INTO COMMANDE_DETAIL (id_commande, id_produit, quantite) "
                          "VALUES (?, ?, ?)");
            query.addBindValue(commandeId);
            query.addBindValue(item.productId);
            query.addBindValue(item.quantity);

            if (!query.exec()) {
                QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout des détails de commande: " + query.lastError().text());
                return false;
            }
        }
    }

    return true;
}

void OrderDialog::loadOrderForEdit(const QString &commandeId)
{
    QSqlQuery query;

    // Charger les informations de la commande et du client
    query.prepare("SELECT c.id_client, cl.nom, cl.prenom, cl.telephone, cl.email, cl.adresse, "
                  "c.total, c.statut "
                  "FROM COMMANDES c "
                  "LEFT JOIN CLIENTS cl ON c.id_client = cl.id_client "
                  "WHERE c.id_commande = ?");
    query.addBindValue(commandeId.toInt());

    if (query.exec() && query.next()) {
        // Stocker les informations du client dans les variables membres
        clientNom = query.value("nom").toString();
        clientPrenom = query.value("prenom").toString();
        clientTelephone = query.value("telephone").toString();
        clientEmail = query.value("email").toString();
        clientAdresse = query.value("adresse").toString();

        totalAmount = query.value("total").toDouble();

        // Charger les détails de la commande
        QSqlQuery detailQuery;
        detailQuery.prepare("SELECT cd.id_produit, p.nom_produit, p.prix, cd.quantite "
                           "FROM COMMANDE_DETAIL cd "
                           "LEFT JOIN PRODUITS p ON cd.id_produit = p.id_produit "
                           "WHERE cd.id_commande = ?");
        detailQuery.addBindValue(commandeId.toInt());

        if (detailQuery.exec()) {
            while (detailQuery.next()) {
                int productId = detailQuery.value("id_produit").toInt();
                QString productName = detailQuery.value("nom_produit").toString();
                double unitPrice = detailQuery.value("prix").toDouble();
                int quantity = detailQuery.value("quantite").toInt();

                addProduct(productId, productName, unitPrice, quantity);
            }
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de charger la commande: " + query.lastError().text());
    }
}