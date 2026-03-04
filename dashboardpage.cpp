#include "dashboardpage.h"
#include "thememanager.h"
#include "chartsmanager.h"
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsBlurEffect>
#include <QScrollArea>
#include <QTabWidget>
#include <QDebug>

GlassmorphismEffect::GlassmorphismEffect(QObject *parent)
    : QGraphicsEffect(parent)
{
}

void GlassmorphismEffect::draw(QPainter *painter)
{
    QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates);
    
    // Create a semi-transparent overlay for glassmorphism effect
    pixmap.fill(Qt::transparent);
    
    QPainter p(&pixmap);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    
    // Draw the source with tint
    drawSource(painter);
    
    // Apply tint overlay
    painter->fillRect(boundingRect(), m_tintColor);
}

DashboardCard::DashboardCard(const DashboardStats &stats, QWidget *parent)
    : QFrame(parent)
{
    setObjectName("dashboardCard");
    
    // Define color mapping for different card types
    QMap<QString, QColor> colorMap;
    colorMap["cyan"] = QColor(16, 185, 129);   // Vert/Cyan
    colorMap["blue"] = QColor(59, 130, 246);   // Bleu
    colorMap["pink"] = QColor(244, 114, 182);  // Rose
    colorMap["red"] = QColor(239, 68, 68);     // Rouge
    colorMap["orange"] = QColor(249, 115, 22); // Orange
    
    QColor bgColor = colorMap.value(stats.colorClass, QColor(100, 116, 139));
    
    // Set gradient background
    QString gradientStyle = QString(
        "QFrame { "
        "   background-color: %1; "
        "   border-radius: 12px; "
        "   border: none; "
        "}"
    ).arg(bgColor.name());
    
    setStyleSheet(gradientStyle);
    setMinimumHeight(140);
    setMaximumHeight(180);
    
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(8);
    shadow->setColor(QColor(0, 0, 0, 80));
    setGraphicsEffect(shadow);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(20, 16, 20, 16);
    
    // Top row: Icon and Title
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(12);
    topLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *iconLabel = new QLabel(stats.icon, this);
    iconLabel->setObjectName("dashboardIcon");
    iconLabel->setAlignment(Qt::AlignCenter);
    QFont iconFont = iconLabel->font();
    iconFont.setPointSize(24);
    iconLabel->setFont(iconFont);
    iconLabel->setFixedSize(45, 45);
    
    QLabel *titleLabel = new QLabel(stats.title, this);
    titleLabel->setObjectName("dashboardCardTitle");
    titleLabel->setStyleSheet("color: white; font-weight: 600; font-size: 12px;");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    topLayout->addWidget(iconLabel);
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    
    // Value row
    QLabel *valueLabel = new QLabel(stats.value, this);
    valueLabel->setObjectName("dashboardCardValue");
    valueLabel->setStyleSheet("color: white; font-weight: 700; font-size: 22px;");
    valueLabel->setAlignment(Qt::AlignLeft);
    
    // Trend row
    QLabel *trendLabel = new QLabel(stats.trend, this);
    trendLabel->setObjectName("dashboardCardTrend");
    QString trendColor = (stats.trend.startsWith("+")) ? "#10b981" : "#ef4444";
    trendLabel->setStyleSheet(QString("color: %1; font-weight: 600; font-size: 11px;").arg(trendColor));
    trendLabel->setAlignment(Qt::AlignLeft);
    
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(valueLabel);
    mainLayout->addWidget(trendLabel);
    mainLayout->addStretch();
}

DashboardPage::DashboardPage(QWidget *parent) : QFrame(parent)
{
    setObjectName("dashboardPage");
    userRole = "VENDEUR";  // Default role
    currentUserId = -1;
    setupUI();
}

void DashboardPage::setUserRole(const QString& role, int userId)
{
    userRole = role;
    currentUserId = userId;
    
    // Clear existing layout
    QLayout *oldLayout = this->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
    }
    
    // Tous les utilisateurs voient le même dashboard avec les données globales
    setupAdminDashboard();
}

void DashboardPage::setupUI()
{
    // Setup dashboard with global company data for all users
    setupAdminDashboard();
}

void DashboardPage::setupAdminDashboard()
{
    qDebug() << "Affichage du Dashboard pour tous les utilisateurs";
    
    // Create scroll area
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    
    // Create container widget for scroll area
    QWidget *container = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setSpacing(32);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Header
    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(6);
    
    QLabel *title = new QLabel("Tableau de Bord - Vue Générale", this);
    title->setObjectName("titleH1");
    QFont titleFont = title->font();
    titleFont.setPointSize(36);
    titleFont.setBold(true);
    titleFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    title->setFont(titleFont);
    
    QLabel *subtitle = new QLabel("Données globales de l'ensemble de l'entreprise", this);
    subtitle->setObjectName("subtitle");
    ThemeManager& theme = ThemeManager::instance();
    subtitle->setStyleSheet(QString("color: %1;").arg(theme.textSecondaryColor().name()));
    QFont subtitleFont = subtitle->font();
    subtitleFont.setPointSize(13);
    subtitle->setFont(subtitleFont);
    
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addLayout(headerLayout);

    // Stats Cards
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(24);

    double totalRevenue = ChartsManager::getTotalRevenue();
    int totalOrders = ChartsManager::getTotalOrders();
    int totalClients = ChartsManager::getTotalClients();
    int totalProducts = ChartsManager::getTotalProducts();
    
    DashboardStats stats[] = {
        {"Chiffre d'Affaires", QString::number((long long)totalRevenue) + " Ar", "💰", "cyan", "+12%", "green"},
        {"Commandes", QString::number(totalOrders), "📋", "blue", "+5%", "green"},
        {"Clients", QString::number(totalClients), "🤝", "orange", "+8%", "green"},
        {"Produits", QString::number(totalProducts), "📦", "pink", "+3%", "green"}
    };
    
    for (int i = 0; i < 4; ++i) {
        DashboardCard *card = new DashboardCard(stats[i], this);
        card->setMinimumWidth(160);
        card->setMaximumWidth(280);
        cardsLayout->addWidget(card, 1);
    }

    QWidget *cardsWidget = new QWidget(this);
    cardsWidget->setLayout(cardsLayout);
    mainLayout->addWidget(cardsWidget);

    // Charts Grid Layout
    QGridLayout *chartsGridLayout = new QGridLayout();
    chartsGridLayout->setSpacing(24);
    
    // Create chart widgets in grid (2 columns)
    chartsWidget = new ChartsWidget(this);
    chartsWidget->setMinimumHeight(250);
    chartsWidget->showSalesMonthlyChart();
    chartsGridLayout->addWidget(chartsWidget, 0, 0);
    
    ChartsWidget *productsChart = new ChartsWidget(this);
    productsChart->setMinimumHeight(250);
    productsChart->showTopProductsChart();
    chartsGridLayout->addWidget(productsChart, 0, 1);
    
    ChartsWidget *paymentChart = new ChartsWidget(this);
    paymentChart->setMinimumHeight(250);
    paymentChart->showPaymentStatusChart();
    chartsGridLayout->addWidget(paymentChart, 1, 0);
    
    ChartsWidget *vendorChart = new ChartsWidget(this);
    vendorChart->setMinimumHeight(250);
    vendorChart->showRevenueByVendorChart();
    chartsGridLayout->addWidget(vendorChart, 1, 1);
    
    mainLayout->addLayout(chartsGridLayout, 1);
    
    // Set container in scroll area and add to frame
    scrollArea->setWidget(container);
    
    QVBoxLayout *frameLayout = new QVBoxLayout(this);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->setSpacing(0);
    frameLayout->addWidget(scrollArea);
    this->setLayout(frameLayout);
}

void DashboardPage::setupVendorDashboard()
{
    qDebug() << "Affichage du Dashboard VENDEUR - ID:" << currentUserId;
    
    // Create scroll area
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    
    // Create container widget for scroll area
    QWidget *container = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setSpacing(32);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Header
    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(6);
    
    QLabel *title = new QLabel("Mon Tableau de Bord", this);
    title->setObjectName("titleH1");
    QFont titleFont = title->font();
    titleFont.setPointSize(36);
    titleFont.setBold(true);
    titleFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    title->setFont(titleFont);
    
    QLabel *subtitle = new QLabel("Suivi de vos ventes et performances", this);
    subtitle->setObjectName("subtitle");
    ThemeManager& theme = ThemeManager::instance();
    subtitle->setStyleSheet(QString("color: %1;").arg(theme.textSecondaryColor().name()));
    QFont subtitleFont = subtitle->font();
    subtitleFont.setPointSize(13);
    subtitle->setFont(subtitleFont);
    
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addLayout(headerLayout);

    // Stats Cards for Vendeur
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(24);

    auto vendorSalesData = ChartsManager::getVendorSalesData(currentUserId);
    double vendorRevenue = 0;
    for (const auto& point : vendorSalesData) {
        vendorRevenue += point.value;
    }
    
    DashboardStats stats[] = {
        {"Mes Ventes", QString::number((long long)vendorRevenue) + " Ar", "💵", "cyan", "+10%", "green"},
        {"Mes Commandes", QString::number(ChartsManager::getTotalOrders() / 3), "📋", "blue", "+4%", "green"},
        {"Mes Clients", QString::number(ChartsManager::getTotalClients() / 2), "👥", "orange", "+6%", "green"},
        {"Non Payées", QString::number(ChartsManager::getUnpaidOrders()), "⚠️", "red", "-2%", "red"}
    };
    
    for (int i = 0; i < 4; ++i) {
        DashboardCard *card = new DashboardCard(stats[i], this);
        card->setMinimumWidth(160);
        card->setMaximumWidth(280);
        cardsLayout->addWidget(card, 1);
    }

    QWidget *cardsWidget = new QWidget(this);
    cardsWidget->setLayout(cardsLayout);
    mainLayout->addWidget(cardsWidget);

    // Charts Grid Layout
    QGridLayout *chartsGridLayout = new QGridLayout();
    chartsGridLayout->setSpacing(24);
    
    // Create chart widgets for vendor in grid (2 columns)
    chartsWidget = new ChartsWidget(this);
    chartsWidget->setMinimumHeight(400);
    chartsWidget->showVendorSalesChart(currentUserId);
    chartsGridLayout->addWidget(chartsWidget, 0, 0);
    
    ChartsWidget *productsChart = new ChartsWidget(this);
    productsChart->setMinimumHeight(400);
    productsChart->showVendorProductsChart(currentUserId);
    chartsGridLayout->addWidget(productsChart, 0, 1);
    
    ChartsWidget *clientsChart = new ChartsWidget(this);
    clientsChart->setMinimumHeight(400);
    clientsChart->showVendorClientsChart(currentUserId);
    chartsGridLayout->addWidget(clientsChart, 1, 0);
    
    ChartsWidget *paymentChart = new ChartsWidget(this);
    paymentChart->setMinimumHeight(400);
    paymentChart->showVendorPaymentChart(currentUserId);
    chartsGridLayout->addWidget(paymentChart, 1, 1);
    
    mainLayout->addLayout(chartsGridLayout, 1);
    
    // Set container in scroll area and add to frame
    scrollArea->setWidget(container);
    
    QVBoxLayout *frameLayout = new QVBoxLayout(this);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->setSpacing(0);
    frameLayout->addWidget(scrollArea);
    this->setLayout(frameLayout);
}

QList<DashboardStats> DashboardPage::getStaticData()
{
    return QList<DashboardStats>{
        {
            "Utilisateurs",
            "12",
            "👥",
            "purple",
            "+2",
            "green"
        },
        {
            "Clients",
            "45",
            "🤝",
            "orange",
            "+8",
            "green"
        },
        {
            "Produits",
            "128",
            "📦",
            "pink",
            "+15",
            "green"
        },
        {
            "Commandes",
            "23",
            "📋",
            "cyan",
            "+5",
            "green"
        }
    };
}

QWidget* DashboardPage::createChartPlaceholder()
{
    ThemeManager& theme = ThemeManager::instance();
    
    QFrame *chartFrame = new QFrame(this);
    chartFrame->setObjectName("chartFrame");
    
    QString bgColor = (theme.currentTheme() == ThemeManager::LightMode) 
        ? theme.surfaceColor().name()
        : theme.surfaceAltColor().name();
    QString borderColor = theme.borderColor().name();
    QString textColor = theme.textColor().name();
    QString secondaryText = theme.textSecondaryColor().name();
    
    chartFrame->setStyleSheet(
        QString("#chartFrame { "
        "background: %1; "
        "border: 1px solid %2; "
        "border-radius: 16px; "
        "padding: 0px; "
        "}").arg(bgColor, borderColor)
    );
    
    QVBoxLayout *layout = new QVBoxLayout(chartFrame);
    layout->setSpacing(24);
    layout->setContentsMargins(48, 48, 48, 48);
    
    QLabel *label = new QLabel("📊 Graphique Analytique", chartFrame);
    label->setAlignment(Qt::AlignCenter);
    QFont labelFont = label->font();
    labelFont.setPointSize(18);
    labelFont.setBold(true);
    labelFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.4);
    label->setFont(labelFont);
    label->setStyleSheet(QString("color: %1;").arg(textColor));
    
    QLabel *descLabel = new QLabel("Graphiques détaillés à venir avec Qt Charts", chartFrame);
    descLabel->setAlignment(Qt::AlignCenter);
    QFont descFont = descLabel->font();
    descFont.setPointSize(12);
    descLabel->setFont(descFont);
    descLabel->setStyleSheet(QString("color: %1; line-height: 1.6;").arg(secondaryText));
    
    layout->addStretch();
    layout->addWidget(label);
    layout->addWidget(descLabel);
    layout->addStretch();
    
    chartFrame->setMinimumHeight(520);
    
    return chartFrame;
}

void DashboardPage::refreshDashboard()
{
    setUserRole(userRole, currentUserId);
}

void DashboardPage::onDataChanged()
{
    refreshDashboard();
}
