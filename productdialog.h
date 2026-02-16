#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>

class StockMovementPage;
class DashboardPage;

class ProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductDialog(QWidget *parent = nullptr, int productId = -1, int userId = -1, StockMovementPage *stockPage = nullptr, DashboardPage *dashPage = nullptr);

private slots:
    void onSave();
    void onCancel();
    void onSelectImage();

private:
    void setupUI();
    void loadProduct(int productId);
    bool validateInput();
    void updateImagePreview();

    int currentProductId;
    QLineEdit *txtNom;
    QTextEdit *txtDescription;
    QLineEdit *txtPrixVente;
    QLineEdit *txtPrixAchat;
    QLineEdit *txtStock;
    QLineEdit *txtSeuilAlerte;
    QLabel *lblImagePreview;
    QPushButton *btnSelectImage;
    QPushButton *btnSave;
    QPushButton *btnCancel;

    QString selectedImagePath;
    int currentUserId;
    StockMovementPage *stockMovementPage;
    DashboardPage *dashboardPage;
};

#endif // PRODUCTDIALOG_H