#ifndef CHARTSWIDGET_H
#define CHARTSWIDGET_H

#include <QFrame>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarSet>
#include "chartsmanager.h"

class ChartsWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ChartsWidget(QWidget *parent = nullptr);

    // Charts ADMIN
    void showSalesMonthlyChart();
    void showTopProductsChart();
    void showPaymentStatusChart();
    void showRevenueByVendorChart();
    void showOrdersDistributionChart();

    // Charts VENDEUR
    void showVendorSalesChart(int vendorId);
    void showVendorProductsChart(int vendorId);
    void showVendorClientsChart(int vendorId);
    void showVendorPaymentChart(int vendorId);

private:
    QChartView* createChartView();
    void setupStyles();
};

#endif // CHARTSWIDGET_H
