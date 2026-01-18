#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thememanager.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QApplication>
#include <QPalette>
#include "dashboardpage.h"
#include "userspage.h"
#include "clientspage.h"
#include "productspage.h"
#include "orderspage.h"
#include "paymentspage.h"
#include "cashpage.h"

MainWindow::MainWindow(const QString &userRole, int userId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentUserId(userId)
{
    qDebug() << "MainWindow constructor appelé";
    ui->setupUi(this);

    // Créer un nouveau central widget propre
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Créer le layout principal
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Créer la sidebar avec le rôle de l'utilisateur
    sidebar = new Sidebar(userRole, this);
    mainLayout->addWidget(sidebar);

    // Créer le stacked widget pour les pages
    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget, 1);

    // Ajouter le Dashboard en premier
    DashboardPage *dashboardPage = new DashboardPage(this);
    stackedWidget->addWidget(dashboardPage);

    // Ajouter les pages modulaires selon le rôle
    if (userRole != "VENDEUR") {
        UsersPage *usersPage = new UsersPage(this);
        stackedWidget->addWidget(usersPage);
    }

    ClientsPage *clientsPage = new ClientsPage(this);
    this->clientsPage = clientsPage;
    stackedWidget->addWidget(clientsPage);

    productsPage = new ProductsPage(userRole, currentUserId, this);
    stackedWidget->addWidget(productsPage);

    ordersPage = new OrdersPage(userRole, currentUserId, this);
    stackedWidget->addWidget(ordersPage);
    ordersPageIndex = stackedWidget->indexOf(ordersPage);

    PaymentsPage *paymentsPage = new PaymentsPage(this);
    stackedWidget->addWidget(paymentsPage);

    if (userRole != "VENDEUR") {
        CashPage *cashPage = new CashPage(this);
        stackedWidget->addWidget(cashPage);
    }

    connect(sidebar, &Sidebar::pageChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
    connect(sidebar, &Sidebar::logoutRequested, this, &MainWindow::onLogoutRequested);
    connect(stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::onPageChanged);
    connect(productsPage, &ProductsPage::orderValidated, ordersPage, &OrdersPage::loadOrders);
    connect(productsPage, &ProductsPage::orderValidated, productsPage, &ProductsPage::loadProducts);

    ThemeManager& themeManager = ThemeManager::instance();
    connect(&themeManager, &ThemeManager::themeChanged, this, &MainWindow::onThemeChanged);

    // Appliquer le thème initial
    applyTheme();
}

void MainWindow::onLogoutRequested()
{
    emit logoutRequested();
    close();
}

void MainWindow::onPageChanged(int index)
{
    if (index == ordersPageIndex) {
        ordersPage->loadOrders();
    }
}

void MainWindow::onThemeToggled()
{
    applyThemeToAllPages();
}

void MainWindow::onThemeChanged(ThemeManager::Theme theme)
{
    applyThemeToAllPages();
}

void MainWindow::applyTheme()
{
    ThemeManager& themeManager = ThemeManager::instance();
    
    QString completeStyle = themeManager.getCompleteStylesheet();
    qApp->setStyleSheet(completeStyle);
    
    setStyleSheet(QString(
        "QMainWindow, QDialog, QWidget, QFrame {"
        "   background: %1;"
        "   color: %2;"
        "   font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;"
        "}"
    ).arg(themeManager.backgroundColor().name(), 
          themeManager.textColor().name()));
    
    update();
    qApp->processEvents();
}

void MainWindow::applyThemeToAllPages()
{
    applyTheme();
    
    if (sidebar) {
        sidebar->updateTheme();
    }
    
    if (clientsPage) {
        clientsPage->onThemeChanged();
    }
    if (productsPage) {
        productsPage->loadProducts();
    }
    if (ordersPage) {
        ordersPage->loadOrders();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
