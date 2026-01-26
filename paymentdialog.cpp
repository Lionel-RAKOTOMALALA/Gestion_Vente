#include "paymentdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

PaymentDialog::PaymentDialog(double totalAmount, QWidget *parent)
    : QDialog(parent), totalAmount(totalAmount)
{
    setupUI();
    if (totalAmount > 0) {
        amountSpinBox->setValue(totalAmount);
    }
}

void PaymentDialog::setupUI()
{
    setWindowTitle("Formulaire de Paiement");
    setMinimumWidth(550);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Titre avec icône
    QLabel *title = new QLabel("💳 Formulaire de Paiement", this);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1e293b;");
    mainLayout->addWidget(title);

    // Montant total
    QLabel *totalDescLabel = new QLabel("Montant de la commande:", this);
    totalDescLabel->setStyleSheet("color: #64748b; font-weight: 600;");
    
    totalLabel = new QLabel(QString::number(totalAmount, 'f', 2) + " Ar", this);
    totalLabel->setStyleSheet(
        "background: linear-gradient(135deg, #10b981 0%, #059669 100%);"
        "color: white;"
        "padding: 15px;"
        "border-radius: 8px;"
        "font-size: 20px;"
        "font-weight: bold;"
        "text-align: center;"
    );
    
    mainLayout->addWidget(totalDescLabel);
    mainLayout->addWidget(totalLabel);
    mainLayout->addSpacing(10);

    // Formulaire
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Mode de paiement
    paymentMethodCombo = new QComboBox(this);
    paymentMethodCombo->addItem("💵 Espèces");
    paymentMethodCombo->addItem("💳 Carte Bancaire");
    paymentMethodCombo->addItem("📱 Mobile Payment");
    paymentMethodCombo->addItem("🏦 Chèque");
    paymentMethodCombo->setMinimumHeight(40);
    paymentMethodCombo->setStyleSheet(
        "QComboBox {"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "   font-size: 14px;"
        "}"
        "QComboBox:focus {"
        "   border: 2px solid #10b981;"
        "}"
    );
    formLayout->addRow("Mode de paiement:", paymentMethodCombo);

    // Montant à payer
    amountSpinBox = new QDoubleSpinBox(this);
    amountSpinBox->setMinimum(0);
    amountSpinBox->setMaximum(999999.99);
    amountSpinBox->setDecimals(2);
    amountSpinBox->setSingleStep(0.01);
    amountSpinBox->setValue(totalAmount);
    amountSpinBox->setMinimumHeight(40);
    amountSpinBox->setStyleSheet(
        "QDoubleSpinBox {"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "   font-size: 14px;"
        "}"
        "QDoubleSpinBox:focus {"
        "   border: 2px solid #10b981;"
        "}"
        "QAbstractSpinBox::up-button, QAbstractSpinBox::down-button {"
        "   background-color: #f1f5f9;"
        "   border: none;"
        "}"
    );
    formLayout->addRow("Montant à payer:", amountSpinBox);

    mainLayout->addLayout(formLayout);

    // Bouton utiliser le total
    useFullAmountBtn = new QPushButton("↔️ Utiliser le montant total", this);
    useFullAmountBtn->setMinimumHeight(38);
    useFullAmountBtn->setStyleSheet(
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
    connect(useFullAmountBtn, &QPushButton::clicked, this, &PaymentDialog::onUseTotal);
    mainLayout->addWidget(useFullAmountBtn);

    // Boutons action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    confirmBtn = new QPushButton("✓ Confirmer le paiement", this);
    confirmBtn->setMinimumHeight(45);
    confirmBtn->setStyleSheet(
        "QPushButton {"
        "   background: linear-gradient(135deg, #10b981 0%, #059669 100%);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background: linear-gradient(135deg, #059669 0%, #047857 100%);"
        "}"
        "QPushButton:pressed {"
        "   background: linear-gradient(135deg, #047857 0%, #065f46 100%);"
        "}"
    );
    connect(confirmBtn, &QPushButton::clicked, this, &PaymentDialog::onConfirm);

    cancelBtn = new QPushButton("✕ Annuler", this);
    cancelBtn->setMinimumHeight(45);
    cancelBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #ef4444;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #dc2626;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #b91c1c;"
        "}"
    );
    connect(cancelBtn, &QPushButton::clicked, this, &PaymentDialog::onCancel);

    buttonLayout->addWidget(confirmBtn, 1);
    buttonLayout->addWidget(cancelBtn, 1);
    mainLayout->addLayout(buttonLayout);

    mainLayout->addStretch();
}

void PaymentDialog::onConfirm()
{
    if (amountSpinBox->value() <= 0) {
        QMessageBox::warning(this, "Erreur", "Le montant doit être supérieur à 0 Ar");
        return;
    }
    
    if (amountSpinBox->value() < totalAmount) {
        int ret = QMessageBox::question(this, "Montant insuffisant",
            QString("Le montant saisi (%1 Ar) est inférieur au total (%2 Ar).\n\nVoulez-vous continuer ?")
                .arg(amountSpinBox->value(), 0, 'f', 2)
                .arg(totalAmount, 0, 'f', 2),
            QMessageBox::Yes | QMessageBox::No);
        
        if (ret != QMessageBox::Yes) {
            return;
        }
    }
    
    accept();
}

void PaymentDialog::onCancel()
{
    reject();
}

void PaymentDialog::onUseTotal()
{
    amountSpinBox->setValue(totalAmount);
}

double PaymentDialog::getAmount() const
{
    return amountSpinBox->value();
}

QString PaymentDialog::getPaymentMethod() const
{
    QString method = paymentMethodCombo->currentText();
    // Enlever l'emoji pour garder seulement le texte
    return method.mid(method.indexOf(" ") + 1);
}
