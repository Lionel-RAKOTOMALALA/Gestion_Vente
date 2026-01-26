#ifndef ORDERDETAILSDIALOG_H
#define ORDERDETAILSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include "receiptgenerator.h"

class OrderDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OrderDetailsDialog(int orderId, QWidget *parent = nullptr);
    ~OrderDetailsDialog();

private:
    void setupUI();
    void loadOrderDetails();
    void setupStyles();

    int orderId;
    ReceiptData orderData;
    
    // UI Components
    QLabel *headerLabel;
    QLabel *orderIdLabel;
    QLabel *dateLabel;
    QLabel *statusLabel;
    
    QLabel *clientNameLabel;
    QLabel *clientPhoneLabel;
    QLabel *clientEmailLabel;
    QLabel *clientAddressLabel;
    
    QTableWidget *productsTable;
    
    QLabel *totalAmountLabel;
    QLabel *paidAmountLabel;
    QLabel *remainingAmountLabel;
    QLabel *paymentStatusLabel;
    
    QPushButton *generatePdfBtn;
    QPushButton *notifyBtn;
    QPushButton *closeBtn;

private slots:
    void onGeneratePdf();
    void onNotifyClient();
};

#endif // ORDERDETAILSDIALOG_H
