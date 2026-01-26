#include "orderdetailsdialog.h"
#include "thememanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDateTime>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QStandardPaths>

OrderDetailsDialog::OrderDetailsDialog(int orderId, QWidget *parent)
    : QDialog(parent), orderId(orderId)
{
    setWindowTitle("Détails de la Commande");
    setMinimumWidth(900);
    setMinimumHeight(700);
    setModal(true);
    
    setupStyles();
    setupUI();
    loadOrderDetails();
}

OrderDetailsDialog::~OrderDetailsDialog()
{
}

void OrderDetailsDialog::setupStyles()
{
    ThemeManager& theme = ThemeManager::instance();
    
    QString dialogStyle = QString(
        "QDialog {"
        "   background: #0f172a;"
        "}"
        "QLabel {"
        "   color: #e2e8f0;"
        "}"
        "QGroupBox {"
        "   color: #e2e8f0;"
        "   border: 1px solid #334155;"
        "   border-radius: 8px;"
        "   margin-top: 12px;"
        "   padding-top: 12px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 10px;"
        "   padding: 0 3px 0 3px;"
        "}"
    );
    
    setStyleSheet(dialogStyle);
}

void OrderDetailsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Header
    headerLabel = new QLabel("Détails Complets de la Commande");
    headerLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: 700;"
        "color: #06b6d4;"
    );
    mainLayout->addWidget(headerLabel);

    // Section 1: Commande Info
    QGroupBox *orderInfoGroup = new QGroupBox("Information de la Commande", this);
    QGridLayout *orderInfoLayout = new QGridLayout(orderInfoGroup);
    orderInfoLayout->setSpacing(15);

    orderIdLabel = new QLabel("ID: --");
    orderIdLabel->setStyleSheet("font-weight: 600; font-size: 13px;");
    
    dateLabel = new QLabel("Date: --");
    dateLabel->setStyleSheet("font-size: 13px;");
    
    statusLabel = new QLabel("Statut: --");
    statusLabel->setStyleSheet("font-size: 13px;");

    orderInfoLayout->addWidget(orderIdLabel, 0, 0);
    orderInfoLayout->addWidget(dateLabel, 0, 1);
    orderInfoLayout->addWidget(statusLabel, 0, 2);
    mainLayout->addWidget(orderInfoGroup);

    // Section 2: Client Info
    QGroupBox *clientGroup = new QGroupBox("Informations Client", this);
    QGridLayout *clientLayout = new QGridLayout(clientGroup);
    clientLayout->setSpacing(12);

    clientNameLabel = new QLabel("Nom: --");
    clientNameLabel->setStyleSheet("font-weight: 600; font-size: 13px;");
    
    clientPhoneLabel = new QLabel("Téléphone: --");
    clientPhoneLabel->setStyleSheet("font-size: 13px;");
    
    clientEmailLabel = new QLabel("Email: --");
    clientEmailLabel->setStyleSheet("font-size: 13px;");
    
    clientAddressLabel = new QLabel("Adresse: --");
    clientAddressLabel->setStyleSheet("font-size: 13px;");
    clientAddressLabel->setWordWrap(true);

    clientLayout->addWidget(clientNameLabel, 0, 0, 1, 2);
    clientLayout->addWidget(clientPhoneLabel, 1, 0);
    clientLayout->addWidget(clientEmailLabel, 1, 1);
    clientLayout->addWidget(clientAddressLabel, 2, 0, 1, 2);
    mainLayout->addWidget(clientGroup);

    // Section 3: Produits
    QGroupBox *productsGroup = new QGroupBox("Produits Commandés", this);
    QVBoxLayout *productsLayout = new QVBoxLayout(productsGroup);

    productsTable = new QTableWidget(this);
    productsTable->setColumnCount(4);
    productsTable->setHorizontalHeaderLabels({"Produit", "Quantité", "Prix Unit.", "Total"});
    productsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    productsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    productsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    productsTable->setShowGrid(false);
    productsTable->verticalHeader()->setVisible(false);
    productsTable->verticalHeader()->setDefaultSectionSize(40);
    
    productsTable->setStyleSheet(
        "QTableWidget {"
        "   background: #1e293b;"
        "   color: #f1f5f9;"
        "   gridline-color: #334155;"
        "   border: 1px solid #334155;"
        "   border-radius: 6px;"
        "}"
        "QTableWidget::item {"
        "   color: #f1f5f9;"
        "   padding: 8px;"
        "   border: none;"
        "}"
        "QHeaderView::section {"
        "   background: #0f172a;"
        "   color: #e2e8f0;"
        "   padding: 8px;"
        "   border: none;"
        "   border-right: 1px solid #334155;"
        "   font-weight: bold;"
        "}"
    );

    productsTable->setColumnWidth(0, 400);
    productsTable->setColumnWidth(1, 80);
    productsTable->setColumnWidth(2, 100);
    productsTable->setColumnWidth(3, 100);

    productsLayout->addWidget(productsTable);
    mainLayout->addWidget(productsGroup);

    // Section 4: Résumé Financier
    QGroupBox *financialGroup = new QGroupBox("Résumé Financier", this);
    QGridLayout *financialLayout = new QGridLayout(financialGroup);
    financialLayout->setSpacing(15);

    totalAmountLabel = new QLabel("Montant Total: -- Ar");
    totalAmountLabel->setStyleSheet("font-weight: 600; font-size: 13px; color: #06b6d4;");
    
    paidAmountLabel = new QLabel("Montant Payé: -- Ar");
    paidAmountLabel->setStyleSheet("font-weight: 600; font-size: 13px; color: #10b981;");
    
    remainingAmountLabel = new QLabel("Reste à Payer: -- Ar");
    remainingAmountLabel->setStyleSheet("font-weight: 600; font-size: 13px; color: #f59e0b;");
    
    paymentStatusLabel = new QLabel("Statut Paiement: --");
    paymentStatusLabel->setStyleSheet("font-weight: 600; font-size: 13px;");

    financialLayout->addWidget(totalAmountLabel, 0, 0);
    financialLayout->addWidget(paidAmountLabel, 0, 1);
    financialLayout->addWidget(remainingAmountLabel, 1, 0);
    financialLayout->addWidget(paymentStatusLabel, 1, 1);
    mainLayout->addWidget(financialGroup);

    // Boutons d'action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    generatePdfBtn = new QPushButton("📄 Générer PDF", this);
    generatePdfBtn->setMinimumHeight(40);
    generatePdfBtn->setMinimumWidth(150);
    generatePdfBtn->setCursor(Qt::PointingHandCursor);
    generatePdfBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #10b981, stop:1 #059669);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   font-weight: 600;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #059669, stop:1 #047857);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #047857, stop:1 #065f46);"
        "}"
    );
    connect(generatePdfBtn, &QPushButton::clicked, this, &OrderDetailsDialog::onGeneratePdf);

    notifyBtn = new QPushButton("🔔 Notifier Client", this);
    notifyBtn->setMinimumHeight(40);
    notifyBtn->setMinimumWidth(150);
    notifyBtn->setCursor(Qt::PointingHandCursor);
    notifyBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #f59e0b, stop:1 #d97706);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   font-weight: 600;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #d97706, stop:1 #b45309);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #b45309, stop:1 #92400e);"
        "}"
    );
    connect(notifyBtn, &QPushButton::clicked, this, &OrderDetailsDialog::onNotifyClient);

    closeBtn = new QPushButton("Fermer", this);
    closeBtn->setMinimumHeight(40);
    closeBtn->setMinimumWidth(150);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "   background: #334155;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   font-weight: 600;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: #475569;"
        "}"
        "QPushButton:pressed {"
        "   background: #1e293b;"
        "}"
    );
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    buttonLayout->addWidget(generatePdfBtn);
    buttonLayout->addWidget(notifyBtn);
    buttonLayout->addWidget(closeBtn);
    mainLayout->addLayout(buttonLayout);
}

void OrderDetailsDialog::loadOrderDetails()
{
    // Récupérer les données de la commande
    ReceiptGenerator generator;
    orderData = generator.getReceiptData(orderId);

    // Afficher les infos de la commande
    orderIdLabel->setText(QString("ID: #%1").arg(orderData.order.id));
    dateLabel->setText(QString("Date: %1").arg(orderData.order.date.toString("dd/MM/yyyy HH:mm")));
    statusLabel->setText(QString("Statut: %1").arg(orderData.order.statut == "EN_COURS" ? "En cours" : "Payée"));

    // Afficher les infos du client
    clientNameLabel->setText(QString("Nom: %1 %2")
        .arg(orderData.client.nom)
        .arg(orderData.client.prenom));
    clientPhoneLabel->setText(QString("Téléphone: %1")
        .arg(orderData.client.telephone.isEmpty() ? "N/A" : orderData.client.telephone));
    clientEmailLabel->setText(QString("Email: %1")
        .arg(orderData.client.email.isEmpty() ? "N/A" : orderData.client.email));
    clientAddressLabel->setText(QString("Adresse: %1")
        .arg(orderData.client.adresse.isEmpty() ? "N/A" : orderData.client.adresse));

    // Remplir la table des produits
    productsTable->setRowCount(orderData.items.size());
    for (int i = 0; i < orderData.items.size(); ++i) {
        const OrderItem& item = orderData.items[i];

        QTableWidgetItem *productNameItem = new QTableWidgetItem(item.productName);
        QTableWidgetItem *quantityItem = new QTableWidgetItem(QString::number(item.quantity));
        QTableWidgetItem *priceItem = new QTableWidgetItem(QString("%1 Ar").arg(QString::number(item.unitPrice, 'f', 2)));
        QTableWidgetItem *totalItem = new QTableWidgetItem(QString("%1 Ar").arg(QString::number(item.total, 'f', 2)));

        quantityItem->setTextAlignment(Qt::AlignCenter);
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        productsTable->setItem(i, 0, productNameItem);
        productsTable->setItem(i, 1, quantityItem);
        productsTable->setItem(i, 2, priceItem);
        productsTable->setItem(i, 3, totalItem);
    }

    // Afficher le résumé financier
    double remainingAmount = orderData.order.total - orderData.payment.montant;
    
    totalAmountLabel->setText(QString("Montant Total: %1 Ar")
        .arg(QString::number(orderData.order.total, 'f', 2)));
    paidAmountLabel->setText(QString("Montant Payé: %1 Ar")
        .arg(QString::number(orderData.payment.montant, 'f', 2)));
    remainingAmountLabel->setText(QString("Reste à Payer: %1 Ar")
        .arg(QString::number(remainingAmount, 'f', 2)));
    paymentStatusLabel->setText(QString("Statut Paiement: %1")
        .arg(orderData.payment.statut));
}

void OrderDetailsDialog::onGeneratePdf()
{
    ReceiptGenerator generator;
    if (generator.generatePDF(orderData)) {
        QMessageBox::information(this, "Succès", "PDF généré avec succès!");
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la génération du PDF");
    }
}

void OrderDetailsDialog::onNotifyClient()
{
    // Vérifier si l'email est valide
    if (orderData.client.email.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "L'email du client n'est pas disponible!");
        return;
    }

    // Générer le PDF d'abord
    ReceiptGenerator generator;
    QString pdfPath = QString("%1/Recu_Commande_%2_%3.pdf")
        .arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
        .arg(orderData.order.id)
        .arg(QDateTime::currentDateTime().toString("ddMMyyyy_hhmmss"));

    // Générer le PDF
    if (!generator.generatePDF(orderData, pdfPath)) {
        QMessageBox::critical(this, "Erreur", "Impossible de générer le PDF!");
        return;
    }

    // Envoyer l'email avec le PDF
    if (generator.sendReceiptByEmail(orderData.client.email, pdfPath, orderData)) {
        QMessageBox::information(this, "Succès", 
            QString("Email de notification envoyé avec succès à %1!")
            .arg(orderData.client.email));
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible d'envoyer l'email!");
    }
}

