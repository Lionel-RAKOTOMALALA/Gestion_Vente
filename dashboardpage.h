#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsEffect>
#include <QMap>

struct DashboardStats {
    QString title;
    QString value;
    QString icon;
    QString colorClass;
    QString trend;  // "+12%" ou "-5%"
    QString trendColor;  // "green" ou "red"
};

class GlassmorphismEffect : public QGraphicsEffect
{
    Q_OBJECT

public:
    explicit GlassmorphismEffect(QObject *parent = nullptr);
    void setBlurRadius(qreal radius) { m_blurRadius = radius; update(); }
    void setTint(const QColor &color) { m_tintColor = color; update(); }

protected:
    void draw(QPainter *painter) override;

private:
    qreal m_blurRadius = 10.0;
    QColor m_tintColor = QColor(255, 255, 255, 20);
};

class DashboardCard : public QFrame
{
    Q_OBJECT

public:
    explicit DashboardCard(const DashboardStats &stats, QWidget *parent = nullptr);
};

class DashboardPage : public QFrame
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);

private:
    void setupUI();
    QList<DashboardStats> getStaticData();
    QWidget* createChartPlaceholder();
};

#endif // DASHBOARDPAGE_H
