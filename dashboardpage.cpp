#include "dashboardpage.h"
#include "thememanager.h"
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsBlurEffect>
#include <QGridLayout>

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
    
    ThemeManager& theme = ThemeManager::instance();
    
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(32);
    shadow->setXOffset(0);
    shadow->setYOffset(12);
    shadow->setColor(QColor(0, 0, 0, 60));
    setGraphicsEffect(shadow);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(14);
    layout->setContentsMargins(28, 28, 28, 28);

    QLabel *iconLabel = new QLabel(stats.icon, this);
    iconLabel->setObjectName("dashboardIcon");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet(QString("color: %1;").arg(theme.textColor().name()));
    QFont iconFont = iconLabel->font();
    iconFont.setPointSize(40);
    iconLabel->setFont(iconFont);
    iconLabel->setFixedSize(70, 70);

    layout->addWidget(iconLabel, 0, Qt::AlignCenter);

    QLabel *titleLabel = new QLabel(stats.title, this);
    titleLabel->setObjectName("dashboardCardTitle");
    titleLabel->setStyleSheet(QString("color: %1; font-weight: 600; letter-spacing: 0.5px;").arg(theme.textSecondaryColor().name()));
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(11);
    titleFont.setWeight(QFont::DemiBold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *valueLabel = new QLabel(stats.value, this);
    valueLabel->setObjectName("dashboardCardValue");
    valueLabel->setStyleSheet(QString("color: %1; font-weight: 700;").arg(theme.textColor().name()));
    QFont valueFont = valueLabel->font();
    valueFont.setPointSize(38);
    valueFont.setBold(true);
    valueLabel->setFont(valueFont);
    valueLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(titleLabel, 0, Qt::AlignCenter);
    layout->addWidget(valueLabel, 0, Qt::AlignCenter);
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
    mainLayout->setSpacing(32);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(6);
    
    QLabel *title = new QLabel("Tableau de Bord", this);
    title->setObjectName("titleH1");
    QFont titleFont = title->font();
    titleFont.setPointSize(36);
    titleFont.setBold(true);
    titleFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    title->setFont(titleFont);
    
    QLabel *subtitle = new QLabel("Aperçu en temps réel de vos métriques clés", this);
    subtitle->setObjectName("subtitle");
    ThemeManager& theme = ThemeManager::instance();
    subtitle->setStyleSheet(QString("color: %1;").arg(theme.textSecondaryColor().name()));
    QFont subtitleFont = subtitle->font();
    subtitleFont.setPointSize(13);
    subtitleFont.setWeight(QFont::Normal);
    subtitle->setFont(subtitleFont);
    
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addLayout(headerLayout);

    QGridLayout *cardsLayout = new QGridLayout();
    cardsLayout->setSpacing(24);
    cardsLayout->setRowStretch(0, 0);
    cardsLayout->setRowStretch(1, 0);

    QList<DashboardStats> stats = getStaticData();
    for (int i = 0; i < stats.size(); ++i) {
        DashboardCard *card = new DashboardCard(stats[i], this);
        card->setMinimumHeight(240);
        card->setMaximumHeight(280);
        cardsLayout->addWidget(card, i / 2, i % 2);
    }

    QWidget *cardsWidget = new QWidget(this);
    cardsWidget->setLayout(cardsLayout);
    mainLayout->addWidget(cardsWidget);

    QFrame *analyticsFrame = new QFrame(this);
    analyticsFrame->setObjectName("analyticsPanel");
    
    QGraphicsDropShadowEffect *panelShadow = new QGraphicsDropShadowEffect(this);
    panelShadow->setBlurRadius(28);
    panelShadow->setXOffset(0);
    panelShadow->setYOffset(10);
    panelShadow->setColor(QColor(0, 0, 0, 50));
    analyticsFrame->setGraphicsEffect(panelShadow);
    
    QVBoxLayout *analyticsLayout = new QVBoxLayout(analyticsFrame);
    analyticsLayout->setContentsMargins(40, 40, 40, 40);
    analyticsLayout->setSpacing(28);
    
    QLabel *analyticsTitle = new QLabel("Statistiques Récentes", this);
    analyticsTitle->setObjectName("titleH2");
    QFont analyticsTitleFont = analyticsTitle->font();
    analyticsTitleFont.setPointSize(20);
    analyticsTitleFont.setBold(true);
    analyticsTitleFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.3);
    analyticsTitle->setFont(analyticsTitleFont);
    analyticsTitle->setStyleSheet(QString("color: %1;").arg(theme.textColor().name()));
    analyticsLayout->addWidget(analyticsTitle);
    
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
