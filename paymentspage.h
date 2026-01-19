#ifndef PAYMENTSPAGE_H
#define PAYMENTSPAGE_H

#include <QFrame>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class PaymentsPage : public QFrame
{
    Q_OBJECT

public:
    explicit PaymentsPage(QWidget *parent = nullptr);

private:
    void setupUI();
    void loadPayments();

private slots:
    void onSearchTextChanged(const QString &text);
    void onStatusFilterChanged(const QString &status);

private:
    QTableWidget *paymentsTable;
    QLineEdit *searchInput;
    QComboBox *statusFilter;
    QPushButton *refreshBtn;
};

#endif // PAYMENTSPAGE_H