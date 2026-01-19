#include "paymentspage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include "thememanager.h"

PaymentsPage::PaymentsPage(QWidget *parent) : QFrame(parent)
{
    setObjectName("paymentsPage");
    setupUI();
    loadPayments();
}

void PaymentsPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(28);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    ThemeManager& theme = ThemeManager::instance();

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *icon = new QLabel(this);
    icon->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #06b6d4, stop:1 #0891b2);"
        "border-radius: 14px;"
        "min-width: 52px; max-width: 52px;"
        "min-height: 52px; max-height: 52px;"
    );

    QLabel *title = new QLabel("Gestion des Paiements", this);
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

    // Filtres et recherche
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
    
    connect(searchInput, &QLineEdit::textChanged, this, &PaymentsPage::onSearchTextChanged);
    filterLayout->addWidget(searchInput, 2);

    statusFilter = new QComboBox(this);
    statusFilter->setMinimumHeight(48);
    statusFilter->addItem("Tous les statuts", "");
    statusFilter->addItem("Validé", "VALIDE");
    statusFilter->addItem("Annulé", "ANNULE");
    statusFilter->setStyleSheet(QString(
        "QComboBox {"
        "   border: 1px solid %1;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 15px;"
        "   background: %2;"
        "   color: %3;"
        "}"
        "QComboBox:focus {"
        "   border: 2px solid %4;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
    ).arg(theme.borderColor().name(),
          theme.inputBackground().name(),
          theme.textColor().name(),
          theme.primaryColor().name()));
    
    connect(statusFilter, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &PaymentsPage::onStatusFilterChanged);
    filterLayout->addWidget(statusFilter, 1);

    refreshBtn = new QPushButton("🔄 Actualiser", this);
    refreshBtn->setMinimumHeight(48);
    refreshBtn->setMinimumWidth(140);
    refreshBtn->setStyleSheet(QString(
        "QPushButton {"
        "   background: %1;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 12px 24px;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: %2;"
        "}"
        "QPushButton:pressed {"
        "   background: %3;"
        "}"
    ).arg(theme.primaryColor().name(),
          theme.primaryHoverColor().name(),
          theme.primaryPressedColor().name()));
    
    connect(refreshBtn, &QPushButton::clicked, this, &PaymentsPage::loadPayments);
    filterLayout->addWidget(refreshBtn);

    mainLayout->addLayout(filterLayout);

    // Table des paiements
    paymentsTable = new QTableWidget(this);
    paymentsTable->setColumnCount(6);
    
    QStringList headers;
    headers << "N° Paiement" << "N° Commande" << "Montant" << "Date" << "Statut" << "Client";
    paymentsTable->setHorizontalHeaderLabels(headers);
    paymentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    paymentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    paymentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    paymentsTable->setShowGrid(false);
    paymentsTable->verticalHeader()->setVisible(false);
    paymentsTable->verticalHeader()->setDefaultSectionSize(50);
    paymentsTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    // Style du tableau identique à orderspage
    paymentsTable->setStyleSheet(
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

    // Ajuster les largeurs des colonnes
    paymentsTable->setColumnWidth(0, 120);
    paymentsTable->setColumnWidth(1, 120);
    paymentsTable->setColumnWidth(2, 120);
    paymentsTable->setColumnWidth(3, 180);
    paymentsTable->setColumnWidth(4, 120);
    paymentsTable->setColumnWidth(5, 200);

    mainLayout->addWidget(paymentsTable);

    mainLayout->addStretch();
}

void PaymentsPage::loadPayments()
{
    paymentsTable->setRowCount(0);

    QSqlQuery query;
    query.prepare("SELECT p.id_paiement, p.id_commande, p.montant, p.date_paiement, p.statut, c.nom "
                  "FROM PAIEMENTS p "
                  "LEFT JOIN COMMANDES cmd ON p.id_commande = cmd.id_commande "
                  "LEFT JOIN CLIENTS c ON cmd.id_client = c.id_client "
                  "ORDER BY p.date_paiement DESC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des paiements: " + query.lastError().text());
        return;
    }

    int row = 0;
    while (query.next()) {
        paymentsTable->insertRow(row);

        // N° Paiement
        paymentsTable->setItem(row, 0, new QTableWidgetItem(QString::number(query.value("id_paiement").toInt())));

        // N° Commande
        paymentsTable->setItem(row, 1, new QTableWidgetItem(QString::number(query.value("id_commande").toInt())));

        // Montant
        double montant = query.value("montant").toDouble();
        paymentsTable->setItem(row, 2, new QTableWidgetItem(QString::number(montant, 'f', 2) + " €"));

        // Date
        QDateTime date = query.value("date_paiement").toDateTime();
        paymentsTable->setItem(row, 3, new QTableWidgetItem(date.toString("dd/MM/yyyy hh:mm")));

        // Statut
        QString statut = query.value("statut").toString();
        paymentsTable->setItem(row, 4, new QTableWidgetItem(statut));

        // Client
        paymentsTable->setItem(row, 5, new QTableWidgetItem(query.value("nom").toString()));

        row++;
    }
}

void PaymentsPage::onSearchTextChanged(const QString &text)
{
    loadPayments();
}

void PaymentsPage::onStatusFilterChanged(const QString &status)
{
    loadPayments();
}