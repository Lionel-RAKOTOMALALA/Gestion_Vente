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
    QDialog(parent), totalAmount(0.0), currentUserId(userId)
{
    setWindowTitle("Nouvelle commande");
    setModal(true);
    setMinimumWidth(700);
    setMinimumHeight(500);

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

OrderDialog::OrderDialog(int userId, const QString &commandeId, QWidget *parent) :
    QDialog(parent), totalAmount(0.0), currentUserId(userId), isEditMode(true), editCommandeId(commandeId)
{
    setWindowTitle("Modifier commande");
    setModal(true);
    setMinimumWidth(700);
    setMinimumHeight(500);

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
}

void OrderDialog::setupOrderSummary()
{
    orderWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(orderWidget);

    QLabel *title = new QLabel("Récapitulatif de la commande", orderWidget);
    title->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 20px;");
    layout->addWidget(title);

    // Table pour afficher les produits
    orderTable = new QTableWidget(orderWidget);
    orderTable->setColumnCount(5);
    orderTable->setHorizontalHeaderLabels({"Produit", "Prix unitaire", "Quantité", "Total", "Actions"});
    orderTable->horizontalHeader()->setStretchLastSection(true);
    orderTable->setAlternatingRowColors(true);
    orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    layout->addWidget(orderTable);

    // Total
    totalLabel = new QLabel("Total: 0.00 €", orderWidget);
    totalLabel->setStyleSheet("font-weight: bold; font-size: 16px; margin: 10px 0;");
    layout->addWidget(totalLabel);

    layout->addStretch();

    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    previousBtn = new QPushButton("Précédent", orderWidget);
    connect(previousBtn, &QPushButton::clicked, this, &OrderDialog::onPreviousStep);
    buttonLayout->addWidget(previousBtn);

    QPushButton *cancelBtn2 = new QPushButton("Annuler", orderWidget);
    connect(cancelBtn2, &QPushButton::clicked, this, &OrderDialog::onCancelOrder);
    buttonLayout->addWidget(cancelBtn2);

    validateBtn = new QPushButton("Valider la commande", orderWidget);
    validateBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px 16px; border: none; border-radius: 4px; }");
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
    if (!checkStocks()) {
        QMessageBox::warning(this, "Stock insuffisant", 
                           "Un ou plusieurs produits dans votre commande n'ont plus assez de stock disponible.");
        return;
    }
    if (saveClientAndOrder()) {
        QMessageBox::information(this, "Commande validée",
                               QString("La commande a été créée avec succès pour un montant de %1 €.").arg(QString::number(totalAmount, 'f', 2)));
        emit orderSaved();
        reset(); // Réinitialiser complètement après validation
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
        QPushButton *removeBtn = new QPushButton("Retirer", orderTable);
        connect(removeBtn, &QPushButton::clicked, [this, row]() {
            onRemoveItem(row);
        });
        orderTable->setCellWidget(row, 4, removeBtn);

        row++;
    }
}

bool OrderDialog::saveClientAndOrder()
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
        query.prepare("INSERT INTO DETAILS_COMMANDE (id_commande, id_produit, quantite, prix_unitaire, total) "
                      "VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(commandeId);
        query.addBindValue(item.productId);
        query.addBindValue(item.quantity);
        query.addBindValue(item.unitPrice);
        query.addBindValue(item.total);

        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout des détails de commande: " + query.lastError().text());
            return false;
        }
    }

    // 4. Mettre à jour le stock des produits
    for (const OrderItem &item : orderItems) {
        query.prepare("UPDATE PRODUITS SET stock = stock - ? WHERE id_produit = ?");
        query.addBindValue(item.quantity);
        query.addBindValue(item.productId);

        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la mise à jour du stock: " + query.lastError().text());
            return false;
        }
    }

    return true;
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

void OrderDialog::resetUI()
{
    nomEdit->clear();
    prenomEdit->clear();
    telephoneEdit->clear();
    emailEdit->clear();
    adresseEdit->clear();
    stackedWidget->setCurrentIndex(0);
}