#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thememanager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QApplication>
#include <QPalette>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QFile>
#include <QPainter>
#include <QPainterPath>
#include "dashboardpage.h"
#include "userspage.h"
#include "clientspage.h"
#include "productspage.h"
#include "orderspage.h"
#include "paymentspage.h"
#include "stockmovementpage.h"
#include "profilepanel.h"

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

    // Créer le layout principal (sidebar + zone droite)
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Créer la sidebar avec le rôle de l'utilisateur
    sidebar = new Sidebar(userRole, this);
    mainLayout->addWidget(sidebar);

    // Zone droite : topbar + contenu (stackedWidget)
    QWidget *rightContainer = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(16, 16, 16, 16);

    // Top bar
    topBar = new QWidget(this);
    topBar->setObjectName("topBar");
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(8, 8, 8, 8);

    topTitleLabel = new QLabel(tr("Gestion Vente"), this);
    QFont titleFont = topTitleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    topTitleLabel->setFont(titleFont);

    // Spacer then profile button on the right
    topLayout->addWidget(topTitleLabel);
    topLayout->addStretch(1);

    profileButton = new QToolButton(this);
    profileButton->setObjectName("profileButton");
    profileButton->setAutoRaise(true);
    profileButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    profileButton->setCursor(Qt::PointingHandCursor);
    profileButton->setFixedSize(44, 44);
    
    // Charger la photo de profil depuis la BD
    QSqlQuery query;
    query.prepare("SELECT photo_profile FROM USERS WHERE id_user = ?");
    query.addBindValue(userId);
    
    QPixmap avatarPixmap;
    if (query.exec() && query.next()) {
        QString photoPath = query.value(0).toString();
        if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
            avatarPixmap.load(photoPath);
        }
    }
    
    // Si pas de photo, utiliser l'avatar par défaut
    if (avatarPixmap.isNull()) {
        avatarPixmap.load(":/images/avatar.svg");
    }
    
    // Créer une image circulaire avec bordure
    QPixmap roundedPixmap(44, 44);
    roundedPixmap.fill(Qt::transparent);
    
    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    // Créer le chemin circulaire
    QPainterPath path;
    path.addEllipse(1, 1, 42, 42);
    painter.setClipPath(path);
    
    // Redimensionner et dessiner l'image
    QPixmap scaledPixmap = avatarPixmap.scaledToWidth(42, Qt::SmoothTransformation);
    int offset = (scaledPixmap.height() - 42) / 2;
    painter.drawPixmap(1, 1 - offset, scaledPixmap);
    
    // Dessiner la bordure
    painter.setClipping(false);
    painter.setPen(QPen(QColor(59, 130, 246), 2));
    painter.drawEllipse(1, 1, 42, 42);
    painter.end();
    
    profileButton->setIcon(QIcon(roundedPixmap));
    profileButton->setIconSize(QSize(44, 44));

    // Menu du profil
    QMenu *profileMenu = new QMenu(this);
    QAction *viewProfile = new QAction(tr("Profil"), this);
    QAction *logoutAction = new QAction(tr("Déconnexion"), this);
    profileMenu->addAction(viewProfile);
    profileMenu->addSeparator();
    profileMenu->addAction(logoutAction);

    connect(viewProfile, &QAction::triggered, this, &MainWindow::onProfileRequested);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::onLogoutRequested);

    profileButton->setMenu(profileMenu);
    profileButton->setPopupMode(QToolButton::InstantPopup);

    topLayout->addWidget(profileButton);

    rightLayout->addWidget(topBar, 0);

    // Créer le stacked widget pour les pages
    stackedWidget = new QStackedWidget(this);
    rightLayout->addWidget(stackedWidget, 1);

    mainLayout->addWidget(rightContainer, 1);

    // Ajouter le Dashboard en premier
    DashboardPage *dashboardPage = new DashboardPage(this);
    dashboardPage->setUserRole(userRole, userId);  // Passer le rôle et l'ID utilisateur
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

    StockMovementPage *stockMovementPage = new StockMovementPage(userRole, currentUserId, this);
    stackedWidget->addWidget(stockMovementPage);

    // Ajouter le panel de profil
    profilePanel = new ProfilePanel(currentUserId, this);
    profilePageIndex = stackedWidget->addWidget(profilePanel);
    
    // Connexions des signaux
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
    } else if (index == profilePageIndex) {
        profilePanel->loadUserData();
    }
}

void MainWindow::onProfileRequested()
{
    stackedWidget->setCurrentIndex(profilePageIndex);
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
    // Style top bar elements if présents
    if (topBar) {
        topBar->setStyleSheet(QString("background: transparent; color: %1;").arg(themeManager.textColor().name()));
    }
    if (profileButton) {
        QString btnStyle = QString(
            "QToolButton#profileButton {"
            "  border-radius: 22px;"
            "  background: %1;"
            "  border: 2px solid %2;"
            "}"
        ).arg(themeManager.primaryColor().name(), themeManager.borderColor().name());
        profileButton->setStyleSheet(btnStyle);
    }
    
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
