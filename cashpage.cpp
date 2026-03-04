#include "cashpage.h"
#include "thememanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include <QScrollArea>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

CashPage::CashPage(QWidget *parent) : QFrame(parent)
{
    setObjectName("cashPage");
    setupUI();
    loadStatistics();
    loadTransactions();
}

void CashPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(28);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    ThemeManager& theme = ThemeManager::instance();

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *icon = new QLabel(this);
    icon->setStyleSheet(
        "background: #f59e0b;"
        "border-radius: 14px;"
        "min-width: 52px; max-width: 52px;"
        "min-height: 52px; max-height: 52px;"
    );

    QLabel *title = new QLabel("💰 Gestion de la Caisse", this);
    title->setObjectName("titleH1");
    title->setStyleSheet(QString(
        "font-size: 32px;"
        "font-weight: 700;"
        "color: %1;"
        "letter-spacing: -0.5px;").arg(theme.textColor().name())
    );

    headerLayout->addWidget(icon);
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    // Statistics Cards
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);

    // Card 1: Total Cashed
    QFrame *card1 = new QFrame(this);
    card1->setObjectName("statCard");
    card1->setStyleSheet(QString(
        "QFrame#statCard {"
        "   background: #10b981;"
        "   border-radius: 14px;"
        "   padding: 24px;"
        "}"
    ));
    QVBoxLayout *card1Layout = new QVBoxLayout(card1);
    QLabel *label1 = new QLabel("Total Encaissé (Aujourd'hui)", this);
    label1->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 13px;");
    totalCashedLabel = new QLabel("0,00 Ar", this);
    totalCashedLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    card1Layout->addWidget(label1);
    card1Layout->addWidget(totalCashedLabel);
    statsLayout->addWidget(card1);

    // Card 2: Transaction Count
    QFrame *card2 = new QFrame(this);
    card2->setObjectName("statCard");
    card2->setStyleSheet(QString(
        "QFrame#statCard {"
        "   background: #3b82f6;"
        "   border-radius: 14px;"
        "   padding: 24px;"
        "}"
    ));
    QVBoxLayout *card2Layout = new QVBoxLayout(card2);
    QLabel *label2 = new QLabel("Nombre de Transactions", this);
    label2->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 13px;");
    transactionCountLabel = new QLabel("0", this);
    transactionCountLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    card2Layout->addWidget(label2);
    card2Layout->addWidget(transactionCountLabel);
    statsLayout->addWidget(card2);

    // Card 3: Average Amount
    QFrame *card3 = new QFrame(this);
    card3->setObjectName("statCard");
    card3->setStyleSheet(QString(
        "QFrame#statCard {"
        "   background: #8b5cf6;"
        "   border-radius: 14px;"
        "   padding: 24px;"
        "}"
    ));
    QVBoxLayout *card3Layout = new QVBoxLayout(card3);
    QLabel *label3 = new QLabel("Montant Moyen", this);
    label3->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 13px;");
    averageAmountLabel = new QLabel("0,00 Ar", this);
    averageAmountLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    card3Layout->addWidget(label3);
    card3Layout->addWidget(averageAmountLabel);
    statsLayout->addWidget(card3);

    mainLayout->addLayout(statsLayout);

    // Filters and Search
    QHBoxLayout *filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(16);

    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Rechercher par commande, client...");
    searchInput->setMinimumHeight(48);
    searchInput->setStyleSheet(QString(
        "QLineEdit {"
        "   border: 1px solid %1;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 15px;"
        "   background: %2;"
        "   color: %3;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid %4;"
        "   outline: none;"
        "   background: %5;"
        "}"
        "QLineEdit::placeholder {"
        "   color: %6;"
        "}"
    ).arg(theme.borderColor().name(),
          theme.inputBackground().name(),
          theme.textColor().name(),
          theme.primaryColor().name(),
          theme.surfaceAltColor().name(),
          theme.textTertiaryColor().name()));
    
    connect(searchInput, &QLineEdit::textChanged, this, &CashPage::onSearchTextChanged);
    filterLayout->addWidget(searchInput, 2);

    dateFilter = new QDateEdit(this);
    dateFilter->setDate(QDate::currentDate());
    dateFilter->setMinimumHeight(48);
    dateFilter->setCalendarPopup(true);
    dateFilter->setStyleSheet(QString(
        "QDateEdit {"
        "   border: 1px solid %1;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 15px;"
        "   background: %2;"
        "   color: %3;"
        "}"
        "QDateEdit:focus {"
        "   border: 2px solid %4;"
        "   outline: none;"
        "}"
    ).arg(theme.borderColor().name(),
          theme.inputBackground().name(),
          theme.textColor().name(),
          theme.primaryColor().name()));
    
    connect(dateFilter, &QDateEdit::dateChanged, this, &CashPage::onDateChanged);
    filterLayout->addWidget(dateFilter, 1);

    refreshBtn = new QPushButton("🔄 Actualiser", this);
    refreshBtn->setMinimumHeight(48);
    refreshBtn->setMinimumWidth(140);
    refreshBtn->setStyleSheet(QString(
        "QPushButton {"
        "   border: 1px solid %1;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 15px;"
        "   background: %2;"
        "   color: %3;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: %4;"
        "   border: 1px solid %5;"
        "}"
    ).arg(theme.borderColor().name(),
          theme.surfaceColor().name(),
          theme.textColor().name(),
          theme.primaryColor().name(),
          theme.primaryColor().name()));
    connect(refreshBtn, &QPushButton::clicked, this, &CashPage::onRefresh);
    filterLayout->addWidget(refreshBtn);

    exportBtn = new QPushButton("📥 Exporter", this);
    exportBtn->setMinimumHeight(48);
    exportBtn->setMinimumWidth(140);
    exportBtn->setStyleSheet(QString(
        "QPushButton {"
        "   background: %1;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: %2;"
        "}"
    ).arg(theme.primaryColor().name(),
          QColor(theme.primaryColor().darker(120)).name()));
    connect(exportBtn, &QPushButton::clicked, this, &CashPage::onExport);
    filterLayout->addWidget(exportBtn);

    mainLayout->addLayout(filterLayout);

    // Transactions Table
    transactionsTable = new QTableWidget(this);
    transactionsTable->setColumnCount(7);
    transactionsTable->setHorizontalHeaderLabels({"Commande", "Client", "Montant", "Date", "Heure", "Statut", "Actions"});
    transactionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    transactionsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    transactionsTable->setAlternatingRowColors(true);
    transactionsTable->setColumnWidth(0, 100);
    transactionsTable->setColumnWidth(1, 200);
    transactionsTable->setColumnWidth(2, 120);
    transactionsTable->setColumnWidth(3, 120);
    transactionsTable->setColumnWidth(4, 100);
    transactionsTable->setColumnWidth(5, 120);
    transactionsTable->setColumnWidth(6, 120);
    transactionsTable->horizontalHeader()->setStretchLastSection(true);
    transactionsTable->setStyleSheet(QString(
        "QTableWidget {"
        "   background: %1;"
        "   alternate-background-color: %2;"
        "   gridline-color: %3;"
        "   border: 1px solid %3;"
        "   border-radius: 12px;"
        "}"
        "QTableWidget::item {"
        "   padding: 12px;"
        "   color: %4;"
        "}"
        "QHeaderView::section {"
        "   background: %5;"
        "   color: %4;"
        "   padding: 12px;"
        "   border: none;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
    ).arg(theme.backgroundColor().name(),
          theme.surfaceAltColor().name(),
          theme.borderColor().name(),
          theme.textColor().name(),
          theme.surfaceColor().name()));
    
    mainLayout->addWidget(transactionsTable, 1);
}

void CashPage::loadStatistics()
{
    QSqlQuery query;
    QDate today = QDate::currentDate();
    QString dateStr = today.toString("yyyy-MM-dd");
    
    // Get total cashed today
    query.prepare("SELECT SUM(montant) FROM PAIEMENTS WHERE DATE(date_paiement) = ? AND statut = 'VALIDE'");
    query.addBindValue(dateStr);
    
    double total = 0;
    if (query.exec()) {
        if (query.next() && !query.value(0).isNull()) {
            total = query.value(0).toDouble();
        }
    }
    
    totalCashedLabel->setText(QString("%1 Ar").arg(total, 0, 'f', 2));

    // Get transaction count
    query.prepare("SELECT COUNT(*) FROM PAIEMENTS WHERE DATE(date_paiement) = ? AND statut = 'VALIDE'");
    query.addBindValue(dateStr);
    
    int count = 0;
    if (query.exec()) {
        if (query.next()) {
            count = query.value(0).toInt();
        }
    }
    
    transactionCountLabel->setText(QString::number(count));

    // Get average amount
    double average = count > 0 ? total / count : 0;
    averageAmountLabel->setText(QString("%1 Ar").arg(average, 0, 'f', 2));
}

void CashPage::loadTransactions()
{
    updateTransactions();
}

void CashPage::updateTransactions()
{
    ThemeManager& theme = ThemeManager::instance();
    
    QSqlQuery query;
    QDate filterDate = dateFilter->date();
    QString dateStr = filterDate.toString("yyyy-MM-dd");
    
    transactionsTable->setRowCount(0);
    
    // Get all payments for the selected date
    query.prepare(
        "SELECT p.id_paiement, c.id_commande, cl.nom, p.montant, p.date_paiement, p.statut "
        "FROM PAIEMENTS p "
        "JOIN COMMANDES c ON p.id_commande = c.id_commande "
        "JOIN CLIENTS cl ON c.id_client = cl.id_client "
        "WHERE DATE(p.date_paiement) = ? "
        "ORDER BY p.date_paiement DESC"
    );
    query.addBindValue(dateStr);
    
    if (!query.exec()) {
        qDebug() << "Erreur lors du chargement des transactions:" << query.lastError().text();
        return;
    }
    
    int row = 0;
    while (query.next()) {
        int paymentId = query.value(0).toInt();
        int orderId = query.value(1).toInt();
        QString clientName = query.value(2).toString();
        double amount = query.value(3).toDouble();
        QDateTime dateTime = query.value(4).toDateTime();
        QString status = query.value(5).toString();
        
        // Apply search filter
        QString searchText = searchInput->text();
        if (!searchText.isEmpty()) {
            if (!QString::number(orderId).contains(searchText, Qt::CaseInsensitive) &&
                !clientName.contains(searchText, Qt::CaseInsensitive)) {
                continue;
            }
        }
        
        transactionsTable->insertRow(row);
        
        // Order ID
        QTableWidgetItem *orderItem = new QTableWidgetItem(QString("#%1").arg(orderId));
        transactionsTable->setItem(row, 0, orderItem);
        
        // Client name
        QTableWidgetItem *clientItem = new QTableWidgetItem(clientName);
        transactionsTable->setItem(row, 1, clientItem);
        
        // Amount
        QTableWidgetItem *amountItem = new QTableWidgetItem(QString("%1 Ar").arg(amount, 0, 'f', 2));
        transactionsTable->setItem(row, 2, amountItem);
        
        // Date
        QTableWidgetItem *dateItem = new QTableWidgetItem(dateTime.date().toString("dd/MM/yyyy"));
        transactionsTable->setItem(row, 3, dateItem);
        
        // Time
        QTableWidgetItem *timeItem = new QTableWidgetItem(dateTime.time().toString("HH:mm:ss"));
        transactionsTable->setItem(row, 4, timeItem);
        
        // Status
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);
        if (status == "VALIDE") {
            statusItem->setForeground(QColor(16, 185, 129));
        } else {
            statusItem->setForeground(QColor(239, 68, 68));
        }
        transactionsTable->setItem(row, 5, statusItem);
        
        // Cancel button
        QPushButton *cancelBtn = new QPushButton("Annuler", this);
        cancelBtn->setEnabled(status == "VALIDE");
        cancelBtn->setProperty("paymentId", paymentId);
        cancelBtn->setStyleSheet(QString(
            "QPushButton {"
            "   background: %1;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 6px;"
            "   padding: 6px 12px;"
            "   font-size: 12px;"
            "   font-weight: 600;"
            "}"
            "QPushButton:hover:!disabled {"
            "   background: %2;"
            "}"
            "QPushButton:disabled {"
            "   background: %3;"
            "   color: %4;"
            "}"
        ).arg(QColor(239, 68, 68).name(),
              QColor(159, 18, 18).name(),
              theme.surfaceColor().name(),
              theme.textTertiaryColor().name()));
        connect(cancelBtn, &QPushButton::clicked, this, &CashPage::onCancelPayment);
        transactionsTable->setCellWidget(row, 6, cancelBtn);
        
        row++;
    }
}

void CashPage::onSearchTextChanged(const QString &text)
{
    updateTransactions();
}

void CashPage::onDateChanged()
{
    updateTransactions();
    loadStatistics();
}

void CashPage::onRefresh()
{
    loadStatistics();
    loadTransactions();
}

void CashPage::onCancelPayment()
{
    ThemeManager& theme = ThemeManager::instance();
    
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    int paymentId = btn->property("paymentId").toInt();
    
    int ret = QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir annuler ce paiement?");
    if (ret != QMessageBox::Yes) return;
    
    QSqlQuery query;
    query.prepare("UPDATE PAIEMENTS SET statut = 'ANNULE' WHERE id_paiement = ?");
    query.addBindValue(paymentId);
    
    if (query.exec()) {
        QMessageBox::information(this, "Succès", "Paiement annulé avec succès");
        updateTransactions();
        loadStatistics();
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'annulation du paiement: " + query.lastError().text());
    }
}

void CashPage::onExport()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter les transactions", 
        QString("%1/transactions_%2.csv").arg(QDir::homePath(), QDate::currentDate().toString("yyyy-MM-dd")),
        "CSV Files (*.csv)");
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier");
        return;
    }
    
    QTextStream out(&file);
    out << "Commande,Client,Montant,Date,Heure,Statut\n";
    
    for (int row = 0; row < transactionsTable->rowCount(); ++row) {
        for (int col = 0; col < 6; ++col) {
            QTableWidgetItem *item = transactionsTable->item(row, col);
            out << item->text() << ",";
        }
        out << "\n";
    }
    
    file.close();
    QMessageBox::information(this, "Succès", "Transactions exportées avec succès");
}