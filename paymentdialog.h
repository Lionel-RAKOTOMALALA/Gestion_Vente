#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>

class PaymentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PaymentDialog(double totalAmount = 0.0, QWidget *parent = nullptr);
    
    double getAmount() const;
    QString getPaymentMethod() const;

private slots:
    void onConfirm();
    void onCancel();
    void onUseTotal();

private:
    void setupUI();
    
    double totalAmount;
    QLabel *totalLabel;
    QDoubleSpinBox *amountSpinBox;
    QComboBox *paymentMethodCombo;
    QPushButton *useFullAmountBtn;
    QPushButton *confirmBtn;
    QPushButton *cancelBtn;
};

#endif // PAYMENTDIALOG_H
