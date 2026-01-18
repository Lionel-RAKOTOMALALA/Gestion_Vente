#include "clientdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

ClientDialog::ClientDialog(QWidget *parent, int clientId)
    : QDialog(parent), currentClientId(clientId)
{
    setupUI();
    if (clientId != -1) {
        loadClient(clientId);
    }
}

void ClientDialog::setupUI()
{
    setWindowTitle(currentClientId == -1 ? "Ajouter un client" : "Modifier le client");
    setMinimumWidth(500);
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Titre
    QLabel *title = new QLabel(currentClientId == -1 ? "➕ Nouveau Client" : "✏️ Modifier le Client", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");
    mainLayout->addWidget(title);

    // Formulaire
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    txtNom = new QLineEdit(this);
    txtNom->setPlaceholderText("Entrez le nom de famille");
    txtNom->setMinimumHeight(40);
    
    txtPrenom = new QLineEdit(this);
    txtPrenom->setPlaceholderText("Entrez le prénom");
    txtPrenom->setMinimumHeight(40);
    
    txtEmail = new QLineEdit(this);
    txtEmail->setPlaceholderText("exemple@email.com");
    txtEmail->setMinimumHeight(40);
    
    txtTelephone = new QLineEdit(this);
    txtTelephone->setPlaceholderText("Numéro de téléphone");
    txtTelephone->setMinimumHeight(40);
    
    txtAdresse = new QLineEdit(this);
    txtAdresse->setPlaceholderText("Adresse complète");
    txtAdresse->setMinimumHeight(40);

    formLayout->addRow("Nom *", txtNom);
    formLayout->addRow("Prénom", txtPrenom);
    formLayout->addRow("Email *", txtEmail);
    formLayout->addRow("Téléphone", txtTelephone);
    formLayout->addRow("Adresse", txtAdresse);

    mainLayout->addLayout(formLayout);

    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    btnSave = new QPushButton("💾 Enregistrer", this);
    btnSave->setMinimumHeight(45);
    connect(btnSave, &QPushButton::clicked, this, &ClientDialog::onSave);

    btnCancel = new QPushButton("❌ Annuler", this);
    btnCancel->setMinimumHeight(45);
    connect(btnCancel, &QPushButton::clicked, this, &ClientDialog::onCancel);

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnCancel);

    mainLayout->addLayout(buttonLayout);
}

void ClientDialog::loadClient(int clientId)
{
    QSqlQuery query;
    query.prepare("SELECT nom, prenom, telephone, email, adresse FROM CLIENTS WHERE id_client = :id");
    query.bindValue(":id", clientId);
    
    if (query.exec() && query.next()) {
        txtNom->setText(query.value(0).toString());
        txtPrenom->setText(query.value(1).toString());
        txtTelephone->setText(query.value(2).toString());
        txtEmail->setText(query.value(3).toString());
        txtAdresse->setText(query.value(4).toString());
    }
}

bool ClientDialog::validateInput()
{
    if (txtNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le nom est requis.");
        txtNom->setFocus();
        return false;
    }
    
    if (txtEmail->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "L'email est requis.");
        txtEmail->setFocus();
        return false;
    }
    
    // Validation email
    if (!txtEmail->text().contains('@')) {
        QMessageBox::warning(this, "Validation", "Email invalide.");
        txtEmail->setFocus();
        return false;
    }
    
    return true;
}

void ClientDialog::onSave()
{
    if (!validateInput()) {
        return;
    }

    QSqlQuery query;
    
    if (currentClientId == -1) {
        // Insertion
        query.prepare("INSERT INTO CLIENTS (nom, prenom, telephone, email, adresse) "
                     "VALUES (:nom, :prenom, :telephone, :email, :adresse)");
        query.bindValue(":nom", txtNom->text().trimmed());
        query.bindValue(":prenom", txtPrenom->text().trimmed());
        query.bindValue(":telephone", txtTelephone->text().trimmed());
        query.bindValue(":email", txtEmail->text().trimmed());
        query.bindValue(":adresse", txtAdresse->text().trimmed());
    } else {
        // Mise à jour
        query.prepare("UPDATE CLIENTS SET nom = :nom, prenom = :prenom, telephone = :telephone, "
                     "email = :email, adresse = :adresse WHERE id_client = :id");
        query.bindValue(":id", currentClientId);
        query.bindValue(":nom", txtNom->text().trimmed());
        query.bindValue(":prenom", txtPrenom->text().trimmed());
        query.bindValue(":telephone", txtTelephone->text().trimmed());
        query.bindValue(":email", txtEmail->text().trimmed());
        query.bindValue(":adresse", txtAdresse->text().trimmed());
    }

    if (query.exec()) {
        QMessageBox::information(this, "Succès", 
            currentClientId == -1 ? "Client ajouté avec succès!" : "Client modifié avec succès!");
        accept();
    } else {
        QMessageBox::critical(this, "Erreur", 
            "Erreur lors de l'enregistrement: " + query.lastError().text());
    }
}

void ClientDialog::onCancel()
{
    reject();
}