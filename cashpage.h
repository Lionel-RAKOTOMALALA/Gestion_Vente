#ifndef CASHPAGE_H
#define CASHPAGE_H

#include <QFrame>
#include <QTableWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>

class CashPage : public QFrame
{
    Q_OBJECT

public:
    explicit CashPage(QWidget *parent = nullptr);

private:
    void setupUI();
    void loadStatistics();
    void loadTransactions();
    void updateTransactions();

private slots:
    void onSearchTextChanged(const QString &text);
    void onDateChanged();
    void onRefresh();
    void onCancelPayment();
    void onExport();

private:
    QTableWidget *transactionsTable;
    QLineEdit *searchInput;
    QDateEdit *dateFilter;
    QPushButton *refreshBtn;
    QPushButton *exportBtn;
    
    // Statistics labels
    QLabel *totalCashedLabel;
    QLabel *transactionCountLabel;
    QLabel *averageAmountLabel;
};

#endif // CASHPAGE_H