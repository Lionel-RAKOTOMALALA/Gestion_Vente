#include "userspage.h"
#include "userdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QScrollBar>

UsersPage::UsersPage(QWidget *parent) : QFrame(parent), currentPage(0), itemsPerPage(5), totalItems(0)
{
    setObjectName("usersPage");
    setupDatabase();
    setupUI();
    applyStyles();
    loadUsers();
}

void UsersPage::setupDatabase()
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS USERS ("
               "id_user INTEGER PRIMARY KEY AUTOINCREMENT, "
               "nom TEXT NOT NULL, "
               "email TEXT UNIQUE NOT NULL, "
               "mot_de_passe TEXT NOT NULL, "
               "role TEXT CHECK(role IN ('ADMIN','VENDEUR','CAISSIER')) NOT NULL, "
               "actif INTEGER DEFAULT 1, "
               "date_creation DATETIME DEFAULT CURRENT_TIMESTAMP)");
}

void UsersPage::setupUI()
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
    
    QLabel *title = new QLabel("Gestion des Utilisateurs", this);
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

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(16);

    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Rechercher par nom ou email...");
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
    connect(searchInput, &QLineEdit::textChanged, this, &UsersPage::onSearchTextChanged);

    roleFilter = new QComboBox(this);
    roleFilter->addItems({"Tous les roles", "ADMIN", "VENDEUR", "CAISSIER"});
    roleFilter->setMinimumHeight(48);
    roleFilter->setMinimumWidth(200);
    roleFilter->setStyleSheet(
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
        "   width: 35px;"
        "}"
        "QComboBox::down-arrow {"
        "   width: 12px;"
        "   height: 12px;"
        "}"
    );
    connect(roleFilter, &QComboBox::currentTextChanged, this, &UsersPage::onFilterRoleChanged);

    searchLayout->addWidget(searchInput, 3);
    searchLayout->addWidget(roleFilter, 1);
    mainLayout->addLayout(searchLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    btnAdd = new QPushButton("+ Ajouter", this);
    btnAdd->setMinimumHeight(48);
    btnAdd->setCursor(Qt::PointingHandCursor);
    btnAdd->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #3b82f6, stop:1 #2563eb);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 12px 32px;"
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
    connect(btnAdd, &QPushButton::clicked, this, &UsersPage::onAddUser);

    btnRefresh = new QPushButton("Actualiser", this);
    btnRefresh->setMinimumHeight(48);
    btnRefresh->setCursor(Qt::PointingHandCursor);
    btnRefresh->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "   stop:0 #3b82f6, stop:1 #2563eb);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 12px 32px;"
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
    connect(btnRefresh, &QPushButton::clicked, this, &UsersPage::loadUsers);

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
    
    tableWidget->setStyleSheet(
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

    QStringList headers = {"ID", "NOM", "EMAIL", "ROLE", "ACTIONS"};
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    tableWidget->setColumnWidth(0, 80);
    tableWidget->setColumnWidth(1, 250);
    tableWidget->setColumnWidth(2, 320);
    tableWidget->setColumnWidth(3, 150);
    tableWidget->setColumnWidth(4, 200);

    tableWidget->verticalHeader()->setDefaultSectionSize(35);

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
    connect(btnFirstPage, &QPushButton::clicked, this, &UsersPage::onFirstPage);
    
    btnPrevPage = new QPushButton("<", this);
    btnPrevPage->setFixedSize(45, 45);
    btnPrevPage->setCursor(Qt::PointingHandCursor);
    btnPrevPage->setStyleSheet(btnFirstPage->styleSheet());
    connect(btnPrevPage, &QPushButton::clicked, this, &UsersPage::onPreviousPage);
    
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
    btnNextPage->setStyleSheet(btnFirstPage->styleSheet());
    connect(btnNextPage, &QPushButton::clicked, this, &UsersPage::onNextPage);
    
    btnLastPage = new QPushButton(">|", this);
    btnLastPage->setFixedSize(45, 45);
    btnLastPage->setCursor(Qt::PointingHandCursor);
    btnLastPage->setStyleSheet(btnFirstPage->styleSheet());
    connect(btnLastPage, &QPushButton::clicked, this, &UsersPage::onLastPage);
    
    paginationLayout->addStretch();
    paginationLayout->addWidget(btnFirstPage);
    paginationLayout->addWidget(btnPrevPage);
    paginationLayout->addWidget(lblPageInfo);
    paginationLayout->addWidget(btnNextPage);
    paginationLayout->addWidget(btnLastPage);
    paginationLayout->addStretch();
    
    mainLayout->addLayout(paginationLayout);
}

void UsersPage::applyStyles()
{
    setStyleSheet(
        "#usersPage {"
        "   background: #f7fafc;"
        "}"
    );
}

QWidget* UsersPage::createRoleBadge(const QString &role)
{
    QWidget *badgeContainer = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(badgeContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft);
    
    QLabel *badge = new QLabel(role);
    badge->setAlignment(Qt::AlignCenter);
    
    QString bgColor, textColor;
    if (role == "ADMIN") {
        bgColor = "#fee2e2";
        textColor = "#991b1b";
    } else if (role == "VENDEUR") {
        bgColor = "#dbeafe";
        textColor = "#1e40af";
    } else {
        bgColor = "#fef3c7";
        textColor = "#92400e";
    }
    
    badge->setStyleSheet(QString(
        "background: %1;"
        "color: %2;"
        "border-radius: 10px;"
        "padding: 8px 20px;"
        "font-weight: 700;"
        "font-size: 11px;"
        "letter-spacing: 0.5px;"
    ).arg(bgColor).arg(textColor));
    
    layout->addWidget(badge);
    return badgeContainer;
}

QWidget* UsersPage::createActionButtons(int userId)
{
    QWidget *actionWidget = new QWidget();
    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
    actionLayout->setContentsMargins(4, 4, 4, 4);
    actionLayout->setSpacing(6);

    QPushButton *editBtn = new QPushButton("✏️", this);
    editBtn->setFixedSize(24, 24);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setToolTip("Modifier l'utilisateur");
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
    connect(editBtn, &QPushButton::clicked, this, [this, userId]() { onEditUser(userId); });

    QPushButton *deleteBtn = new QPushButton("🗑️", this);
    deleteBtn->setFixedSize(24, 24);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setToolTip("Supprimer l'utilisateur");
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
    connect(deleteBtn, &QPushButton::clicked, this, [this, userId]() { onDeleteUser(userId); });

    actionLayout->addWidget(editBtn);
    actionLayout->addWidget(deleteBtn);

    return actionWidget;
}

void UsersPage::loadUsers()
{
    QSqlQuery countQuery;
    QString filter = "1=1";

    QString searchText = searchInput->text();
    if (!searchText.isEmpty()) {
        filter += QString(" AND (nom LIKE '%%1%' OR email LIKE '%%1%')").arg(searchText);
    }

    QString roleText = roleFilter->currentText();
    if (roleText != "Tous les roles") {
        filter += QString(" AND role = '%1'").arg(roleText);
    }

    countQuery.prepare(QString("SELECT COUNT(*) FROM USERS WHERE %1").arg(filter));
    if (countQuery.exec() && countQuery.next()) {
        totalItems = countQuery.value(0).toInt();
    }

    tableWidget->setRowCount(0);

    QSqlQuery query;
    int offset = currentPage * itemsPerPage;
    
    query.prepare(QString("SELECT id_user, nom, email, role FROM USERS WHERE %1 ORDER BY date_creation DESC LIMIT %2 OFFSET %3")
                  .arg(filter).arg(itemsPerPage).arg(offset));

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des utilisateurs: " + query.lastError().text());
        return;
    }

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);

        int id = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString email = query.value(2).toString();
        QString role = query.value(3).toString();

        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(id));
        idItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        idItem->setForeground(QColor("#718096"));
        idItem->setFont(QFont("Arial", 13, QFont::Bold));
        
        QTableWidgetItem *nomItem = new QTableWidgetItem(nom);
        nomItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        nomItem->setFont(QFont("Arial", 14, QFont::DemiBold));
        nomItem->setForeground(QColor("#1a202c"));
        
        QTableWidgetItem *emailItem = new QTableWidgetItem(email);
        emailItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        emailItem->setForeground(QColor("#4a5568"));

        tableWidget->setItem(row, 0, idItem);
        tableWidget->setItem(row, 1, nomItem);
        tableWidget->setItem(row, 2, emailItem);
        
        tableWidget->setCellWidget(row, 3, createRoleBadge(role));
        tableWidget->setCellWidget(row, 4, createActionButtons(id));

        row++;
    }
    
    updatePaginationControls();
}

void UsersPage::updatePaginationControls()
{
    int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;
    if (totalPages == 0) totalPages = 1;
    
    lblPageInfo->setText(QString("Page %1 / %2 (%3 utilisateurs)")
                         .arg(currentPage + 1)
                         .arg(totalPages)
                         .arg(totalItems));
    
    btnFirstPage->setEnabled(currentPage > 0);
    btnPrevPage->setEnabled(currentPage > 0);
    btnNextPage->setEnabled(currentPage < totalPages - 1);
    btnLastPage->setEnabled(currentPage < totalPages - 1);
}

void UsersPage::onFirstPage()
{
    currentPage = 0;
    loadUsers();
}

void UsersPage::onPreviousPage()
{
    if (currentPage > 0) {
        currentPage--;
        loadUsers();
    }
}

void UsersPage::onNextPage()
{
    int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;
    if (currentPage < totalPages - 1) {
        currentPage++;
        loadUsers();
    }
}

void UsersPage::onLastPage()
{
    int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;
    currentPage = totalPages - 1;
    if (currentPage < 0) currentPage = 0;
    loadUsers();
}

void UsersPage::onAddUser()
{
    UserDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        currentPage = 0;
        loadUsers();
    }
}

void UsersPage::onEditUser(int userId)
{
    UserDialog dialog(this, userId);
    if (dialog.exec() == QDialog::Accepted) {
        loadUsers();
    }
}

void UsersPage::onDeleteUser(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT nom FROM USERS WHERE id_user = :id");
    query.bindValue(":id", userId);
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Erreur", "Utilisateur introuvable.");
        return;
    }
    QString nom = query.value(0).toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "Confirmation", 
        QString("Voulez-vous vraiment supprimer l'utilisateur '%1' ?").arg(nom),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        query.prepare("DELETE FROM USERS WHERE id_user = :id");
        query.bindValue(":id", userId);
        if (query.exec()) {
            QMessageBox::information(this, "Succes", "Utilisateur supprime avec succes.");
            loadUsers();
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression: " + query.lastError().text());
        }
    }
}

void UsersPage::onSearchTextChanged(const QString &text)
{
    currentPage = 0;
    loadUsers();
}

void UsersPage::onFilterRoleChanged(const QString &role)
{
    currentPage = 0;
    loadUsers();
}
