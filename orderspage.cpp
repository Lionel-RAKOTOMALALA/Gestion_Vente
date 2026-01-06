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
    // Les tables sont créées automatiquement dans OrderDialog
}

void OrdersPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *icon = new QLabel(this);
    icon->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #667eea, stop:1 #764ba2);"
        "border-radius: 12px;"
        "min-width: 48px; max-width: 48px;"
        "min-height: 48px; max-height: 48px;"
    );

    QLabel *title = new QLabel("Gestion des Commandes", this);
    title->setObjectName("titleH1");
    title->setStyleSheet(
        "font-size: 32px;"
        "font-weight: 700;"
        "color: #1a202c;"
        "letter-spacing: -0.5px;"
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
    searchInput->setStyleSheet(
        "QLineEdit {"
        "   border: 1px solid #e2e8f0;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 15px;"
        "   background: white;"
        "   color: #2d3748;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #667eea;"
        "   outline: none;"
        "}"
        "QLineEdit::placeholder {"
        "   color: #a0aec0;"
        "}"
    );
    connect(searchInput, &QLineEdit::textChanged, this, &OrdersPage::onSearchTextChanged);
    filterLayout->addWidget(searchInput);

    statusFilter = new QComboBox(this);
    statusFilter->setMinimumHeight(48);
    statusFilter->addItem("Tous les statuts", "");
    statusFilter->addItem("En cours", "EN_COURS");
    statusFilter->addItem("Payée", "PAYEE");
    statusFilter->addItem("Annulée", "ANNULEE");
    statusFilter->setStyleSheet(
        "QComboBox {"
        "   border: 1px solid #e2e8f0;"
        "   border-radius: 12px;"
        "   padding: 12px 20px;"
        "   font-size: 15px;"
        "   background: white;"
        "   color: #2d3748;"
        "}"
        "QComboBox:focus {"
        "   border: 2px solid #667eea;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
        "QComboBox::down-arrow {"
        "   image: url(down_arrow.png);"
        "}"
    );
    connect(statusFilter, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &OrdersPage::onStatusFilterChanged);
    filterLayout->addWidget(statusFilter);

    refreshBtn = new QPushButton("Actualiser", this);
    refreshBtn->setMinimumHeight(48);
    refreshBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #3b82f6, stop:1 #2563eb);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 12px 24px;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #2563eb, stop:1 #1d4ed8);"
        "}"
        "QPushButton:pressed {"
        "   background: #1d4ed8;"
        "}"
    );
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
    ordersTable->setAlternatingRowColors(true);
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ordersTable->verticalHeader()->setDefaultSectionSize(50);
    ordersTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ordersTable->setStyleSheet(
        "QTableWidget {"
        "   background: white;"
        "   border: 1px solid #e2e8f0;"
        "   border-radius: 16px;"
        "   font-size: 14px;"
        "   color: #2d3748;"
        "}"
        "QTableWidget::item {"
        "   padding: 16px 16px;"
        "   border-bottom: 1px solid #f7fafc;"
        "}"
        "QTableWidget::item:selected {"
        "   background: #edf2f7;"
        "   color: #2d3748;"
        "}"
        "QHeaderView::section {"
        "   background: #f8fafc;"
        "   color: #4a5568;"
        "   padding: 18px 16px;"
        "   border: none;"
        "   border-bottom: 2px solid #e2e8f0;"
        "   font-weight: 700;"
        "   font-size: 12px;"
        "   text-transform: uppercase;"
        "   letter-spacing: 0.8px;"
        "   text-align: left;"
        "}"
        "QHeaderView::section:first {"
        "   border-top-left-radius: 16px;"
        "}"
        "QHeaderView::section:last {"
        "   border-top-right-radius: 16px;"
        "}"
        "QScrollBar:vertical {"
        "   background: #f7fafc;"
        "   width: 12px;"
        "   border-radius: 6px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "   stop:0 #667eea, stop:1 #764ba2);"
        "   border-radius: 6px;"
        "   min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "   stop:0 #5568d3, stop:1 #6b3f8f);"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "   background: none;"
        "}"
        "QScrollBar:horizontal {"
        "   background: #f7fafc;"
        "   height: 12px;"
        "   border-radius: 6px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "   stop:0 #667eea, stop:1 #764ba2);"
        "   border-radius: 6px;"
        "   min-width: 30px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "   stop:0 #5568d3, stop:1 #6b3f8f);"
        "}"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
        "   width: 0px;"
        "}"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
        "   background: none;"
        "}"
    );

    // Ajuster les largeurs des colonnes
    ordersTable->setColumnWidth(0, 100);  // N° Commande
    ordersTable->setColumnWidth(1, 150);  // Date
    ordersTable->setColumnWidth(2, 200);  // Client
    ordersTable->setColumnWidth(3, 150);  // Vendeur
    ordersTable->setColumnWidth(4, 100);  // Statut
    ordersTable->setColumnWidth(5, 100);  // Total
    if (userRole == "ADMIN") {
        ordersTable->setColumnWidth(6, 200);  // Produits
        ordersTable->setColumnWidth(7, 150);  // Actions
    } else {
        ordersTable->horizontalHeader()->setStretchLastSection(true);
    }

    // Définir la hauteur des lignes pour accommoder les boutons
    ordersTable->verticalHeader()->setDefaultSectionSize(60);

    connect(ordersTable, &QTableWidget::cellDoubleClicked, this, &OrdersPage::onViewOrderDetails);

    mainLayout->addWidget(ordersTable);

    // Pagination
    paginationWidget = new QWidget(this);
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setSpacing(12);
    
    btnFirstPage = new QPushButton("|<", paginationWidget);
    btnFirstPage->setFixedSize(45, 45);
    btnFirstPage->setStyleSheet(
        "QPushButton {"
        "   background: #f8fafc;"
        "   color: #4a5568;"
        "   border: 1px solid #e2e8f0;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: #e2e8f0;"
        "}"
        "QPushButton:pressed {"
        "   background: #cbd5e0;"
        "}"
        "QPushButton:disabled {"
        "   color: #a0aec0;"
        "   background: #f7fafc;"
        "}"
    );
    connect(btnFirstPage, &QPushButton::clicked, this, &OrdersPage::onFirstPageClicked);
    paginationLayout->addWidget(btnFirstPage);

    btnPreviousPage = new QPushButton("<", paginationWidget);
    btnPreviousPage->setFixedSize(45, 45);
    btnPreviousPage->setStyleSheet(
        "QPushButton {"
        "   background: #f8fafc;"
        "   color: #4a5568;"
        "   border: 1px solid #e2e8f0;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: #e2e8f0;"
        "}"
        "QPushButton:pressed {"
        "   background: #cbd5e0;"
        "}"
        "QPushButton:disabled {"
        "   color: #a0aec0;"
        "   background: #f7fafc;"
        "}"
    );
    connect(btnPreviousPage, &QPushButton::clicked, this, &OrdersPage::onPreviousPageClicked);
    paginationLayout->addWidget(btnPreviousPage);

    pageInfoLabel = new QLabel("Page 1 / 1", paginationWidget);
    pageInfoLabel->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 500;"
        "color: #4a5568;"
        "padding: 0 16px;"
    );
    paginationLayout->addWidget(pageInfoLabel);

    btnNextPage = new QPushButton(">", paginationWidget);
    btnNextPage->setFixedSize(45, 45);
    btnNextPage->setStyleSheet(
        "QPushButton {"
        "   background: #f8fafc;"
        "   color: #4a5568;"
        "   border: 1px solid #e2e8f0;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: #e2e8f0;"
        "}"
        "QPushButton:pressed {"
        "   background: #cbd5e0;"
        "}"
        "QPushButton:disabled {"
        "   color: #a0aec0;"
        "   background: #f7fafc;"
        "}"
    );
    connect(btnNextPage, &QPushButton::clicked, this, &OrdersPage::onNextPageClicked);
    paginationLayout->addWidget(btnNextPage);

    btnLastPage = new QPushButton(">|", paginationWidget);
    btnLastPage->setFixedSize(45, 45);
    btnLastPage->setStyleSheet(
        "QPushButton {"
        "   background: #f8fafc;"
        "   color: #4a5568;"
        "   border: 1px solid #e2e8f0;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: #e2e8f0;"
        "}"
        "QPushButton:pressed {"
        "   background: #cbd5e0;"
        "}"
        "QPushButton:disabled {"
        "   color: #a0aec0;"
        "   background: #f7fafc;"
        "}"
    );
    connect(btnLastPage, &QPushButton::clicked, this, &OrdersPage::onLastPageClicked);
    paginationLayout->addWidget(btnLastPage);

    paginationLayout->addStretch();
    mainLayout->addWidget(paginationWidget);
}

void OrdersPage::loadOrders()
{
    ordersTable->setRowCount(0);

    // D'abord, compter le nombre total d'éléments
    QString countQueryStr = R"(
        SELECT COUNT(DISTINCT c.id_commande) as total
        FROM COMMANDES c
        LEFT JOIN CLIENTS cl ON c.id_client = cl.id_client
        LEFT JOIN USERS u ON c.id_user = u.id_user
        LEFT JOIN COMMANDE_DETAIL cd ON c.id_commande = cd.id_commande
        LEFT JOIN PRODUITS p ON cd.id_produit = p.id_produit
        WHERE 1=1
    )";

    // Appliquer les filtres pour le comptage
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
        totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage; // Division avec arrondi supérieur
        if (totalPages == 0) totalPages = 1;
    } else {
        totalItems = 0;
        totalPages = 1;
    }

    // Ajuster la page actuelle si nécessaire
    if (currentPage > totalPages) {
        currentPage = totalPages;
    }
    if (currentPage < 1) {
        currentPage = 1;
    }

    // Mettre à jour l'interface de pagination
    updatePaginationUI();

    // Maintenant charger les données de la page actuelle
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

        // N° Commande
        int idCommande = query.value("id_commande").toInt();
        ordersTable->setItem(row, 0, new QTableWidgetItem(QString::number(idCommande)));

        // Date
        QString dateStr = query.value("date_commande").toString();
        QDateTime dateTime = QDateTime::fromString(dateStr, "yyyy-MM-ddTHH:mm:ss");
        if (!dateTime.isValid()) {
            dateTime = QDateTime::fromString(dateStr, "yyyy-MM-dd HH:mm:ss");
        }
        QString formattedDate = dateTime.isValid() ? dateTime.toString("dd/MM/yyyy HH:mm") : dateStr;
        ordersTable->setItem(row, 1, new QTableWidgetItem(formattedDate));

        // Client
        QString clientNom = query.value("client_nom").toString().trimmed();
        ordersTable->setItem(row, 2, new QTableWidgetItem(clientNom));

        // Vendeur
        QString vendeurNom = query.value("vendeur_nom").toString();
        ordersTable->setItem(row, 3, new QTableWidgetItem(vendeurNom));

        // Statut avec couleur
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

        // Total
        double total = query.value("total").toDouble();
        ordersTable->setItem(row, 5, new QTableWidgetItem(QString("€%1").arg(QString::number(total, 'f', 2))));

        // Produits
        QString produits = query.value("produits").toString();
        if (produits.isEmpty()) {
            produits = "Aucun produit";
        }
        ordersTable->setItem(row, 6, new QTableWidgetItem(produits));

        // Actions (uniquement pour les admins)
        if (userRole == "ADMIN") {
            QWidget *actionWidget = new QWidget();
            QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
            actionLayout->setContentsMargins(4, 4, 4, 4);
            actionLayout->setSpacing(6);

            QPushButton *editBtn = new QPushButton("✏️");
            editBtn->setFixedSize(24, 24);
            editBtn->setCursor(Qt::PointingHandCursor);
            editBtn->setToolTip("Modifier");
            editBtn->setStyleSheet(
                "QPushButton {"
                "   background: #3b82f6;"
                "   color: white;"
                "   border: 2px solid #3b82f6;"
                "   border-radius: 6px;"
                "   font-size: 12px;"
                "   font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "   background: #2563eb;"
                "   border-color: #2563eb;"
                "   transform: scale(1.1);"
                "}"
                "QPushButton:pressed {"
                "   background: #1d4ed8;"
                "   border-color: #1d4ed8;"
                "}"
            );
            connect(editBtn, &QPushButton::clicked, this, [this, idCommande]() {
                onEditOrder(QString::number(idCommande));
            });

            QPushButton *deleteBtn = new QPushButton("🗑️");
            deleteBtn->setFixedSize(24, 24);
            deleteBtn->setCursor(Qt::PointingHandCursor);
            deleteBtn->setToolTip("Supprimer");
            deleteBtn->setStyleSheet(
                "QPushButton {"
                "   background: #ef4444;"
                "   color: white;"
                "   border: 2px solid #ef4444;"
                "   border-radius: 6px;"
                "   font-size: 12px;"
                "   font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "   background: #dc2626;"
                "   border-color: #dc2626;"
                "   transform: scale(1.1);"
                "}"
                "QPushButton:pressed {"
                "   background: #b91c1c;"
                "   border-color: #b91c1c;"
                "}"
            );
            connect(deleteBtn, &QPushButton::clicked, this, [this, idCommande]() {
                onDeleteOrder(QString::number(idCommande));
            });

            actionLayout->addWidget(editBtn);
            actionLayout->addWidget(deleteBtn);
            actionLayout->addStretch();

            ordersTable->setCellWidget(row, 7, actionWidget);
        }

        row++;
    }

    // Message si aucune commande
    if (row == 0) {
        ordersTable->insertRow(0);
        QTableWidgetItem *emptyItem = new QTableWidgetItem("Aucune commande trouvée");
        emptyItem->setTextAlignment(Qt::AlignCenter);
        int spanColumns = (userRole == "ADMIN") ? 8 : 7;
        ordersTable->setItem(0, 0, emptyItem);
        ordersTable->setSpan(0, 0, 1, spanColumns);
    }
}

void OrdersPage::onSearchTextChanged(const QString &text)
{
    currentSearchText = text;
    loadOrders();
}

void OrdersPage::onStatusFilterChanged(const QString &status)
{
    currentStatusFilter = statusFilter->currentData().toString();
    loadOrders();
}

void OrdersPage::onRefreshClicked()
{
    loadOrders();
}

void OrdersPage::onViewOrderDetails(int row, int column)
{
    if (row >= 0 && ordersTable->item(row, 0)) {
        QString commandeId = ordersTable->item(row, 0)->text();
        // Ici on pourrait ouvrir un dialog avec les détails complets de la commande
        QMessageBox::information(this, "Détails commande",
                               QString("Affichage des détails de la commande N°%1").arg(commandeId));
    }
}

void OrdersPage::onEditOrder(const QString &commandeId)
{
    // Ouvrir le dialog d'ordre en mode édition
    OrderDialog *orderDialog = new OrderDialog(userId, commandeId, this);
    orderDialog->setModal(true);
    orderDialog->show();

    // Recharger les commandes après modification
    connect(orderDialog, &OrderDialog::orderSaved, this, &OrdersPage::loadOrders);
}

void OrdersPage::onDeleteOrder(const QString &commandeId)
{
    // Demander confirmation
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmer la suppression",
        QString("Êtes-vous sûr de vouloir supprimer la commande N°%1 ?\nCette action est irréversible.").arg(commandeId),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        QSqlDatabase db = QSqlDatabase::database();
        if (db.open()) {
            QSqlQuery query(db);

            // Supprimer d'abord les détails de la commande
            query.prepare("DELETE FROM COMMANDE_DETAIL WHERE commande_id = ?");
            query.addBindValue(commandeId.toInt());
            if (!query.exec()) {
                QMessageBox::critical(this, "Erreur",
                                    "Erreur lors de la suppression des détails de commande: " + query.lastError().text());
                return;
            }

            // Supprimer la commande
            query.prepare("DELETE FROM COMMANDES WHERE id = ?");
            query.addBindValue(commandeId.toInt());
            if (query.exec()) {
                QMessageBox::information(this, "Succès",
                                       QString("Commande N°%1 supprimée avec succès.").arg(commandeId));
                loadOrders(); // Recharger la liste
            } else {
                QMessageBox::critical(this, "Erreur",
                                    "Erreur lors de la suppression de la commande: " + query.lastError().text());
            }
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données.");
        }
    }
}

void OrdersPage::updatePaginationUI()
{
    // Activer/désactiver les boutons selon la page actuelle
    btnFirstPage->setEnabled(currentPage > 1);
    btnPreviousPage->setEnabled(currentPage > 1);
    btnNextPage->setEnabled(currentPage < totalPages);
    btnLastPage->setEnabled(currentPage < totalPages);

    // Mettre à jour le label d'information de page
    if (totalItems == 0) {
        pageInfoLabel->setText("Aucune commande");
    } else {
        int startItem = (currentPage - 1) * itemsPerPage + 1;
        int endItem = qMin(currentPage * itemsPerPage, totalItems);
        pageInfoLabel->setText(QString("Page %1 sur %2 (%3-%4 sur %5)")
                              .arg(currentPage)
                              .arg(totalPages)
                              .arg(startItem)
                              .arg(endItem)
                              .arg(totalItems));
    }
}

void OrdersPage::onFirstPageClicked()
{
    if (currentPage > 1) {
        currentPage = 1;
        loadOrders();
    }
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
    if (currentPage < totalPages) {
        currentPage = totalPages;
        loadOrders();
    }
}
