#include "productdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QPixmap>
#include <QDir>
#include <QStandardPaths>
#include <QDoubleValidator>
#include <QIntValidator>

ProductDialog::ProductDialog(QWidget *parent, int productId)
    : QDialog(parent), currentProductId(productId), selectedImagePath("")
{
    setupUI();
    if (productId != -1) {
        loadProduct(productId);
    }
}

void ProductDialog::setupUI()
{
    setWindowTitle(currentProductId == -1 ? "Ajouter un produit" : "Modifier le produit");
    setMinimumWidth(600);
    setModal(true);
    setStyleSheet("QDialog { background: #0f172a; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Titre
    QLabel *title = new QLabel(currentProductId == -1 ? "➕ Nouveau Produit" : "✏️ Modifier le Produit", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #f1f5f9;");
    mainLayout->addWidget(title);

    // Formulaire
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    txtNom = new QLineEdit(this);
    txtNom->setPlaceholderText("Nom du produit");
    txtNom->setMinimumHeight(40);

    txtDescription = new QTextEdit(this);
    txtDescription->setPlaceholderText("Description du produit");
    txtDescription->setMinimumHeight(80);
    txtDescription->setMaximumHeight(120);

    txtPrixVente = new QLineEdit(this);
    txtPrixVente->setPlaceholderText("0.00");
    txtPrixVente->setMinimumHeight(40);
    txtPrixVente->setValidator(new QDoubleValidator(0, 999999, 2, this));

    txtPrixAchat = new QLineEdit(this);
    txtPrixAchat->setPlaceholderText("0.00");
    txtPrixAchat->setMinimumHeight(40);
    txtPrixAchat->setValidator(new QDoubleValidator(0, 999999, 2, this));

    txtStock = new QLineEdit(this);
    txtStock->setPlaceholderText("0");
    txtStock->setMinimumHeight(40);
    txtStock->setValidator(new QIntValidator(0, 999999, this));

    txtSeuilAlerte = new QLineEdit(this);
    txtSeuilAlerte->setPlaceholderText("5");
    txtSeuilAlerte->setMinimumHeight(40);
    txtSeuilAlerte->setValidator(new QIntValidator(0, 999999, this));

    // Section image
    QHBoxLayout *imageLayout = new QHBoxLayout();
    lblImagePreview = new QLabel(this);
    lblImagePreview->setFixedSize(120, 120);
    lblImagePreview->setStyleSheet(
        "border: 2px dashed #334155; "
        "border-radius: 8px; "
        "background: #1e293b;"
        "color: #94a3b8;"
    );
    lblImagePreview->setAlignment(Qt::AlignCenter);
    lblImagePreview->setText("📷\nAucune image");

    btnSelectImage = new QPushButton("Choisir une image", this);
    btnSelectImage->setMinimumHeight(40);
    btnSelectImage->setStyleSheet(
        "QPushButton {"
        "   background: #667eea;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: #5568d3;"
        "}"
    );
    connect(btnSelectImage, &QPushButton::clicked, this, &ProductDialog::onSelectImage);

    imageLayout->addWidget(lblImagePreview);
    imageLayout->addWidget(btnSelectImage);
    imageLayout->addStretch();

    // Style des inputs
    QString inputStyle =
        "QLineEdit, QTextEdit {"
        "   border: 2px solid #334155;"
        "   border-radius: 6px;"
        "   padding: 8px 12px;"
        "   font-size: 14px;"
        "   background: #1e293b;"
        "   color: #f1f5f9;"
        "}"
        "QLineEdit:focus, QTextEdit:focus {"
        "   border-color: #667eea;"
        "   background: #1e293b;"
        "}"
        "QLineEdit::placeholder, QTextEdit::placeholder {"
        "   color: #64748b;"
        "}";

    txtNom->setStyleSheet(inputStyle);
    txtDescription->setStyleSheet(inputStyle + " QTextEdit { max-height: 120px; }");
    txtPrixVente->setStyleSheet(inputStyle);
    txtPrixAchat->setStyleSheet(inputStyle);
    txtStock->setStyleSheet(inputStyle);
    txtSeuilAlerte->setStyleSheet(inputStyle);

    // Style des labels
    QString labelStyle = "QLabel { color: #f1f5f9; }";
    
    QLabel *nomLabel = new QLabel("Nom du produit *", this);
    nomLabel->setStyleSheet(labelStyle);
    QLabel *descLabel = new QLabel("Description", this);
    descLabel->setStyleSheet(labelStyle);
    QLabel *prixVenteLabel = new QLabel("Prix de vente (€) *", this);
    prixVenteLabel->setStyleSheet(labelStyle);
    QLabel *prixAchatLabel = new QLabel("Prix d'achat (€)", this);
    prixAchatLabel->setStyleSheet(labelStyle);
    QLabel *stockLabel = new QLabel("Stock *", this);
    stockLabel->setStyleSheet(labelStyle);
    QLabel *seuilLabel = new QLabel("Seuil d'alerte", this);
    seuilLabel->setStyleSheet(labelStyle);
    QLabel *imageLabel = new QLabel("Image du produit", this);
    imageLabel->setStyleSheet(labelStyle);

    formLayout->addRow(nomLabel, txtNom);
    formLayout->addRow(descLabel, txtDescription);
    formLayout->addRow(prixVenteLabel, txtPrixVente);
    formLayout->addRow(prixAchatLabel, txtPrixAchat);
    formLayout->addRow(stockLabel, txtStock);
    formLayout->addRow(seuilLabel, txtSeuilAlerte);
    formLayout->addRow(imageLabel, imageLayout);

    mainLayout->addLayout(formLayout);

    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    btnSave = new QPushButton("💾 Enregistrer", this);
    btnSave->setMinimumHeight(45);
    btnSave->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #10b981, stop: 1 #059669);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 10px 30px;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #059669, stop: 1 #047857);"
        "}"
        "QPushButton:pressed {"
        "   background: #047857;"
        "}"
    );
    connect(btnSave, &QPushButton::clicked, this, &ProductDialog::onSave);

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
        "}"
        "QPushButton:hover {"
        "   background: #e53e3e;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background: #c53030;"
        "   border-color: #c53030;"
        "}"
    );
    connect(btnCancel, &QPushButton::clicked, this, &ProductDialog::onCancel);

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnCancel);

    mainLayout->addLayout(buttonLayout);
}

void ProductDialog::loadProduct(int productId)
{
    QSqlQuery query;
    query.prepare("SELECT nom_produit, description, photo_produit, prix_vente, prix_achat, stock, seuil_alerte FROM PRODUITS WHERE id_produit = :id");
    query.bindValue(":id", productId);

    if (query.exec() && query.next()) {
        txtNom->setText(query.value(0).toString());
        txtDescription->setText(query.value(1).toString());
        selectedImagePath = query.value(2).toString();
        txtPrixVente->setText(QString::number(query.value(3).toDouble(), 'f', 2));
        txtPrixAchat->setText(query.value(4).isNull() ? "" : QString::number(query.value(4).toDouble(), 'f', 2));
        txtStock->setText(QString::number(query.value(5).toInt()));
        txtSeuilAlerte->setText(QString::number(query.value(6).toInt()));

        updateImagePreview();
    }
}

void ProductDialog::updateImagePreview()
{
    if (!selectedImagePath.isEmpty() && QFile::exists(selectedImagePath)) {
        QPixmap pixmap(selectedImagePath);
        lblImagePreview->setPixmap(pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        lblImagePreview->setStyleSheet("border: 2px solid #10b981; border-radius: 8px;");
    } else {
        lblImagePreview->setPixmap(QPixmap());
        lblImagePreview->setText("📷\nAucune image");
        lblImagePreview->setStyleSheet(
            "border: 2px dashed #334155; "
            "border-radius: 8px; "
            "background: #1e293b; "
            "color: #94a3b8;"
        );
    }
}

bool ProductDialog::validateInput()
{
    if (txtNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le nom du produit est requis.");
        txtNom->setFocus();
        return false;
    }

    if (txtPrixVente->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le prix de vente est requis.");
        txtPrixVente->setFocus();
        return false;
    }

    if (txtStock->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le stock est requis.");
        txtStock->setFocus();
        return false;
    }

    bool ok;
    double prixVente = txtPrixVente->text().toDouble(&ok);
    if (!ok || prixVente <= 0) {
        QMessageBox::warning(this, "Validation", "Le prix de vente doit être un nombre positif.");
        txtPrixVente->setFocus();
        return false;
    }

    if (!txtPrixAchat->text().trimmed().isEmpty()) {
        double prixAchat = txtPrixAchat->text().toDouble(&ok);
        if (!ok || prixAchat < 0) {
            QMessageBox::warning(this, "Validation", "Le prix d'achat doit être un nombre positif ou vide.");
            txtPrixAchat->setFocus();
            return false;
        }
    }

    int stock = txtStock->text().toInt(&ok);
    if (!ok || stock < 0) {
        QMessageBox::warning(this, "Validation", "Le stock doit être un nombre entier positif.");
        txtStock->setFocus();
        return false;
    }

    return true;
}

void ProductDialog::onSelectImage()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Sélectionner une image",
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)"
    );

    if (!fileName.isEmpty()) {
        selectedImagePath = fileName;
        updateImagePreview();
    }
}

void ProductDialog::onSave()
{
    if (!validateInput()) {
        return;
    }

    QSqlQuery query;

    if (currentProductId == -1) {
        // Insertion
        query.prepare("INSERT INTO PRODUITS (nom_produit, description, photo_produit, prix_vente, prix_achat, stock, seuil_alerte) "
                     "VALUES (:nom, :description, :photo, :prix_vente, :prix_achat, :stock, :seuil)");
        query.bindValue(":nom", txtNom->text().trimmed());
        query.bindValue(":description", txtDescription->toPlainText().trimmed());
        query.bindValue(":photo", selectedImagePath);
        query.bindValue(":prix_vente", txtPrixVente->text().toDouble());
        query.bindValue(":prix_achat", txtPrixAchat->text().trimmed().isEmpty() ? QVariant() : txtPrixAchat->text().toDouble());
        query.bindValue(":stock", txtStock->text().toInt());
        query.bindValue(":seuil", txtSeuilAlerte->text().trimmed().isEmpty() ? 5 : txtSeuilAlerte->text().toInt());
    } else {
        // Mise à jour
        query.prepare("UPDATE PRODUITS SET nom_produit = :nom, description = :description, photo_produit = :photo, "
                     "prix_vente = :prix_vente, prix_achat = :prix_achat, stock = :stock, seuil_alerte = :seuil WHERE id_produit = :id");
        query.bindValue(":id", currentProductId);
        query.bindValue(":nom", txtNom->text().trimmed());
        query.bindValue(":description", txtDescription->toPlainText().trimmed());
        query.bindValue(":photo", selectedImagePath);
        query.bindValue(":prix_vente", txtPrixVente->text().toDouble());
        query.bindValue(":prix_achat", txtPrixAchat->text().trimmed().isEmpty() ? QVariant() : txtPrixAchat->text().toDouble());
        query.bindValue(":stock", txtStock->text().toInt());
        query.bindValue(":seuil", txtSeuilAlerte->text().trimmed().isEmpty() ? 5 : txtSeuilAlerte->text().toInt());
    }

    if (query.exec()) {
        QMessageBox::information(this,
            "Succès",
            currentProductId == -1 ? "Produit ajouté avec succès!" : "Produit modifié avec succès!"
        );
        accept();
    } else {
        QMessageBox::critical(this, "Erreur",
            "Erreur lors de l'enregistrement: " + query.lastError().text()
        );
    }
}

void ProductDialog::onCancel()
{
    reject();
}