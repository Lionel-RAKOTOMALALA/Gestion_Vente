#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
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

    // Créer la sidebar avec le rôle de l'utilisateur
    sidebar = new Sidebar(userRole, this);
    mainLayout->addWidget(sidebar);

    // Créer le stacked widget pour les pages
    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget, 1); // Stretch factor 1

    // Ajouter le Dashboard en premier
    DashboardPage *dashboardPage = new DashboardPage(this);
    stackedWidget->addWidget(dashboardPage);

    // Ajouter les pages modulaires selon le rôle
    if (userRole != "VENDEUR") {
        UsersPage *usersPage = new UsersPage(this);
        stackedWidget->addWidget(usersPage);
    }

    ClientsPage *clientsPage = new ClientsPage(this);
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

    // Connecter la sidebar au stacked widget
    connect(sidebar, &Sidebar::pageChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
    connect(sidebar, &Sidebar::logoutRequested, this, &MainWindow::onLogoutRequested);

    // Connecter le changement de page pour actualiser les données
    connect(stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::onPageChanged);

    // Connecter la validation de commande depuis la page produits à l'actualisation de la page commandes
    connect(productsPage, &ProductsPage::orderValidated, ordersPage, &OrdersPage::loadOrders);
    // Et à l'actualisation de la page produits pour mettre à jour les stocks
    connect(productsPage, &ProductsPage::orderValidated, productsPage, &ProductsPage::loadProducts);
}

void MainWindow::onLogoutRequested()
{
    // Émettre le signal de déconnexion pour retourner à la page de login
    emit logoutRequested();
    // Fermer la fenêtre principale
    close();
}

void MainWindow::onPageChanged(int index)
{
    // Actualiser les données de la page commandes quand elle devient visible
    if (index == ordersPageIndex) {
        ordersPage->loadOrders();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
