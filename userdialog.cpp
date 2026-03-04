#include "userdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QFileDialog>
#include <QPixmap>

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
    setStyleSheet("QDialog { background: #0f172a; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Titre
    QLabel *title = new QLabel(currentUserId == -1 ? "➕ Nouvel Utilisateur" : "✏️ Modifier l'Utilisateur", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #f1f5f9;");
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
    cboRole->addItems({"ADMIN", "VENDEUR"});
    cboRole->setMinimumHeight(40);
    
    chkActif = new QCheckBox("Compte actif", this);
    chkActif->setChecked(true);
    chkActif->setStyleSheet("QCheckBox { color: #f1f5f9; }");

    // Style des inputs dark mode
    QString inputStyle = 
        "QLineEdit, QComboBox {"
        "   border: 2px solid #334155;"
        "   border-radius: 6px;"
        "   padding: 8px 12px;"
        "   font-size: 14px;"
        "   background: #1e293b;"
        "   color: #f1f5f9;"
        "}"
        "QLineEdit:focus, QComboBox:focus {"
        "   border-color: #667eea;"
        "   background: #1e293b;"
        "}"
        "QLineEdit::placeholder {"
        "   color: #64748b;"
        "}"
        "QComboBox::drop-down {"
        "   background: #0f172a;"
        "}"
        "QComboBox::down-arrow {"
        "   color: #f1f5f9;"
        "}";
    
    txtNom->setStyleSheet(inputStyle);
    txtEmail->setStyleSheet(inputStyle);
    txtPassword->setStyleSheet(inputStyle);
    cboRole->setStyleSheet(inputStyle);

    // Style des labels
    QString labelStyle = "QLabel { color: #f1f5f9; }";

    QLabel *nomLabel = new QLabel("Nom complet *", this);
    nomLabel->setStyleSheet(labelStyle);
    formLayout->addRow(nomLabel, txtNom);
    
    QLabel *emailLabel = new QLabel("Email *", this);
    emailLabel->setStyleSheet(labelStyle);
    formLayout->addRow(emailLabel, txtEmail);
    
    QLabel *passLabel = new QLabel("Mot de passe *", this);
    passLabel->setStyleSheet(labelStyle);
    formLayout->addRow(passLabel, txtPassword);
    
    QLabel *roleLabel = new QLabel("Rôle *", this);
    roleLabel->setStyleSheet(labelStyle);
    formLayout->addRow(roleLabel, cboRole);
    formLayout->addRow("", chkActif);

    mainLayout->addLayout(formLayout);

    // Section photo de profil
    QLabel *photoLabel = new QLabel("Photo de profil", this);
    photoLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #f1f5f9; margin-top: 20px;");
    mainLayout->addWidget(photoLabel);

    QHBoxLayout *photoLayout = new QHBoxLayout();
    photoLayout->setSpacing(20);

    lblPhotoPreview = new QLabel(this);
    lblPhotoPreview->setFixedSize(150, 150);
    lblPhotoPreview->setStyleSheet("border: 2px dashed #334155; border-radius: 8px; background: #1e293b;");
    lblPhotoPreview->setAlignment(Qt::AlignCenter);
    lblPhotoPreview->setText("📷");
    lblPhotoPreview->setStyleSheet("border: 2px dashed #334155; border-radius: 8px; background: #1e293b; font-size: 48px;");

    btnSelectPhoto = new QPushButton("📁 Sélectionner une photo", this);
    btnSelectPhoto->setMinimumHeight(45);
    btnSelectPhoto->setStyleSheet(
        "QPushButton {"
        "   background: #3b82f6;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: #2563eb;"
        "}"
        "QPushButton:pressed {"
        "   background: #1d4ed8;"
        "}"
    );
    connect(btnSelectPhoto, &QPushButton::clicked, this, &UserDialog::onSelectPhoto);

    photoLayout->addWidget(lblPhotoPreview);
    photoLayout->addWidget(btnSelectPhoto);
    photoLayout->addStretch();

    mainLayout->addLayout(photoLayout);

    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    btnSave = new QPushButton("💾 Enregistrer", this);
    btnSave->setMinimumHeight(45);
    btnSave->setStyleSheet(
        "QPushButton {"
        "   background: #10b981;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 10px 30px;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "   outline: none;"
        "}"
        "QPushButton:hover {"
        "   background: #059669;"
        "}"
        "QPushButton:pressed {"
        "   background: #047857;"
        "}"
    );
    connect(btnSave, &QPushButton::clicked, this, &UserDialog::onSave);

    btnCancel = new QPushButton("❌ Annuler", this);
    btnCancel->setMinimumHeight(45);
    btnCancel->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #e53e3e;"
        "   border: 2px solid #e53e3e;"
        "   border-radius: 6px;"
        "   padding: 10px 30px;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "   outline: none;"
        "}"
        "QPushButton:hover {"
        "   background: #e53e3e;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background: #c53030;"
        "}"
    );
    connect(btnCancel, &QPushButton::clicked, this, &UserDialog::onCancel);

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnCancel);

    mainLayout->addLayout(buttonLayout);
}

void UserDialog::loadUser(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT nom, email, role, actif, photo_profile FROM USERS WHERE id_user = :id");
    query.bindValue(":id", userId);
    
    if (query.exec() && query.next()) {
        txtNom->setText(query.value(0).toString());
        txtEmail->setText(query.value(1).toString());
        cboRole->setCurrentText(query.value(2).toString());
        chkActif->setChecked(query.value(3).toBool());
        photoPath = query.value(4).toString();
        displayPhotoPreview();
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
        query.prepare("INSERT INTO USERS (nom, email, mot_de_passe, role, actif, photo_profile) "
                     "VALUES (:nom, :email, :password, :role, :actif, :photo)");
        query.bindValue(":nom", txtNom->text().trimmed());
        query.bindValue(":email", txtEmail->text().trimmed());
        query.bindValue(":password", hashedPassword);
        query.bindValue(":role", cboRole->currentText());
        query.bindValue(":actif", chkActif->isChecked() ? 1 : 0);
        query.bindValue(":photo", photoPath);
    } else {
        // Mise à jour
        if (hashedPassword.isEmpty()) {
            query.prepare("UPDATE USERS SET nom = :nom, email = :email, role = :role, actif = :actif, photo_profile = :photo "
                         "WHERE id_user = :id");
        } else {
            query.prepare("UPDATE USERS SET nom = :nom, email = :email, mot_de_passe = :password, "
                         "role = :role, actif = :actif, photo_profile = :photo WHERE id_user = :id");
            query.bindValue(":password", hashedPassword);
        }
        query.bindValue(":id", currentUserId);
        query.bindValue(":nom", txtNom->text().trimmed());
        query.bindValue(":email", txtEmail->text().trimmed());
        query.bindValue(":role", cboRole->currentText());
        query.bindValue(":actif", chkActif->isChecked() ? 1 : 0);
        query.bindValue(":photo", photoPath);
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

void UserDialog::onSelectPhoto()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Sélectionner une photo", "",
        "Images (*.png *.jpg *.jpeg *.bmp);;Tous les fichiers (*)");
    
    if (!fileName.isEmpty()) {
        photoPath = fileName;
        displayPhotoPreview();
    }
}

void UserDialog::displayPhotoPreview()
{
    if (photoPath.isEmpty()) {
        lblPhotoPreview->clear();
        lblPhotoPreview->setText("📷");
        lblPhotoPreview->setStyleSheet("border: 2px dashed #334155; border-radius: 8px; background: #1e293b; font-size: 48px;");
    } else {
        QPixmap pixmap(photoPath);
        if (!pixmap.isNull()) {
            QPixmap scaledPixmap = pixmap.scaledToWidth(146, Qt::SmoothTransformation);
            lblPhotoPreview->setPixmap(scaledPixmap);
            lblPhotoPreview->setStyleSheet("border: 2px solid #667eea; border-radius: 8px; background: #1e293b;");
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de charger l'image.");
            photoPath = "";
            lblPhotoPreview->clear();
            lblPhotoPreview->setText("📷");
            lblPhotoPreview->setStyleSheet("border: 2px dashed #334155; border-radius: 8px; background: #1e293b; font-size: 48px;");
        }
    }
}