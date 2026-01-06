#ifndef ORDERSPAGE_H
#define ORDERSPAGE_H

#include <QFrame>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

class OrdersPage : public QFrame
{
    Q_OBJECT

public:
    explicit OrdersPage(const QString &userRole, int userId, QWidget *parent = nullptr);
    void loadOrders();

private slots:
    void onSearchTextChanged(const QString &text);
    void onStatusFilterChanged(const QString &status);
    void onRefreshClicked();
    void onViewOrderDetails(int row, int column);
    void onEditOrder(const QString &commandeId);
    void onDeleteOrder(const QString &commandeId);
    void onFirstPageClicked();
    void onPreviousPageClicked();
    void onNextPageClicked();
    void onLastPageClicked();

private:
    void setupUI();
    void setupDatabase();
    void applyFilters();
    void updatePaginationUI();

    QTableWidget *ordersTable;
    QLineEdit *searchInput;
    QComboBox *statusFilter;
    QPushButton *refreshBtn;
    
    // Pagination
    QWidget *paginationWidget;
    QPushButton *btnFirstPage;
    QPushButton *btnPreviousPage;
    QPushButton *btnNextPage;
    QPushButton *btnLastPage;
    QLabel *pageInfoLabel;
    
    QString userRole;
    int userId;
    
    QString currentSearchText;
    QString currentStatusFilter;
    
    // Pagination data
    int currentPage;
    int itemsPerPage;
    int totalItems;
    int totalPages;
};

#endif // ORDERSPAGE_H