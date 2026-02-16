#ifndef STOCKMOVEMENTPAGE_H
#define STOCKMOVEMENTPAGE_H

#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>

class StockMovementPage : public QFrame
{
    Q_OBJECT

public:
    explicit StockMovementPage(const QString &userRole, int userId, QWidget *parent = nullptr);
    void loadStockMovements();

private slots:
    // manual add removed: movements are automatic
    // edit/delete actions removed: stock movements are immutable
    void onSearchTextChanged(const QString &text);
    void onFilterTypeChanged(int index);
    void onFilterDateChanged();
    void refreshMovements();
    void onExportPDF();

private:
    void setupUI();
    void setupDatabase();
    void applyStyles();
    void loadStatistics();
    void populateMovementsTable(const QString &filterType = "", const QDate &startDate = QDate(), const QDate &endDate = QDate());
    void updateProductStock(int productId, int quantity, const QString &type);

    QLineEdit *searchInput;
    QPushButton *btnRefresh;
    QPushButton *btnExportPDF;
    QComboBox *filterType;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QTableWidget *movementsTable;
    
    // Statistics labels
    QLabel *totalProductsLabel;
    QLabel *movementsCountLabel;
    QLabel *lowStockLabel;
    
    QString userRole;
    int userId;
};

#endif // STOCKMOVEMENTPAGE_H
