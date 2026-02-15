#include "profilepanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QPixmap>
#include <QFont>
#include <QGroupBox>
#include <QFormLayout>
#include <QDateTime>
#include <QCryptographicHash>
#include <QInputDialog>

ProfilePanel::ProfilePanel(int userId, QWidget *parent)
    : QFrame(parent), currentUserId(userId), isEditMode(false)
{
    setObjectName("profilePanel");
    setupUI();
    applyStyles();
    loadUserData();
}

void ProfilePanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // En-tête avec icône et titre
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QLabel *icon = new QLabel(this);
    icon->setText("👤");
    icon->setStyleSheet("font-size: 48px;");
    
    QLabel *title = new QLabel("Mon Profil", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #f1f5f9;");
    
    headerLayout->addWidget(icon);
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    
    mainLayout->addLayout(headerLayout);

    // ===== MODE AFFICHAGE =====
    displayContainer = new QWidget(this);
    QVBoxLayout *displayLayout = new QVBoxLayout(displayContainer);
    displayLayout->setSpacing(24);

    // Section photo
    QHBoxLayout *photoLayout = new QHBoxLayout();
    
    lblPhotoDisplay = new QLabel(this);
    lblPhotoDisplay->setFixedSize(180, 180);
    lblPhotoDisplay->setStyleSheet(
        "border: 3px solid #667eea;"
        "border-radius: 12px;"
        "background: #1e293b;"
    );
    lblPhotoDisplay->setAlignment(Qt::AlignCenter);
    lblPhotoDisplay->setText("📷");
    lblPhotoDisplay->setStyleSheet(
        "border: 3px solid #667eea;"
        "border-radius: 12px;"
        "background: #1e293b;"
        "font-size: 80px;"
    );
    
    photoLayout->addWidget(lblPhotoDisplay);
    
    // Section infos utilisateur
    QGroupBox *infoGroup = new QGroupBox("Informations Personnelles", this);
    infoGroup->setStyleSheet(
        "QGroupBox {"
        "   color: #f1f5f9;"
        "   border: 2px solid #334155;"
        "   border-radius: 8px;"
        "   margin-top: 8px;"
        "   padding-top: 12px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 12px;"
        "   padding: 0 3px 0 3px;"
        "}"
    );
    
    QFormLayout *formLayout = new QFormLayout(infoGroup);
    formLayout->setSpacing(16);
    
    lblNomDisplay = new QLabel(this);
    lblNomDisplay->setStyleSheet("color: #e2e8f0; font-size: 14px;");
    
    lblEmailDisplay = new QLabel(this);
    lblEmailDisplay->setStyleSheet("color: #e2e8f0; font-size: 14px;");
    lblEmailDisplay->setTextInteractionFlags(Qt::TextSelectableByMouse);
    
    lblRoleDisplay = new QLabel(this);
    lblRoleDisplay->setStyleSheet("color: #e2e8f0; font-size: 14px;");
    
    lblStatusDisplay = new QLabel(this);
    lblStatusDisplay->setStyleSheet("color: #10b981; font-size: 14px; font-weight: bold;");
    
    lblDateCreationDisplay = new QLabel(this);
    lblDateCreationDisplay->setStyleSheet("color: #cbd5e1; font-size: 12px;");
    
    QLabel *nomLabel = new QLabel("Nom complet:", this);
    nomLabel->setStyleSheet("color: #cbd5e1; font-weight: bold;");
    formLayout->addRow(nomLabel, lblNomDisplay);
    
    QLabel *emailLabel = new QLabel("Email:", this);
    emailLabel->setStyleSheet("color: #cbd5e1; font-weight: bold;");
    formLayout->addRow(emailLabel, lblEmailDisplay);
    
    QLabel *roleLabel = new QLabel("Rôle:", this);
    roleLabel->setStyleSheet("color: #cbd5e1; font-weight: bold;");
    formLayout->addRow(roleLabel, lblRoleDisplay);
    
    QLabel *statusLabel = new QLabel("Statut:", this);
    statusLabel->setStyleSheet("color: #cbd5e1; font-weight: bold;");
    formLayout->addRow(statusLabel, lblStatusDisplay);
    
    QLabel *dateLabel = new QLabel("Membre depuis:", this);
    dateLabel->setStyleSheet("color: #cbd5e1; font-weight: bold;");
    formLayout->addRow(dateLabel, lblDateCreationDisplay);
    
    photoLayout->addWidget(infoGroup, 1);
    
    displayLayout->addLayout(photoLayout);

    // Boutons d'action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    btnEditProfile = new QPushButton("✏️ Modifier le profil", this);
    btnEditProfile->setMinimumHeight(45);
    btnEditProfile->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #5568d3, stop:1 #6b3fa0);"
        "}"
        "QPushButton:pressed {"
        "   background: #4f46e5;"
        "}"
    );
    connect(btnEditProfile, &QPushButton::clicked, this, &ProfilePanel::onEditProfile);
    
    btnChangePassword = new QPushButton("🔐 Changer le mot de passe", this);
    btnChangePassword->setMinimumHeight(45);
    btnChangePassword->setStyleSheet(
        "QPushButton {"
        "   background: #f59e0b;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: #d97706;"
        "}"
        "QPushButton:pressed {"
        "   background: #b45309;"
        "}"
    );
    connect(btnChangePassword, &QPushButton::clicked, this, &ProfilePanel::onChangePassword);
    
    buttonLayout->addWidget(btnEditProfile);
    buttonLayout->addWidget(btnChangePassword);
    buttonLayout->addStretch();
    
    displayLayout->addLayout(buttonLayout);
    displayLayout->addStretch();
    
    mainLayout->addWidget(displayContainer);

    // ===== MODE ÉDITION =====
    editContainer = new QWidget(this);
    QVBoxLayout *editLayout = new QVBoxLayout(editContainer);
    editLayout->setSpacing(20);

    // Section photo en mode édition
    QGroupBox *photoGroup = new QGroupBox("Photo de Profil", this);
    photoGroup->setStyleSheet(
        "QGroupBox {"
        "   color: #f1f5f9;"
        "   border: 2px solid #334155;"
        "   border-radius: 8px;"
        "   margin-top: 8px;"
        "   padding-top: 12px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 12px;"
        "   padding: 0 3px 0 3px;"
        "}"
    );
    
    QVBoxLayout *photoEditLayout = new QVBoxLayout(photoGroup);
    photoEditLayout->setSpacing(12);
    
    QHBoxLayout *photoPreviewLayout = new QHBoxLayout();
    
    lblPhotoEdit = new QLabel(this);
    lblPhotoEdit->setFixedSize(150, 150);
    lblPhotoEdit->setStyleSheet(
        "border: 2px dashed #334155;"
        "border-radius: 8px;"
        "background: #1e293b;"
        "font-size: 60px;"
    );
    lblPhotoEdit->setAlignment(Qt::AlignCenter);
    lblPhotoEdit->setText("📷");
    
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
    connect(btnSelectPhoto, &QPushButton::clicked, this, &ProfilePanel::onSelectPhoto);
    
    photoPreviewLayout->addWidget(lblPhotoEdit);
    photoPreviewLayout->addWidget(btnSelectPhoto);
    photoPreviewLayout->addStretch();
    
    photoEditLayout->addLayout(photoPreviewLayout);
    
    editLayout->addWidget(photoGroup);

    // Section infos en mode édition
    QGroupBox *editGroup = new QGroupBox("Modifier les Informations", this);
    editGroup->setStyleSheet(
        "QGroupBox {"
        "   color: #f1f5f9;"
        "   border: 2px solid #334155;"
        "   border-radius: 8px;"
        "   margin-top: 8px;"
        "   padding-top: 12px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 12px;"
        "   padding: 0 3px 0 3px;"
        "}"
    );
    
    QFormLayout *editFormLayout = new QFormLayout(editGroup);
    editFormLayout->setSpacing(15);
    
    txtNomEdit = new QLineEdit(this);
    txtNomEdit->setMinimumHeight(40);
    
    txtEmailEdit = new QLineEdit(this);
    txtEmailEdit->setMinimumHeight(40);
    
    cboRoleEdit = new QComboBox(this);
    cboRoleEdit->addItems({"ADMIN", "VENDEUR", "CAISSIER"});
    cboRoleEdit->setMinimumHeight(40);
    cboRoleEdit->setEnabled(false);  // Le rôle ne peut pas être modifié ici
    
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
        "}"
        "QLineEdit::placeholder {"
        "   color: #64748b;"
        "}"
        "QComboBox::drop-down {"
        "   background: #0f172a;"
        "}"
        "QComboBox::down-arrow {"
        "   color: #f1f5f9;"
        "}"
        "QComboBox:disabled {"
        "   background: #0f172a;"
        "   color: #64748b;"
        "}";
    
    txtNomEdit->setStyleSheet(inputStyle);
    txtEmailEdit->setStyleSheet(inputStyle);
    cboRoleEdit->setStyleSheet(inputStyle);
    
    QString labelStyle = "color: #f1f5f9; font-weight: bold;";
    
    QLabel *nomEditLabel = new QLabel("Nom complet:", this);
    nomEditLabel->setStyleSheet(labelStyle);
    editFormLayout->addRow(nomEditLabel, txtNomEdit);
    
    QLabel *emailEditLabel = new QLabel("Email:", this);
    emailEditLabel->setStyleSheet(labelStyle);
    editFormLayout->addRow(emailEditLabel, txtEmailEdit);
    
    QLabel *roleEditLabel = new QLabel("Rôle:", this);
    roleEditLabel->setStyleSheet(labelStyle);
    editFormLayout->addRow(roleEditLabel, cboRoleEdit);
    
    editLayout->addWidget(editGroup);
    editLayout->addStretch();

    // Boutons de sauvegarde/annulation
    QHBoxLayout *editButtonLayout = new QHBoxLayout();
    
    btnSaveProfile = new QPushButton("💾 Enregistrer", this);
    btnSaveProfile->setMinimumHeight(45);
    btnSaveProfile->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #059669);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 10px 30px;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #059669, stop:1 #047857);"
        "}"
        "QPushButton:pressed {"
        "   background: #047857;"
        "}"
    );
    connect(btnSaveProfile, &QPushButton::clicked, this, &ProfilePanel::onSaveProfile);
    
    btnCancelEdit = new QPushButton("❌ Annuler", this);
    btnCancelEdit->setMinimumHeight(45);
    btnCancelEdit->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #e53e3e;"
        "   border: 2px solid #e53e3e;"
        "   border-radius: 6px;"
        "   padding: 10px 30px;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: #e53e3e;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background: #c53030;"
        "}"
    );
    connect(btnCancelEdit, &QPushButton::clicked, this, &ProfilePanel::onCancelEdit);
    
    editButtonLayout->addStretch();
    editButtonLayout->addWidget(btnSaveProfile);
    editButtonLayout->addWidget(btnCancelEdit);
    
    editLayout->addLayout(editButtonLayout);
    
    mainLayout->addWidget(editContainer);
    
    // Masquer le mode édition initialement
    editContainer->hide();
}

void ProfilePanel::applyStyles()
{
    setStyleSheet(
        "ProfilePanel {"
        "   background: #0f172a;"
        "}"
    );
}

void ProfilePanel::loadUserData()
{
    QSqlQuery query;
    query.prepare("SELECT nom, email, role, actif, date_creation FROM USERS WHERE id_user = :id");
    query.bindValue(":id", currentUserId);
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Impossible de charger les données utilisateur: " + query.lastError().text());
        return;
    }
    
    if (query.next()) {
        QString nom = query.value(0).toString();
        QString email = query.value(1).toString();
        QString role = query.value(2).toString();
        bool actif = query.value(3).toBool();
        QString dateCreation = query.value(4).toString();
        
        // Mise à jour mode affichage
        lblNomDisplay->setText(nom);
        lblEmailDisplay->setText(email);
        
        // Formatage du rôle avec couleur
        QString roleDisplay = role;
        QString roleColor = "#667eea";
        if (role == "ADMIN") {
            roleColor = "#e53e3e";
            roleDisplay = "Administrateur";
        } else if (role == "VENDEUR") {
            roleColor = "#3b82f6";
            roleDisplay = "Vendeur";
        } else if (role == "CAISSIER") {
            roleColor = "#f59e0b";
            roleDisplay = "Caissier";
        }
        
        lblRoleDisplay->setText(roleDisplay);
        lblRoleDisplay->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;").arg(roleColor));
        
        lblStatusDisplay->setText(actif ? "Actif ✓" : "Inactif");
        lblStatusDisplay->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;")
            .arg(actif ? "#10b981" : "#ef4444"));
        
        // Formatage de la date
        QDateTime dt = QDateTime::fromString(dateCreation, "yyyy-MM-dd hh:mm:ss");
        QString formattedDate = dt.toString("d MMMM yyyy");
        lblDateCreationDisplay->setText(formattedDate);
        
        // Mise à jour mode édition
        txtNomEdit->setText(nom);
        txtEmailEdit->setText(email);
        cboRoleEdit->setCurrentText(role);
        
        // Charger la photo séparément
        loadPhotoProfile();
    }
}

void ProfilePanel::loadPhotoProfile()
{
    QSqlQuery query;
    query.prepare("SELECT photo_profile FROM USERS WHERE id_user = :id");
    query.bindValue(":id", currentUserId);
    
    if (query.exec() && query.next()) {
        currentPhotoPath = query.value(0).toString();
        originalPhotoPath = currentPhotoPath;
    } else {
        currentPhotoPath = "";
        originalPhotoPath = "";
    }
    
    loadPhotoFromDatabase();
}

void ProfilePanel::loadPhotoFromDatabase()
{
    if (currentPhotoPath.isEmpty()) {
        lblPhotoDisplay->clear();
        lblPhotoDisplay->setText("📷");
        lblPhotoDisplay->setStyleSheet(
            "border: 3px solid #667eea;"
            "border-radius: 12px;"
            "background: #1e293b;"
            "font-size: 80px;"
        );
        
        lblPhotoEdit->clear();
        lblPhotoEdit->setText("📷");
    } else {
        QPixmap pixmap(currentPhotoPath);
        if (!pixmap.isNull()) {
            QPixmap scaledPixmapDisplay = pixmap.scaledToWidth(176, Qt::SmoothTransformation);
            lblPhotoDisplay->setPixmap(scaledPixmapDisplay);
            lblPhotoDisplay->setStyleSheet(
                "border: 3px solid #667eea;"
                "border-radius: 12px;"
                "background: #1e293b;"
            );
            
            QPixmap scaledPixmapEdit = pixmap.scaledToWidth(146, Qt::SmoothTransformation);
            lblPhotoEdit->setPixmap(scaledPixmapEdit);
            lblPhotoEdit->setStyleSheet(
                "border: 2px solid #667eea;"
                "border-radius: 8px;"
                "background: #1e293b;"
            );
        }
    }
}

void ProfilePanel::onEditProfile()
{
    setupEditMode(true);
    displayContainer->hide();
    editContainer->show();
}

void ProfilePanel::onCancelEdit()
{
    setupEditMode(false);
    currentPhotoPath = originalPhotoPath;  // Restaurer la photo originale
    loadPhotoFromDatabase();
    editContainer->hide();
    displayContainer->show();
}

void ProfilePanel::onSelectPhoto()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Sélectionner une photo", "",
        "Images (*.png *.jpg *.jpeg *.bmp);;Tous les fichiers (*)");
    
    if (!fileName.isEmpty()) {
        currentPhotoPath = fileName;
        displayPhotoPreview();
    }
}

void ProfilePanel::displayPhotoPreview()
{
    QPixmap pixmap(currentPhotoPath);
    if (!pixmap.isNull()) {
        QPixmap scaledPixmap = pixmap.scaledToWidth(146, Qt::SmoothTransformation);
        lblPhotoEdit->setPixmap(scaledPixmap);
        lblPhotoEdit->setStyleSheet(
            "border: 2px solid #667eea;"
            "border-radius: 8px;"
            "background: #1e293b;"
        );
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible de charger l'image.");
        currentPhotoPath = "";
        lblPhotoEdit->clear();
        lblPhotoEdit->setText("📷");
    }
}

void ProfilePanel::setupEditMode(bool isEdit)
{
    isEditMode = isEdit;
}

bool ProfilePanel::validateInput()
{
    if (txtNomEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le nom est requis.");
        txtNomEdit->setFocus();
        return false;
    }
    
    if (txtEmailEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "L'email est requis.");
        txtEmailEdit->setFocus();
        return false;
    }
    
    if (!txtEmailEdit->text().contains('@')) {
        QMessageBox::warning(this, "Validation", "Email invalide.");
        txtEmailEdit->setFocus();
        return false;
    }
    
    return true;
}

void ProfilePanel::onSaveProfile()
{
    if (!validateInput()) {
        return;
    }
    
    QSqlQuery query;
    
    // Vérifier si la photo a changé
    if (currentPhotoPath == originalPhotoPath) {
        // Pas de changement de photo
        query.prepare("UPDATE USERS SET nom = :nom, email = :email WHERE id_user = :id");
    } else {
        // Photo changée
        query.prepare("UPDATE USERS SET nom = :nom, email = :email, photo_profile = :photo WHERE id_user = :id");
    }
    
    query.bindValue(":id", currentUserId);
    query.bindValue(":nom", txtNomEdit->text().trimmed());
    query.bindValue(":email", txtEmailEdit->text().trimmed());
    
    // Lier la photo seulement si elle a changé
    if (currentPhotoPath != originalPhotoPath) {
        query.bindValue(":photo", currentPhotoPath);
    }
    
    if (query.exec()) {
        QMessageBox::information(this, "Succès", "Profil mis à jour avec succès!");
        loadUserData();
        onCancelEdit();
    } else {
        QMessageBox::critical(this, "Erreur", 
            "Erreur lors de la mise à jour: " + query.lastError().text());
    }
}

void ProfilePanel::onChangePassword()
{
    bool ok;
    QString oldPassword = QInputDialog::getText(this,
        tr("Changer le mot de passe"),
        tr("Ancien mot de passe:"),
        QLineEdit::Password, "", &ok);
    
    if (!ok || oldPassword.isEmpty()) {
        return;
    }
    
    // Vérifier l'ancien mot de passe
    QSqlQuery query;
    query.prepare("SELECT mot_de_passe FROM USERS WHERE id_user = :id");
    query.bindValue(":id", currentUserId);
    
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Erreur", "Impossible de vérifier le mot de passe.");
        return;
    }
    
    QString storedHash = query.value(0).toString();
    QString inputHash = QCryptographicHash::hash(
        oldPassword.toUtf8(), 
        QCryptographicHash::Sha256
    ).toHex();
    
    if (storedHash != inputHash) {
        QMessageBox::warning(this, "Erreur", "L'ancien mot de passe est incorrect.");
        return;
    }
    
    // Demander le nouveau mot de passe
    QString newPassword = QInputDialog::getText(this,
        tr("Nouveau mot de passe"),
        tr("Nouveau mot de passe:"),
        QLineEdit::Password, "", &ok);
    
    if (!ok || newPassword.isEmpty()) {
        return;
    }
    
    if (newPassword.length() < 6) {
        QMessageBox::warning(this, "Erreur", "Le mot de passe doit contenir au moins 6 caractères.");
        return;
    }
    
    // Confirmer le nouveau mot de passe
    QString confirmPassword = QInputDialog::getText(this,
        tr("Confirmer le mot de passe"),
        tr("Confirmez le nouveau mot de passe:"),
        QLineEdit::Password, "", &ok);
    
    if (!ok || confirmPassword.isEmpty() || confirmPassword != newPassword) {
        QMessageBox::warning(this, "Erreur", "Les mots de passe ne correspondent pas.");
        return;
    }
    
    // Mettre à jour
    QString newHash = QCryptographicHash::hash(
        newPassword.toUtf8(), 
        QCryptographicHash::Sha256
    ).toHex();
    
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE USERS SET mot_de_passe = :password WHERE id_user = :id");
    updateQuery.bindValue(":password", newHash);
    updateQuery.bindValue(":id", currentUserId);
    
    if (updateQuery.exec()) {
        QMessageBox::information(this, "Succès", "Mot de passe changé avec succès!");
    } else {
        QMessageBox::critical(this, "Erreur", 
            "Erreur lors de la modification: " + updateQuery.lastError().text());
    }
}
