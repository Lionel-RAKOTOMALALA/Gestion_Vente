#ifndef CHARTSMANAGER_H
#define CHARTSMANAGER_H

#include <QString>
#include <QList>
#include <QPair>
#include <QColor>

struct ChartDataPoint {
    QString label;
    double value;
    QColor color;
};

class ChartsManager
{
public:
    ChartsManager();

    // Données pour les charts ADMIN
    static QList<ChartDataPoint> getSalesDataByMonth();
    static QList<ChartDataPoint> getTopProducts();
    static QList<ChartDataPoint> getPaymentStatus();
    static QList<ChartDataPoint> getRevenueByVendor();
    static QList<ChartDataPoint> getOrdersStatusDistribution();
    
    // Données pour les charts VENDEUR
    static QList<ChartDataPoint> getVendorSalesData(int vendorId);
    static QList<ChartDataPoint> getVendorTopProducts(int vendorId);
    static QList<ChartDataPoint> getVendorClientsFidelity(int vendorId);
    static QList<ChartDataPoint> getVendorPaymentStats(int vendorId);

    // Données pour les stats cards
    static double getTotalRevenue();
    static int getTotalOrders();
    static int getTotalClients();
    static int getTotalProducts();
    static int getUnpaidOrders();
    static double getPendingAmount();
};

#endif // CHARTSMANAGER_H
