#include "userdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

UserDialog::UserDialog(QWidget *parent, int userId)
    : QDialog(parent), currentUserId(userId)
{
    setupUI();
    if (userId != -1) {
        loadUser(userId);
    }
}

void UserDialog::setupUI()
{
    setWindowTitle(currentUserId == -1 ? "Ajouter un utilisateur" : "Modifier l'utilisateur");
    setMinimumWidth(500);
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Titre
    QLabel *title = new QLabel(currentUserId == -1 ? "➕ Nouvel Utilisateur" : "✏️ Modifier l'Utilisateur", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(title);

    // Formulaire
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    txtNom = new QLineEdit(this);
    txtNom->setPlaceholderText("Entrez le nom complet");
    txtNom->setMinimumHeight(40);
    
    txtEmail = new QLineEdit(this);
    txtEmail->setPlaceholderText("exemple@email.com");
    txtEmail->setMinimumHeight(40);
    
    txtPassword = new QLineEdit(this);
    txtPassword->setEchoMode(QLineEdit::Password);
    txtPassword->setPlaceholderText(currentUserId == -1 ? "Mot de passe" : "Laisser vide pour ne pas changer");
    txtPassword->setMinimumHeight(40);
    
    cboRole = new QComboBox(this);
    cboRole->addItems({"ADMIN", "VENDEUR", "CAISSIER"});
    cboRole->setMinimumHeight(40);
    
    chkActif = new QCheckBox("Compte actif", this);
    chkActif->setChecked(true);

    // Style des inputs
    QString inputStyle = 
        "QLineEdit, QComboBox {"
        "   border: 2px solid #e0e0e0;"
        "   border-radius: 6px;"
        "   padding: 8px 12px;"
        "   font-size: 14px;"
        "   background: white;"
        "}"
        "QLineEdit:focus, QComboBox:focus {"
        "   border-color: #3498db;"
        "}";
    
    txtNom->setStyleSheet(inputStyle);
    txtEmail->setStyleSheet(inputStyle);
    txtPassword->setStyleSheet(inputStyle);
    cboRole->setStyleSheet(inputStyle);

    formLayout->addRow("Nom complet *", txtNom);
    formLayout->addRow("Email *", txtEmail);
    formLayout->addRow("Mot de passe *", txtPassword);
    formLayout->addRow("Rôle *", cboRole);
    formLayout->addRow("", chkActif);

    mainLayout->addLayout(formLayout);

    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    btnSave = new QPushButton("💾 Enregistrer", this);
    btnSave->setMinimumHeight(45);
    btnSave->setStyleSheet(
        "QPushButton {"
        "   background: #27ae60;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 10px 30px;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: #229954;"
        "}"
    );
    connect(btnSave, &QPushButton::clicked, this, &UserDialog::onSave);

    btnCancel = new QPushButton("❌ Annuler", this);
    btnCancel->setMinimumHeight(45);
    btnCancel->setStyleSheet(
        "QPushButton {"
        "   background: #95a5a6;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 10px 30px;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: #7f8c8d;"
        "}"
    );
    connect(btnCancel, &QPushButton::clicked, this, &UserDialog::onCancel);

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnCancel);

    mainLayout->addLayout(buttonLayout);

    setStyleSheet("QDialog { background: #f5f6fa; }");
}

void UserDialog::loadUser(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT nom, email, role, actif FROM USERS WHERE id_user = :id");
    query.bindValue(":id", userId);
    
    if (query.exec() && query.next()) {
        txtNom->setText(query.value(0).toString());
        txtEmail->setText(query.value(1).toString());
        cboRole->setCurrentText(query.value(2).toString());
        chkActif->setChecked(query.value(3).toBool());
    }
}

bool UserDialog::validateInput()
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
    
    if (currentUserId == -1 && txtPassword->text().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le mot de passe est requis pour un nouvel utilisateur.");
        txtPassword->setFocus();
        return false;
    }
    
    if (!txtPassword->text().isEmpty() && txtPassword->text().length() < 6) {
        QMessageBox::warning(this, "Validation", "Le mot de passe doit contenir au moins 6 caractères.");
        txtPassword->setFocus();
        return false;
    }
    
    return true;
}

void UserDialog::onSave()
{
    if (!validateInput()) {
        return;
    }

    QSqlQuery query;
    
    // Hash du mot de passe
    QString hashedPassword;
    if (!txtPassword->text().isEmpty()) {
        QByteArray hash = QCryptographicHash::hash(
            txtPassword->text().toUtf8(), 
            QCryptographicHash::Sha256
        );
        hashedPassword = hash.toHex();
    }

    if (currentUserId == -1) {
        // Insertion
        query.prepare("INSERT INTO USERS (nom, email, mot_de_passe, role, actif) "
                     "VALUES (:nom, :email, :password, :role, :actif)");
        query.bindValue(":nom", txtNom->text().trimmed());
        query.bindValue(":email", txtEmail->text().trimmed());
        query.bindValue(":password", hashedPassword);
        query.bindValue(":role", cboRole->currentText());
        query.bindValue(":actif", chkActif->isChecked() ? 1 : 0);
    } else {
        // Mise à jour
        if (hashedPassword.isEmpty()) {
            query.prepare("UPDATE USERS SET nom = :nom, email = :email, role = :role, actif = :actif "
                         "WHERE id_user = :id");
        } else {
            query.prepare("UPDATE USERS SET nom = :nom, email = :email, mot_de_passe = :password, "
                         "role = :role, actif = :actif WHERE id_user = :id");
            query.bindValue(":password", hashedPassword);
        }
        query.bindValue(":id", currentUserId);
        query.bindValue(":nom", txtNom->text().trimmed());
        query.bindValue(":email", txtEmail->text().trimmed());
        query.bindValue(":role", cboRole->currentText());
        query.bindValue(":actif", chkActif->isChecked() ? 1 : 0);
    }

    if (query.exec()) {
        QMessageBox::information(this, "Succès", 
            currentUserId == -1 ? "Utilisateur ajouté avec succès!" : "Utilisateur modifié avec succès!");
        accept();
    } else {
        QMessageBox::critical(this, "Erreur", 
            "Erreur lors de l'enregistrement: " + query.lastError().text());
    }
}

void UserDialog::onCancel()
{
    reject();
}