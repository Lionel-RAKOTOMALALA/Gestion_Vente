#include "clientspage.h"
#include "clientdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QScrollBar>
#include "thememanager.h"

ClientsPage::ClientsPage(QWidget *parent) : QFrame(parent), currentPage(0), itemsPerPage(5), totalItems(0)
{
    setObjectName("clientsPage");
    setupDatabase();
    setupUI();
    applyStyles();
    loadClients();
}

void ClientsPage::setupDatabase()
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS CLIENTS ("
               "id_client INTEGER PRIMARY KEY AUTOINCREMENT, "
               "nom TEXT NOT NULL, "
               "prenom TEXT, "
               "telephone TEXT, "
               "email TEXT, "
               "adresse TEXT, "
               "date_creation DATETIME DEFAULT CURRENT_TIMESTAMP)");
}

void ClientsPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *icon = new QLabel(this);
    icon->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #667eea, stop:1 #764ba2);"
        "border-radius: 12px;"
        "min-width: 48px; max-width: 48px;"
        "min-height: 48px; max-height: 48px;"
    );
    
    QLabel *title = new QLabel("Gestion des Clients", this);
    title->setObjectName("titleH1");
    ThemeManager& theme = ThemeManager::instance();
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

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(16);

    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Rechercher par nom ou email...");
    searchInput->setMinimumHeight(48);
    connect(searchInput, &QLineEdit::textChanged, this, &ClientsPage::onSearchTextChanged);

    searchLayout->addWidget(searchInput, 1);
    mainLayout->addLayout(searchLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    btnAdd = new QPushButton("+ Ajouter", this);
    btnAdd->setMinimumHeight(48);
    btnAdd->setCursor(Qt::PointingHandCursor);
    connect(btnAdd, &QPushButton::clicked, this, &ClientsPage::onAddClient);

    btnRefresh = new QPushButton("Actualiser", this);
    btnRefresh->setMinimumHeight(48);
    btnRefresh->setCursor(Qt::PointingHandCursor);
    connect(btnRefresh, &QPushButton::clicked, this, &ClientsPage::loadClients);

    buttonLayout->addWidget(btnAdd);
    buttonLayout->addWidget(btnRefresh);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    tableWidget = new QTableWidget(this);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(false);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setShowGrid(false);
    tableWidget->verticalHeader()->setDefaultSectionSize(60);
    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    QStringList headers = {"ID", "NOM", "PRÉNOM", "TÉLÉPHONE", "EMAIL", "ADRESSE", "ACTIONS"};
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    tableWidget->setColumnWidth(0, 60);
    tableWidget->setColumnWidth(1, 130);
    tableWidget->setColumnWidth(2, 130);
    tableWidget->setColumnWidth(3, 130);
    tableWidget->setColumnWidth(4, 170);
    tableWidget->setColumnWidth(5, 150);
    tableWidget->setColumnWidth(6, 150);

    // Définir la hauteur des lignes pour accommoder les boutons
    tableWidget->verticalHeader()->setDefaultSectionSize(50);

    mainLayout->addWidget(tableWidget);

    QHBoxLayout *paginationLayout = new QHBoxLayout();
    paginationLayout->setSpacing(12);
    
    btnFirstPage = new QPushButton("|<", this);
    btnFirstPage->setFixedSize(45, 45);
    btnFirstPage->setCursor(Qt::PointingHandCursor);
    btnFirstPage->setStyleSheet(
        "QPushButton {"
        "   background: white;"
        "   color: #4a5568;"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   font-weight: bold;"
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
        "}"
    );
    connect(btnFirstPage, &QPushButton::clicked, this, &ClientsPage::onFirstPage);
    
    btnPrevPage = new QPushButton("<", this);
    btnPrevPage->setFixedSize(45, 45);
    btnPrevPage->setCursor(Qt::PointingHandCursor);
    btnPrevPage->setStyleSheet(
        "QPushButton {"
        "   background: white;"
        "   color: #4a5568;"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 10px;"
        "   font-size: 18px;"
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
        "}"
    );
    connect(btnPrevPage, &QPushButton::clicked, this, &ClientsPage::onPreviousPage);
    
    lblPageInfo = new QLabel("Page 1 / 1", this);
    lblPageInfo->setAlignment(Qt::AlignCenter);
    lblPageInfo->setMinimumWidth(150);
    lblPageInfo->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 600;"
        "color: #4a5568;"
        "background: white;"
        "border: 2px solid #e2e8f0;"
        "border-radius: 10px;"
        "padding: 10px 20px;"
    );
    
    btnNextPage = new QPushButton(">", this);
    btnNextPage->setFixedSize(45, 45);
    btnNextPage->setCursor(Qt::PointingHandCursor);
    btnNextPage->setStyleSheet(
        "QPushButton {"
        "   background: white;"
        "   color: #4a5568;"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 10px;"
        "   font-size: 18px;"
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
        "}"
    );
    connect(btnNextPage, &QPushButton::clicked, this, &ClientsPage::onNextPage);
    
    btnLastPage = new QPushButton(">|", this);
    btnLastPage->setFixedSize(45, 45);
    btnLastPage->setCursor(Qt::PointingHandCursor);
    btnLastPage->setStyleSheet(btnFirstPage->styleSheet());
    connect(btnLastPage, &QPushButton::clicked, this, &ClientsPage::onLastPage);
    
    paginationLayout->addStretch();
    paginationLayout->addWidget(btnFirstPage);
    paginationLayout->addWidget(btnPrevPage);
    paginationLayout->addWidget(lblPageInfo);
    paginationLayout->addWidget(btnNextPage);
    paginationLayout->addWidget(btnLastPage);
    paginationLayout->addStretch();
    
    mainLayout->addLayout(paginationLayout);
}

void ClientsPage::applyStyles()
{
    // Mode sombre forcé
    setStyleSheet(QString(
        "#clientsPage {"
        "   background: %1;"
        "}"
        "QTableWidget {"
        "   background: %1;"
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
    ).arg(ThemeManager::instance().backgroundColor().name()));
}

QWidget* ClientsPage::createActionButtons(int clientId)
{
    QWidget *actionWidget = new QWidget();
    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
    actionLayout->setContentsMargins(2, 2, 2, 2);
    actionLayout->setSpacing(4);
    actionLayout->setAlignment(Qt::AlignCenter);

    QPushButton *editBtn = new QPushButton("✏️", this);
    editBtn->setFixedSize(22, 22);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setToolTip("Modifier le client");
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
    connect(editBtn, &QPushButton::clicked, this, [this, clientId]() { onEditClient(clientId); });

    QPushButton *deleteBtn = new QPushButton("🗑️", this);
    deleteBtn->setFixedSize(22, 22);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setToolTip("Supprimer le client");
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
    connect(deleteBtn, &QPushButton::clicked, this, [this, clientId]() { onDeleteClient(clientId); });

    actionLayout->addWidget(editBtn);
    actionLayout->addWidget(deleteBtn);

    return actionWidget;
}

void ClientsPage::loadClients()
{
    QSqlQuery countQuery;
    QString filter = "1=1";

    QString searchText = searchInput->text();
    if (!searchText.isEmpty()) {
        filter += QString(" AND (nom LIKE '%%1%' OR prenom LIKE '%%1%' OR email LIKE '%%1%')").arg(searchText);
    }

    countQuery.prepare(QString("SELECT COUNT(*) FROM CLIENTS WHERE %1").arg(filter));
    if (countQuery.exec() && countQuery.next()) {
        totalItems = countQuery.value(0).toInt();
    }

    tableWidget->setRowCount(0);

    QSqlQuery query;
    int offset = currentPage * itemsPerPage;
    
    query.prepare(QString("SELECT id_client, nom, prenom, telephone, email, adresse FROM CLIENTS WHERE %1 ORDER BY date_creation DESC LIMIT %2 OFFSET %3")
                  .arg(filter).arg(itemsPerPage).arg(offset));

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des clients: " + query.lastError().text());
        return;
    }

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);

        int id = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString prenom = query.value(2).toString();
        QString telephone = query.value(3).toString();
        QString email = query.value(4).toString();
        QString adresse = query.value(5).toString();

        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(id));
        idItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        idItem->setForeground(QColor("#718096"));
        idItem->setFont(QFont("Arial", 13, QFont::Bold));
        
        QTableWidgetItem *nomItem = new QTableWidgetItem(nom);
        nomItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        nomItem->setFont(QFont("Arial", 14, QFont::DemiBold));
        nomItem->setForeground(QColor("#1a202c"));
        
        QTableWidgetItem *prenomItem = new QTableWidgetItem(prenom);
        prenomItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        prenomItem->setForeground(QColor("#4a5568"));

        QTableWidgetItem *telephoneItem = new QTableWidgetItem(telephone);
        telephoneItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        telephoneItem->setForeground(QColor("#4a5568"));

        QTableWidgetItem *emailItem = new QTableWidgetItem(email);
        emailItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        emailItem->setForeground(QColor("#4a5568"));

        QTableWidgetItem *adresseItem = new QTableWidgetItem(adresse);
        adresseItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        adresseItem->setForeground(QColor("#4a5568"));

        tableWidget->setItem(row, 0, idItem);
        tableWidget->setItem(row, 1, nomItem);
        tableWidget->setItem(row, 2, prenomItem);
        tableWidget->setItem(row, 3, telephoneItem);
        tableWidget->setItem(row, 4, emailItem);
        tableWidget->setItem(row, 5, adresseItem);
        tableWidget->setCellWidget(row, 6, createActionButtons(id));

        row++;
    }
    
    updatePaginationControls();
}

void ClientsPage::updatePaginationControls()
{
    int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;
    if (totalPages == 0) totalPages = 1;
    
    lblPageInfo->setText(QString("Page %1 / %2 (%3 clients)")
                         .arg(currentPage + 1)
                         .arg(totalPages)
                         .arg(totalItems));
    
    btnFirstPage->setEnabled(currentPage > 0);
    btnPrevPage->setEnabled(currentPage > 0);
    btnNextPage->setEnabled(currentPage < totalPages - 1);
    btnLastPage->setEnabled(currentPage < totalPages - 1);
}

void ClientsPage::onFirstPage()
{
    currentPage = 0;
    loadClients();
}

void ClientsPage::onPreviousPage()
{
    if (currentPage > 0) {
        currentPage--;
        loadClients();
    }
}

void ClientsPage::onNextPage()
{
    int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;
    if (currentPage < totalPages - 1) {
        currentPage++;
        loadClients();
    }
}

void ClientsPage::onLastPage()
{
    int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;
    currentPage = totalPages - 1;
    if (currentPage < 0) currentPage = 0;
    loadClients();
}

void ClientsPage::onAddClient()
{
    ClientDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        currentPage = 0;
        loadClients();
    }
}

void ClientsPage::onEditClient(int clientId)
{
    ClientDialog dialog(this, clientId);
    if (dialog.exec() == QDialog::Accepted) {
        loadClients();
    }
}

void ClientsPage::onDeleteClient(int clientId)
{
    QSqlQuery query;
    query.prepare("SELECT nom FROM CLIENTS WHERE id_client = :id");
    query.bindValue(":id", clientId);
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Erreur", "Client introuvable.");
        return;
    }
    QString nom = query.value(0).toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "Confirmation", 
        QString("Voulez-vous vraiment supprimer le client '%1' ?").arg(nom),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        query.prepare("DELETE FROM CLIENTS WHERE id_client = :id");
        query.bindValue(":id", clientId);
        if (query.exec()) {
            QMessageBox::information(this, "Succès", "Client supprimé avec succès.");
            loadClients();
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression: " + query.lastError().text());
        }
    }
}

void ClientsPage::onSearchTextChanged(const QString &text)
{
    currentPage = 0;
    loadClients();
}

void ClientsPage::onThemeChanged()
{
    applyStyles();
    loadClients();
}
