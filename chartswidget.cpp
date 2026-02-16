#include "chartswidget.h"
#include "thememanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QFont>
#include <QDebug>

ChartsWidget::ChartsWidget(QWidget *parent)
    : QFrame(parent)
{
    setObjectName("chartsWidget");
    setupStyles();
}

void ChartsWidget::setupStyles()
{
    ThemeManager& theme = ThemeManager::instance();
    QString bgColor = (theme.currentTheme() == ThemeManager::LightMode)
        ? theme.surfaceColor().name()
        : theme.surfaceAltColor().name();
    
    setStyleSheet(
        QString("#chartsWidget {"
        "background: %1;"
        "border: none;"
        "border-radius: 16px;"
        "}").arg(bgColor)
    );
}

QChartView* ChartsWidget::createChartView()
{
    QChartView* chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    chartView->setMinimumHeight(550);
    
    return chartView;
}

void ChartsWidget::showSalesMonthlyChart()
{
    QChartView *chartView = createChartView();
    
    QChart *chart = new QChart();
    chart->setTitle("Ventes Mensuelles (12 derniers mois)");
    QFont titleFont("Segoe UI", 18, QFont::Bold);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(QColor(241, 245, 249)));
    chart->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    
    QBarSet *set = new QBarSet("Ventes");
    set->setColor(QColor(59, 130, 246));
    
    QStringList categories;
    
    auto data = ChartsManager::getSalesDataByMonth();
    for (const auto& point : data) {
        set->append(point.value);
        categories << point.label;
    }
    
    QBarSeries *series = new QBarSeries();
    series->append(set);
    chart->addSeries(series);
    
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor(241, 245, 249));
    axisX->setLabelsFont(QFont("Segoe UI", 11));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelsColor(QColor(241, 245, 249));
    axisY->setLabelsFont(QFont("Segoe UI", 11));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(QColor(241, 245, 249));
    
    chartView->setChart(chart);
    
    // Remplacer le contenu du widget
    QLayout *layout = this->layout();
    if (layout) delete layout;
    
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(12);
    newLayout->addWidget(chartView);
    this->setLayout(newLayout);
}

void ChartsWidget::showTopProductsChart()
{
    QChartView *chartView = createChartView();
    
    QChart *chart = new QChart();
    chart->setTitle("Top 10 Produits Vendus");
    QFont titleFont2("Segoe UI", 18, QFont::Bold);
    chart->setTitleFont(titleFont2);
    chart->setTitleBrush(QBrush(QColor(241, 245, 249)));
    chart->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    
    QPieSeries *series = new QPieSeries();
    
    auto data = ChartsManager::getTopProducts();
    for (const auto& point : data) {
        QPieSlice *slice = series->append(point.label, point.value);
        slice->setColor(point.color);
        slice->setLabelVisible(true);
        slice->setLabelColor(QColor(241, 245, 249));
        slice->setLabel(QString("%1 (%2)").arg(point.label).arg((int)point.value));
    }
    
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setLabelColor(QColor(241, 245, 249));
    
    chartView->setChart(chart);
    
    QLayout *layout = this->layout();
    if (layout) delete layout;
    
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(12);
    newLayout->addWidget(chartView);
    this->setLayout(newLayout);
}

void ChartsWidget::showPaymentStatusChart()
{
    QChartView *chartView = createChartView();
    
    QChart *chart = new QChart();
    chart->setTitle("Statut des Commandes");
    QFont titleFont3("Segoe UI", 18, QFont::Bold);
    chart->setTitleFont(titleFont3);
    chart->setTitleBrush(QBrush(QColor(241, 245, 249)));
    chart->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    
    QPieSeries *series = new QPieSeries();
    
    auto data = ChartsManager::getPaymentStatus();
    for (const auto& point : data) {
        QPieSlice *slice = series->append(point.label, point.value);
        slice->setColor(point.color);
        slice->setLabelVisible(true);
        slice->setLabelColor(QColor(241, 245, 249));
        slice->setLabel(QString("%1: %2").arg(point.label).arg((int)point.value));
    }
    
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setLabelColor(QColor(241, 245, 249));
    
    chartView->setChart(chart);
    
    QLayout *layout = this->layout();
    if (layout) delete layout;
    
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(12);
    newLayout->addWidget(chartView);
    this->setLayout(newLayout);
}

void ChartsWidget::showRevenueByVendorChart()
{
    QChartView *chartView = createChartView();
    
    QChart *chart = new QChart();
    chart->setTitle("Revenus par Vendeur");
    QFont titleFont4("Segoe UI", 18, QFont::Bold);
    chart->setTitleFont(titleFont4);
    chart->setTitleBrush(QBrush(QColor(241, 245, 249)));
    chart->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    
    QPieSeries *series = new QPieSeries();
    
    auto data = ChartsManager::getRevenueByVendor();
    for (const auto& point : data) {
        QPieSlice *slice = series->append(point.label, point.value);
        slice->setColor(point.color);
        slice->setLabelVisible(true);
        slice->setLabelColor(QColor(241, 245, 249));
        slice->setLabel(QString("%1\n%2 Ar").arg(point.label).arg((long long)point.value));
    }
    
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setLabelColor(QColor(241, 245, 249));
    
    chartView->setChart(chart);
    
    QLayout *layout = this->layout();
    if (layout) delete layout;
    
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(12);
    newLayout->addWidget(chartView);
    this->setLayout(newLayout);
}

void ChartsWidget::showOrdersDistributionChart()
{
    QChartView *chartView = createChartView();
    
    QChart *chart = new QChart();
    chart->setTitle("Distribution des Commandes");
    QFont titleFont5("Segoe UI", 18, QFont::Bold);
    chart->setTitleFont(titleFont5);
    chart->setTitleBrush(QBrush(QColor(241, 245, 249)));
    chart->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    
    QBarSet *set = new QBarSet("Nombre de commandes");
    set->setColor(QColor(139, 92, 246));
    
    QStringList categories;
    auto data = ChartsManager::getOrdersStatusDistribution();
    for (const auto& point : data) {
        set->append(point.value);
        categories << point.label;
    }
    
    QBarSeries *series = new QBarSeries();
    series->append(set);
    chart->addSeries(series);
    
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor(241, 245, 249));
    axisX->setLabelsFont(QFont("Segoe UI", 11));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelsColor(QColor(241, 245, 249));
    axisY->setLabelsFont(QFont("Segoe UI", 11));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chartView->setChart(chart);
    
    QLayout *layout = this->layout();
    if (layout) delete layout;
    
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(12);
    newLayout->addWidget(chartView);
    this->setLayout(newLayout);
}

// ============= CHARTS VENDEUR =============

void ChartsWidget::showVendorSalesChart(int vendorId)
{
    QChartView *chartView = createChartView();
    
    QChart *chart = new QChart();
    chart->setTitle("Mes Ventes (6 derniers mois)");
    QFont titleFont6("Segoe UI", 18, QFont::Bold);
    chart->setTitleFont(titleFont6);
    chart->setTitleBrush(QBrush(QColor(241, 245, 249)));
    chart->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    
    QBarSet *set = new QBarSet("Ventes");
    set->setColor(QColor(6, 182, 212));
    
    QStringList categories;
    auto data = ChartsManager::getVendorSalesData(vendorId);
    for (const auto& point : data) {
        set->append(point.value);
        categories << point.label;
    }
    
    QBarSeries *series = new QBarSeries();
    series->append(set);
    chart->addSeries(series);
    
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor(241, 245, 249));
    axisX->setLabelsFont(QFont("Segoe UI", 11));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelsColor(QColor(241, 245, 249));
    axisY->setLabelsFont(QFont("Segoe UI", 11));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chartView->setChart(chart);
    
    QLayout *layout = this->layout();
    if (layout) delete layout;
    
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(12);
    newLayout->addWidget(chartView);
    this->setLayout(newLayout);
}

void ChartsWidget::showVendorProductsChart(int vendorId)
{
    QChartView *chartView = createChartView();
    
    QChart *chart = new QChart();
    chart->setTitle("Mes Produits Populaires");
    QFont titleFont7("Segoe UI", 18, QFont::Bold);
    chart->setTitleFont(titleFont7);
    chart->setTitleBrush(QBrush(QColor(241, 245, 249)));
    chart->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    
    QPieSeries *series = new QPieSeries();
    
    auto data = ChartsManager::getVendorTopProducts(vendorId);
    for (const auto& point : data) {
        QPieSlice *slice = series->append(point.label, point.value);
        slice->setColor(point.color);
        slice->setLabelVisible(true);
        slice->setLabelColor(QColor(241, 245, 249));
        slice->setLabel(QString("%1 (%2)").arg(point.label).arg((int)point.value));
    }
    
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setLabelColor(QColor(241, 245, 249));
    
    chartView->setChart(chart);
    
    QLayout *layout = this->layout();
    if (layout) delete layout;
    
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(12);
    newLayout->addWidget(chartView);
    this->setLayout(newLayout);
}

void ChartsWidget::showVendorClientsChart(int vendorId)
{
    QChartView *chartView = createChartView();
    
    QChart *chart = new QChart();
    chart->setTitle("Fidélité de mes Clients");
    QFont titleFont8("Segoe UI", 18, QFont::Bold);
    chart->setTitleFont(titleFont8);
    chart->setTitleBrush(QBrush(QColor(241, 245, 249)));
    chart->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    
    QPieSeries *series = new QPieSeries();
    
    auto data = ChartsManager::getVendorClientsFidelity(vendorId);
    for (const auto& point : data) {
        QPieSlice *slice = series->append(point.label, point.value);
        slice->setColor(point.color);
        slice->setLabelVisible(true);
        slice->setLabelColor(QColor(241, 245, 249));
        slice->setLabel(QString("%1: %2").arg(point.label).arg((int)point.value));
    }
    
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setLabelColor(QColor(241, 245, 249));
    
    chartView->setChart(chart);
    
    QLayout *layout = this->layout();
    if (layout) delete layout;
    
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(12);
    newLayout->addWidget(chartView);
    this->setLayout(newLayout);
}

void ChartsWidget::showVendorPaymentChart(int vendorId)
{
    QChartView *chartView = createChartView();
    
    QChart *chart = new QChart();
    chart->setTitle("Statut de mes Ventes");
    QFont titleFont9("Segoe UI", 18, QFont::Bold);
    chart->setTitleFont(titleFont9);
    chart->setTitleBrush(QBrush(QColor(241, 245, 249)));
    chart->setBackgroundBrush(QBrush(QColor(15, 23, 42)));
    
    QPieSeries *series = new QPieSeries();
    
    auto data = ChartsManager::getVendorPaymentStats(vendorId);
    for (const auto& point : data) {
        QPieSlice *slice = series->append(point.label, point.value);
        slice->setColor(point.color);
        slice->setLabelVisible(true);
        slice->setLabelColor(QColor(241, 245, 249));
        slice->setLabel(QString("%1\n%2 Ar").arg(point.label).arg((long long)point.value));
    }
    
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setLabelColor(QColor(241, 245, 249));
    
    chartView->setChart(chart);
    
    QLayout *layout = this->layout();
    if (layout) delete layout;
    
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(12);
    newLayout->addWidget(chartView);
    this->setLayout(newLayout);
}
