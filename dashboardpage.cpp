#include "dashboardpage.h"
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsBlurEffect>

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
    setProperty("colorClass", stats.colorClass);
    
    // Glassmorphism effect
    GlassmorphismEffect *glassEffect = new GlassmorphismEffect(this);
    glassEffect->setTint(QColor(255, 255, 255, 15));
    setGraphicsEffect(glassEffect);
    
    // Drop shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(25);
    shadow->setXOffset(0);
    shadow->setYOffset(8);
    shadow->setColor(QColor(0, 0, 0, 50));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(24, 24, 24, 24);

    QLabel *iconLabel = new QLabel(stats.icon, this);
    iconLabel->setObjectName("dashboardIcon");
    iconLabel->setAlignment(Qt::AlignLeft);
    iconLabel->setStyleSheet("color: white;");
    QFont iconFont = iconLabel->font();
    iconFont.setPointSize(32);
    iconLabel->setFont(iconFont);

    layout->addWidget(iconLabel, 0, Qt::AlignLeft);
    layout->addSpacing(16);

    QLabel *titleLabel = new QLabel(stats.title, this);
    titleLabel->setObjectName("dashboardCardTitle");
    titleLabel->setStyleSheet("color: rgba(255, 255, 255, 0.8); font-weight: 500;");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setWeight(QFont::Medium);
    titleLabel->setFont(titleFont);

    QLabel *valueLabel = new QLabel(stats.value, this);
    valueLabel->setObjectName("dashboardCardValue");
    valueLabel->setStyleSheet("color: white; font-weight: bold;");
    QFont valueFont = valueLabel->font();
    valueFont.setPointSize(36);
    valueFont.setBold(true);
    valueLabel->setFont(valueFont);

    layout->addWidget(titleLabel, 0, Qt::AlignLeft);
    layout->addWidget(valueLabel, 0, Qt::AlignLeft);
    layout->addStretch();
}

DashboardPage::DashboardPage(QWidget *parent) : QFrame(parent)
{
    setObjectName("dashboardPage");
    setupUI();
}

void DashboardPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(28);
    mainLayout->setContentsMargins(32, 32, 32, 32);

    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(8);
    
    QLabel *title = new QLabel("Tableau de Bord", this);
    title->setObjectName("titleH1");
    QFont titleFont = title->font();
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    title->setFont(titleFont);
    
    QLabel *subtitle = new QLabel("Realtime Message Data", this);
    subtitle->setObjectName("subtitle");
    QFont subtitleFont = subtitle->font();
    subtitleFont.setPointSize(14);
    subtitle->setFont(subtitleFont);
    
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addLayout(headerLayout);

    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);

    // Utiliser les données statiques
    QList<DashboardStats> stats = getStaticData();
    for (const DashboardStats &stat : stats) {
        DashboardCard *card = new DashboardCard(stat, this);
        cardsLayout->addWidget(card);
    }

    mainLayout->addLayout(cardsLayout);

    QFrame *analyticsFrame = new QFrame(this);
    analyticsFrame->setObjectName("analyticsPanel");
    
    QGraphicsDropShadowEffect *panelShadow = new QGraphicsDropShadowEffect(this);
    panelShadow->setBlurRadius(25);
    panelShadow->setXOffset(0);
    panelShadow->setYOffset(6);
    panelShadow->setColor(QColor(0, 0, 0, 30));
    analyticsFrame->setGraphicsEffect(panelShadow);
    
    QVBoxLayout *analyticsLayout = new QVBoxLayout(analyticsFrame);
    analyticsLayout->setContentsMargins(32, 32, 32, 32);
    analyticsLayout->setSpacing(24);
    
    QLabel *analyticsTitle = new QLabel("Statistiques Récentes", this);
    analyticsTitle->setObjectName("titleH2");
    QFont analyticsTitleFont = analyticsTitle->font();
    analyticsTitleFont.setPointSize(20);
    analyticsTitleFont.setBold(true);
    analyticsTitle->setFont(analyticsTitleFont);
    analyticsLayout->addWidget(analyticsTitle);
    
    // Créer le chart
    QWidget *chartWidget = createChartPlaceholder();
    analyticsLayout->addWidget(chartWidget, 1);

    mainLayout->addWidget(analyticsFrame, 1);
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
    QFrame *chartFrame = new QFrame(this);
    chartFrame->setObjectName("chartFrame");
    chartFrame->setStyleSheet(
        "#chartFrame { "
        "background: rgba(30, 41, 59, 0.9); "
        "border: 1px solid rgba(100, 116, 139, 0.5); "
        "border-radius: 12px; "
        "}"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(chartFrame);
    layout->setSpacing(20);
    layout->setContentsMargins(40, 40, 40, 40);
    
    QLabel *label = new QLabel("📊 Histogramme - Données Statiques", chartFrame);
    label->setAlignment(Qt::AlignCenter);
    QFont labelFont = label->font();
    labelFont.setPointSize(16);
    labelFont.setBold(true);
    label->setFont(labelFont);
    label->setStyleSheet("color: white;");
    
    QLabel *descLabel = new QLabel("À venir avec Qt Charts", chartFrame);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setStyleSheet("color: rgba(203, 213, 225, 0.7); font-size: 13px;");
    
    layout->addStretch();
    layout->addWidget(label);
    layout->addWidget(descLabel);
    layout->addStretch();
    
    chartFrame->setMinimumHeight(500);
    
    return chartFrame;
}
