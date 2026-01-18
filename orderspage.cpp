#include "orderspage.h"
#include "orderdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include "thememanager.h"

OrdersPage::OrdersPage(const QString &userRole, int userId, QWidget *parent) : 
    QFrame(parent), 
    userRole(userRole), 
    userId(userId),
    currentPage(1),
    itemsPerPage(5),
    totalItems(0),
    totalPages(1)
{
    setObjectName("ordersPage");
    setupDatabase();
    setupUI();
    loadOrders();
}

void OrdersPage::setupDatabase()
{
}

void OrdersPage::setupUI()
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

    QLabel *title = new QLabel("Gestion des Commandes", this);
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
    searchInput->setPlaceholderText("Rechercher par client, commande...");
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
    
    connect(searchInput, &QLineEdit::textChanged, this, &OrdersPage::onSearchTextChanged);
    filterLayout->addWidget(searchInput, 2);

    statusFilter = new QComboBox(this);
    statusFilter->setMinimumHeight(48);
    statusFilter->addItem("Tous les statuts", "");
    statusFilter->addItem("En cours", "EN_COURS");
    statusFilter->addItem("Payée", "PAYEE");
    statusFilter->addItem("Annulée", "ANNULEE");
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
            this, &OrdersPage::onStatusFilterChanged);
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
    
    connect(refreshBtn, &QPushButton::clicked, this, &OrdersPage::onRefreshClicked);
    filterLayout->addWidget(refreshBtn);

    mainLayout->addLayout(filterLayout);

    // Table des commandes
    int columnCount = (userRole == "ADMIN") ? 8 : 7;
    ordersTable = new QTableWidget(this);
    ordersTable->setColumnCount(columnCount);
    
    QStringList headers;
    headers << "N° Commande" << "Date" << "Client" << "Vendeur" << "Statut" << "Total" << "Produits";
    if (userRole == "ADMIN") {
        headers << "Actions";
    }
    ordersTable->setHorizontalHeaderLabels(headers);
    ordersTable->horizontalHeader()->setStretchLastSection(true);
    ordersTable->setAlternatingRowColors(false);
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ordersTable->setShowGrid(false);
    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->verticalHeader()->setDefaultSectionSize(50);
    ordersTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    // Style du tableau identique à clientspage
    ordersTable->setStyleSheet(
        "QTableWidget {"
        "   background: #0f172a;"
        "   color: #e2e8f0;"
        "   gridline-color: #334155;"
        "}"
        "QTableWidget::item {"
        "   color: #f1f5f9;"
        "   padding: 8px;"
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
    ordersTable->setColumnWidth(0, 100);
    ordersTable->setColumnWidth(1, 150);
    ordersTable->setColumnWidth(2, 200);
    ordersTable->setColumnWidth(3, 150);
    ordersTable->setColumnWidth(4, 100);
    ordersTable->setColumnWidth(5, 100);
    if (userRole == "ADMIN") {
        ordersTable->setColumnWidth(6, 200);
        ordersTable->setColumnWidth(7, 150);
    }

    ordersTable->verticalHeader()->setDefaultSectionSize(60);

    connect(ordersTable, &QTableWidget::cellDoubleClicked, this, &OrdersPage::onViewOrderDetails);

    mainLayout->addWidget(ordersTable);

    // Pagination
    paginationWidget = new QWidget(this);
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setSpacing(12);
    
    QString btnStyle = 
        "QPushButton {"
        "   background: white;"
        "   color: #4a5568;"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   padding: 0px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "   border-color: transparent;"
        "}"
        "QPushButton:disabled {"
        "   background: #f7fafc;"
        "   color: #cbd5e0;"
        "   border-color: #e2e8f0;"
        "}";
    
    btnFirstPage = new QPushButton("|<", paginationWidget);
    btnFirstPage->setFixedSize(45, 45);
    btnFirstPage->setStyleSheet(btnStyle);
    connect(btnFirstPage, &QPushButton::clicked, this, &OrdersPage::onFirstPageClicked);
    
    btnPreviousPage = new QPushButton("<", paginationWidget);
    btnPreviousPage->setFixedSize(45, 45);
    btnPreviousPage->setStyleSheet(btnStyle);
    connect(btnPreviousPage, &QPushButton::clicked, this, &OrdersPage::onPreviousPageClicked);

    pageInfoLabel = new QLabel("Page 1 / 1", paginationWidget);
    pageInfoLabel->setStyleSheet(QString(
        "font-size: 14px;"
        "font-weight: 600;"
        "color: #4a5568;"
        "background: white;"
        "border: 2px solid #e2e8f0;"
        "border-radius: 10px;"
        "padding: 10px 20px;"
    ));
    pageInfoLabel->setAlignment(Qt::AlignCenter);
    pageInfoLabel->setMinimumWidth(150);

    btnNextPage = new QPushButton(">", paginationWidget);
    btnNextPage->setFixedSize(45, 45);
    btnNextPage->setStyleSheet(btnStyle);
    connect(btnNextPage, &QPushButton::clicked, this, &OrdersPage::onNextPageClicked);

    btnLastPage = new QPushButton(">|", paginationWidget);
    btnLastPage->setFixedSize(45, 45);
    btnLastPage->setStyleSheet(btnStyle);
    connect(btnLastPage, &QPushButton::clicked, this, &OrdersPage::onLastPageClicked);

    paginationLayout->addStretch();
    paginationLayout->addWidget(btnFirstPage);
    paginationLayout->addWidget(btnPreviousPage);
    paginationLayout->addWidget(pageInfoLabel);
    paginationLayout->addWidget(btnNextPage);
    paginationLayout->addWidget(btnLastPage);
    paginationLayout->addStretch();
    
    mainLayout->addWidget(paginationWidget);
}

void OrdersPage::loadOrders()
{
    ordersTable->setRowCount(0);

    QString countQueryStr = R"(
        SELECT COUNT(DISTINCT c.id_commande) as total
        FROM COMMANDES c
        LEFT JOIN CLIENTS cl ON c.id_client = cl.id_client
        LEFT JOIN USERS u ON c.id_user = u.id_user
        LEFT JOIN COMMANDE_DETAIL cd ON c.id_commande = cd.id_commande
        LEFT JOIN PRODUITS p ON cd.id_produit = p.id_produit
        WHERE 1=1
    )";

    QStringList conditions;

    if (!currentSearchText.isEmpty()) {
        conditions << "(cl.nom LIKE '%" + currentSearchText + "%' OR "
                   "cl.prenom LIKE '%" + currentSearchText + "%' OR "
                   "CAST(c.id_commande AS TEXT) LIKE '%" + currentSearchText + "%' OR "
                   "p.nom_produit LIKE '%" + currentSearchText + "%')";
    }

    if (!currentStatusFilter.isEmpty()) {
        conditions << "c.statut = '" + currentStatusFilter + "'";
    }

    if (!conditions.isEmpty()) {
        countQueryStr += " AND " + conditions.join(" AND ");
    }

    QSqlQuery countQuery;
    if (countQuery.exec(countQueryStr) && countQuery.next()) {
        totalItems = countQuery.value("total").toInt();
        totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;
        if (totalPages == 0) totalPages = 1;
    } else {
        totalItems = 0;
        totalPages = 1;
    }

    if (currentPage > totalPages) {
        currentPage = totalPages;
    }
    if (currentPage < 1) {
        currentPage = 1;
    }

    updatePaginationUI();

    QString queryStr = R"(
        SELECT
            c.id_commande,
            c.date_commande,
            cl.nom || ' ' || COALESCE(cl.prenom, '') as client_nom,
            u.nom as vendeur_nom,
            c.statut,
            c.total,
            GROUP_CONCAT(p.nom_produit, ', ') as produits
        FROM COMMANDES c
        LEFT JOIN CLIENTS cl ON c.id_client = cl.id_client
        LEFT JOIN USERS u ON c.id_user = u.id_user
        LEFT JOIN COMMANDE_DETAIL cd ON c.id_commande = cd.id_commande
        LEFT JOIN PRODUITS p ON cd.id_produit = p.id_produit
        WHERE 1=1
    )";

    if (!conditions.isEmpty()) {
        queryStr += " AND " + conditions.join(" AND ");
    }

    queryStr += " GROUP BY c.id_commande, c.date_commande, cl.nom, cl.prenom, u.nom, c.statut, c.total "
                "ORDER BY c.date_commande DESC "
                "LIMIT " + QString::number(itemsPerPage) + " OFFSET " + QString::number((currentPage - 1) * itemsPerPage);

    QSqlQuery query;
    if (!query.exec(queryStr)) {
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des commandes: " + query.lastError().text());
        return;
    }

    int row = 0;
    while (query.next()) {
        ordersTable->insertRow(row);

        ordersTable->setItem(row, 0, new QTableWidgetItem(QString::number(query.value("id_commande").toInt())));

        QString dateStr = query.value("date_commande").toString();
        QDateTime dateTime = QDateTime::fromString(dateStr, "yyyy-MM-ddTHH:mm:ss");
        if (!dateTime.isValid()) {
            dateTime = QDateTime::fromString(dateStr, "yyyy-MM-dd HH:mm:ss");
        }
        QString formattedDate = dateTime.isValid() ? dateTime.toString("dd/MM/yyyy HH:mm") : dateStr;
        ordersTable->setItem(row, 1, new QTableWidgetItem(formattedDate));

        ordersTable->setItem(row, 2, new QTableWidgetItem(query.value("client_nom").toString().trimmed()));
        ordersTable->setItem(row, 3, new QTableWidgetItem(query.value("vendeur_nom").toString()));

        QString statut = query.value("statut").toString();
        QTableWidgetItem *statusItem = new QTableWidgetItem(statut);

        if (statut == "EN_COURS") {
            statusItem->setBackground(QColor("#fef3c7"));
            statusItem->setForeground(QColor("#d97706"));
        } else if (statut == "PAYEE") {
            statusItem->setBackground(QColor("#d1fae5"));
            statusItem->setForeground(QColor("#059669"));
        } else if (statut == "ANNULEE") {
            statusItem->setBackground(QColor("#fee2e2"));
            statusItem->setForeground(QColor("#dc2626"));
        }

        ordersTable->setItem(row, 4, statusItem);

        double total = query.value("total").toDouble();
        ordersTable->setItem(row, 5, new QTableWidgetItem(QString("€%1").arg(QString::number(total, 'f', 2))));

        QString produits = query.value("produits").toString();
        if (produits.isEmpty()) {
            produits = "Aucun produit";
        }
        ordersTable->setItem(row, 6, new QTableWidgetItem(produits));

        if (userRole == "ADMIN") {
            QWidget *actionWidget = new QWidget();
            QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
            actionLayout->setContentsMargins(2, 2, 2, 2);
            actionLayout->setSpacing(4);
            actionLayout->setAlignment(Qt::AlignCenter);

            QPushButton *editBtn = new QPushButton("✏️");
            editBtn->setFixedSize(22, 22);
            editBtn->setCursor(Qt::PointingHandCursor);
            editBtn->setStyleSheet(
                "QPushButton {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                "   stop:0 #667eea, stop:1 #764ba2);"
                "   color: white;"
                "   border: none;"
                "   border-radius: 4px;"
                "   font-size: 11px;"
                "   font-weight: bold;"
                "   padding: 0px;"
                "}"
                "QPushButton:hover {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                "   stop:0 #5568d3, stop:1 #6a3a8a);"
                "}"
                "QPushButton:pressed {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                "   stop:0 #4556b8, stop:1 #5a2a7a);"
                "}"
            );

            QPushButton *deleteBtn = new QPushButton("🗑️");
            deleteBtn->setFixedSize(22, 22);
            deleteBtn->setCursor(Qt::PointingHandCursor);
            deleteBtn->setStyleSheet(
                "QPushButton {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                "   stop:0 #f56565, stop:1 #e53e3e);"
                "   color: white;"
                "   border: none;"
                "   border-radius: 4px;"
                "   font-size: 11px;"
                "   font-weight: bold;"
                "   padding: 0px;"
                "}"
                "QPushButton:hover {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                "   stop:0 #e53e3e, stop:1 #c53030);"
                "}"
                "QPushButton:pressed {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                "   stop:0 #c53030, stop:1 #742a2a);"
                "}"
            );

            int idCommande = query.value("id_commande").toInt();
            connect(editBtn, &QPushButton::clicked, this, [this, idCommande]() {
                onEditOrder(QString::number(idCommande));
            });
            connect(deleteBtn, &QPushButton::clicked, this, [this, idCommande]() {
                onDeleteOrder(QString::number(idCommande));
            });

            actionLayout->addWidget(editBtn);
            actionLayout->addWidget(deleteBtn);

            ordersTable->setCellWidget(row, 7, actionWidget);
        }

        row++;
    }
}

void OrdersPage::updatePaginationUI()
{
    pageInfoLabel->setText(QString("Page %1 / %2").arg(currentPage).arg(totalPages));
    btnFirstPage->setEnabled(currentPage > 1);
    btnPreviousPage->setEnabled(currentPage > 1);
    btnNextPage->setEnabled(currentPage < totalPages);
    btnLastPage->setEnabled(currentPage < totalPages);
}

void OrdersPage::onSearchTextChanged(const QString &text)
{
    currentSearchText = text;
    currentPage = 1;
    loadOrders();
}

void OrdersPage::onStatusFilterChanged(const QString &text)
{
    currentStatusFilter = statusFilter->currentData().toString();
    currentPage = 1;
    loadOrders();
}

void OrdersPage::onFirstPageClicked()
{
    currentPage = 1;
    loadOrders();
}

void OrdersPage::onPreviousPageClicked()
{
    if (currentPage > 1) {
        currentPage--;
        loadOrders();
    }
}

void OrdersPage::onNextPageClicked()
{
    if (currentPage < totalPages) {
        currentPage++;
        loadOrders();
    }
}

void OrdersPage::onLastPageClicked()
{
    currentPage = totalPages;
    loadOrders();
}

void OrdersPage::onRefreshClicked()
{
    loadOrders();
}

void OrdersPage::onViewOrderDetails(int row, int column)
{
    // TODO: Handle order details view
}

void OrdersPage::onEditOrder(const QString &orderId)
{
}

void OrdersPage::onDeleteOrder(const QString &orderId)
{
}
