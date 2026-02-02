#include "chartsmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

ChartsManager::ChartsManager()
{
}

// ============= DONNÉES POUR ADMIN =============

QList<ChartDataPoint> ChartsManager::getSalesDataByMonth()
{
    QList<ChartDataPoint> data;
    
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "  SUBSTR(DATE(c.date_commande), 1, 7) as mois, "
        "  SUM(c.total) as total_ventes, "
        "  COUNT(*) as nombre_commandes "
        "FROM COMMANDES c "
        "WHERE c.date_commande >= date('now', '-12 months') "
        "GROUP BY SUBSTR(DATE(c.date_commande), 1, 7) "
        "ORDER BY mois DESC "
        "LIMIT 12"
    );

    if (query.exec()) {
        QList<QColor> colors = {
            QColor(59, 130, 246),    // Blue
            QColor(139, 92, 246),    // Purple
            QColor(236, 72, 153),    // Pink
            QColor(248, 113, 113),   // Red
            QColor(34, 197, 94),     // Green
            QColor(250, 204, 21),    // Yellow
            QColor(6, 182, 212),     // Cyan
            QColor(168, 85, 247),    // Violet
            QColor(239, 68, 68),     // Red2
            QColor(16, 185, 129),    // Emerald
            QColor(251, 146, 60),    // Orange
            QColor(15, 23, 42)       // Dark
        };
        
        int colorIndex = 0;
        while (query.next()) {
            ChartDataPoint point;
            point.label = query.value("mois").toString();
            point.value = query.value("total_ventes").toDouble();
            point.color = colors[colorIndex % colors.size()];
            data.append(point);
            colorIndex++;
        }
    } else {
        qDebug() << "Erreur getSalesDataByMonth:" << query.lastError().text();
    }
    
    return data;
}

QList<ChartDataPoint> ChartsManager::getTopProducts()
{
    QList<ChartDataPoint> data;
    
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "  p.nom_produit, "
        "  SUM(dc.quantite) as total_quantite "
        "FROM DETAILS_COMMANDE dc "
        "JOIN PRODUITS p ON dc.id_produit = p.id_produit "
        "GROUP BY p.id_produit, p.nom_produit "
        "ORDER BY total_quantite DESC "
        "LIMIT 10"
    );

    if (query.exec()) {
        QList<QColor> colors = {
            QColor(59, 130, 246), QColor(139, 92, 246), QColor(236, 72, 153),
            QColor(248, 113, 113), QColor(34, 197, 94), QColor(250, 204, 21),
            QColor(6, 182, 212), QColor(168, 85, 247), QColor(239, 68, 68),
            QColor(16, 185, 129)
        };
        
        int colorIndex = 0;
        while (query.next()) {
            ChartDataPoint point;
            point.label = query.value("nom_produit").toString();
            point.value = query.value("total_quantite").toDouble();
            point.color = colors[colorIndex % colors.size()];
            data.append(point);
            colorIndex++;
        }
    } else {
        qDebug() << "Erreur getTopProducts:" << query.lastError().text();
    }
    
    return data;
}

QList<ChartDataPoint> ChartsManager::getPaymentStatus()
{
    QList<ChartDataPoint> data;
    
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "  c.statut, "
        "  COUNT(*) as nombre, "
        "  SUM(c.total) as montant_total "
        "FROM COMMANDES c "
        "GROUP BY c.statut"
    );

    if (query.exec()) {
        QMap<QString, QColor> colors;
        colors["EN_COURS"] = QColor(250, 204, 21);    // Yellow
        colors["PAYEE"] = QColor(34, 197, 94);        // Green
        colors["ANNULEE"] = QColor(239, 68, 68);      // Red
        
        while (query.next()) {
            ChartDataPoint point;
            point.label = query.value("statut").toString();
            point.value = query.value("nombre").toDouble();
            point.color = colors.value(point.label, QColor(148, 163, 184));
            data.append(point);
        }
    } else {
        qDebug() << "Erreur getPaymentStatus:" << query.lastError().text();
    }
    
    return data;
}

QList<ChartDataPoint> ChartsManager::getRevenueByVendor()
{
    QList<ChartDataPoint> data;
    
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "  u.nom as vendeur, "
        "  SUM(c.total) as total_ventes, "
        "  COUNT(c.id_commande) as nombre_commandes "
        "FROM COMMANDES c "
        "JOIN USERS u ON c.id_user = u.id_user "
        "WHERE u.role = 'VENDEUR' "
        "GROUP BY u.id_user, u.nom "
        "ORDER BY total_ventes DESC"
    );

    if (query.exec()) {
        QList<QColor> colors = {
            QColor(59, 130, 246), QColor(139, 92, 246), QColor(236, 72, 153),
            QColor(248, 113, 113), QColor(34, 197, 94), QColor(250, 204, 21)
        };
        
        int colorIndex = 0;
        while (query.next()) {
            ChartDataPoint point;
            point.label = query.value("vendeur").toString();
            point.value = query.value("total_ventes").toDouble();
            point.color = colors[colorIndex % colors.size()];
            data.append(point);
            colorIndex++;
        }
    } else {
        qDebug() << "Erreur getRevenueByVendor:" << query.lastError().text();
    }
    
    return data;
}

QList<ChartDataPoint> ChartsManager::getOrdersStatusDistribution()
{
    QList<ChartDataPoint> data;
    
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "  c.statut, "
        "  COUNT(*) as nombre "
        "FROM COMMANDES c "
        "GROUP BY c.statut"
    );

    if (query.exec()) {
        QMap<QString, QColor> colors;
        colors["EN_COURS"] = QColor(250, 204, 21);
        colors["PAYEE"] = QColor(34, 197, 94);
        colors["ANNULEE"] = QColor(239, 68, 68);
        
        while (query.next()) {
            ChartDataPoint point;
            point.label = query.value("statut").toString();
            point.value = query.value("nombre").toDouble();
            point.color = colors.value(point.label, QColor(148, 163, 184));
            data.append(point);
        }
    } else {
        qDebug() << "Erreur getOrdersStatusDistribution:" << query.lastError().text();
    }
    
    return data;
}

// ============= DONNÉES POUR VENDEUR =============

QList<ChartDataPoint> ChartsManager::getVendorSalesData(int vendorId)
{
    QList<ChartDataPoint> data;
    
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "  SUBSTR(DATE(c.date_commande), 1, 7) as mois, "
        "  SUM(c.total) as total_ventes "
        "FROM COMMANDES c "
        "WHERE c.id_user = :vendorId "
        "AND c.date_commande >= date('now', '-6 months') "
        "GROUP BY SUBSTR(DATE(c.date_commande), 1, 7) "
        "ORDER BY mois DESC"
    );
    query.bindValue(":vendorId", vendorId);

    if (query.exec()) {
        QList<QColor> colors = {
            QColor(59, 130, 246), QColor(139, 92, 246), QColor(236, 72, 153),
            QColor(248, 113, 113), QColor(34, 197, 94), QColor(250, 204, 21)
        };
        
        int colorIndex = 0;
        while (query.next()) {
            ChartDataPoint point;
            point.label = query.value("mois").toString();
            point.value = query.value("total_ventes").toDouble();
            point.color = colors[colorIndex % colors.size()];
            data.append(point);
            colorIndex++;
        }
    } else {
        qDebug() << "Erreur getVendorSalesData:" << query.lastError().text();
    }
    
    return data;
}

QList<ChartDataPoint> ChartsManager::getVendorTopProducts(int vendorId)
{
    QList<ChartDataPoint> data;
    
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "  p.nom_produit, "
        "  SUM(dc.quantite) as total_quantite "
        "FROM DETAILS_COMMANDE dc "
        "JOIN COMMANDES c ON dc.id_commande = c.id_commande "
        "JOIN PRODUITS p ON dc.id_produit = p.id_produit "
        "WHERE c.id_user = :vendorId "
        "GROUP BY p.id_produit, p.nom_produit "
        "ORDER BY total_quantite DESC "
        "LIMIT 8"
    );
    query.bindValue(":vendorId", vendorId);

    if (query.exec()) {
        QList<QColor> colors = {
            QColor(59, 130, 246), QColor(139, 92, 246), QColor(236, 72, 153),
            QColor(248, 113, 113), QColor(34, 197, 94), QColor(250, 204, 21),
            QColor(6, 182, 212), QColor(168, 85, 247)
        };
        
        int colorIndex = 0;
        while (query.next()) {
            ChartDataPoint point;
            point.label = query.value("nom_produit").toString();
            point.value = query.value("total_quantite").toDouble();
            point.color = colors[colorIndex % colors.size()];
            data.append(point);
            colorIndex++;
        }
    } else {
        qDebug() << "Erreur getVendorTopProducts:" << query.lastError().text();
    }
    
    return data;
}

QList<ChartDataPoint> ChartsManager::getVendorClientsFidelity(int vendorId)
{
    QList<ChartDataPoint> data;
    
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "  cl.nom, "
        "  COUNT(c.id_commande) as nombre_commandes, "
        "  SUM(c.total) as montant_total "
        "FROM COMMANDES c "
        "JOIN CLIENTS cl ON c.id_client = cl.id_client "
        "WHERE c.id_user = :vendorId "
        "GROUP BY c.id_client, cl.nom "
        "ORDER BY nombre_commandes DESC "
        "LIMIT 8"
    );
    query.bindValue(":vendorId", vendorId);

    if (query.exec()) {
        QList<QColor> colors = {
            QColor(59, 130, 246), QColor(139, 92, 246), QColor(236, 72, 153),
            QColor(248, 113, 113), QColor(34, 197, 94), QColor(250, 204, 21),
            QColor(6, 182, 212), QColor(168, 85, 247)
        };
        
        int colorIndex = 0;
        while (query.next()) {
            ChartDataPoint point;
            point.label = query.value("nom").toString();
            point.value = query.value("nombre_commandes").toDouble();
            point.color = colors[colorIndex % colors.size()];
            data.append(point);
            colorIndex++;
        }
    } else {
        qDebug() << "Erreur getVendorClientsFidelity:" << query.lastError().text();
    }
    
    return data;
}

QList<ChartDataPoint> ChartsManager::getVendorPaymentStats(int vendorId)
{
    QList<ChartDataPoint> data;
    
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "  c.statut, "
        "  COUNT(*) as nombre, "
        "  SUM(c.total) as montant_total "
        "FROM COMMANDES c "
        "WHERE c.id_user = :vendorId "
        "GROUP BY c.statut"
    );
    query.bindValue(":vendorId", vendorId);

    if (query.exec()) {
        QMap<QString, QColor> colors;
        colors["EN_COURS"] = QColor(250, 204, 21);
        colors["PAYEE"] = QColor(34, 197, 94);
        colors["ANNULEE"] = QColor(239, 68, 68);
        
        while (query.next()) {
            ChartDataPoint point;
            point.label = query.value("statut").toString();
            point.value = query.value("montant_total").toDouble();
            point.color = colors.value(point.label, QColor(148, 163, 184));
            data.append(point);
        }
    } else {
        qDebug() << "Erreur getVendorPaymentStats:" << query.lastError().text();
    }
    
    return data;
}

// ============= DONNÉES POUR STATS CARDS =============

double ChartsManager::getTotalRevenue()
{
    QSqlQuery query;
    query.prepare("SELECT COALESCE(SUM(total), 0) as total FROM COMMANDES WHERE statut = 'PAYEE'");
    
    if (query.exec() && query.next()) {
        return query.value("total").toDouble();
    }
    return 0.0;
}

int ChartsManager::getTotalOrders()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) as count FROM COMMANDES");
    
    if (query.exec() && query.next()) {
        return query.value("count").toInt();
    }
    return 0;
}

int ChartsManager::getTotalClients()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) as count FROM CLIENTS");
    
    if (query.exec() && query.next()) {
        return query.value("count").toInt();
    }
    return 0;
}

int ChartsManager::getTotalProducts()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) as count FROM PRODUITS");
    
    if (query.exec() && query.next()) {
        return query.value("count").toInt();
    }
    return 0;
}

int ChartsManager::getUnpaidOrders()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) as count FROM COMMANDES WHERE statut = 'EN_COURS'");
    
    if (query.exec() && query.next()) {
        return query.value("count").toInt();
    }
    return 0;
}

double ChartsManager::getPendingAmount()
{
    QSqlQuery query;
    query.prepare("SELECT COALESCE(SUM(total), 0) as total FROM COMMANDES WHERE statut = 'EN_COURS'");
    
    if (query.exec() && query.next()) {
        return query.value("total").toDouble();
    }
    return 0.0;
}
