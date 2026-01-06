#ifndef ORDERDIALOG_H
#define ORDERDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QMap>
#include <QSqlQuery>

struct OrderItem {
    int productId;
    QString productName;
    double unitPrice;
    int quantity;
    double total;
};

class OrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OrderDialog(int userId, QWidget *parent = nullptr);
    explicit OrderDialog(int userId, const QString &commandeId, QWidget *parent = nullptr);
    ~OrderDialog();

    void addProduct(int productId, const QString &productName, double unitPrice, int quantity = 1);
    void removeProduct(int productId, int quantity = 1);
    double getTotal() const;
    bool checkStocks();
    void reset();
    void resetUI();

private slots:
    void onNextStep();
    void onPreviousStep();
    void onValidateOrder();
    void onCancelOrder();
    void onRemoveItem(int row);

signals:
    void orderSaved();

private:
    void setupUI();
    void setupClientForm();
    void setupOrderSummary();
    void updateTotal();
    void updateTable();
    void createTablesIfNotExist();
    bool saveClientAndOrder();
    void loadOrderForEdit(const QString &commandeId);

    QStackedWidget *stackedWidget;
    
    // Étape 1: Formulaire client
    QWidget *clientWidget;
    QLineEdit *nomEdit;
    QLineEdit *prenomEdit;
    QLineEdit *telephoneEdit;
    QLineEdit *emailEdit;
    QTextEdit *adresseEdit;
    QPushButton *nextBtn;
    
    // Étape 2: Récapitulatif commande
    QWidget *orderWidget;
    QTableWidget *orderTable;
    QLabel *totalLabel;
    QPushButton *previousBtn;
    QPushButton *validateBtn;
    QPushButton *cancelBtn;

    QMap<int, OrderItem> orderItems; // productId -> OrderItem
    double totalAmount;
    int currentUserId;
    bool isEditMode;
    QString editCommandeId;
    
    // Informations client pour l'édition
    QString clientNom;
    QString clientPrenom;
    QString clientTelephone;
    QString clientEmail;
    QString clientAdresse;
};

#endif // ORDERDIALOG_H