#ifndef PRODUCTSPAGE_H
#define PRODUCTSPAGE_H

#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "orderdialog.h"

class ProductsPage : public QFrame
{
    Q_OBJECT

public:
    explicit ProductsPage(const QString &userRole, int userId, QWidget *parent = nullptr);
    void loadProducts();

private slots:
    void onAddProduct();
    void onEditProduct(int productId);
    void onDeleteProduct(int productId);
    void onOrderProduct();
    void onSearchTextChanged(const QString &text);

signals:
    void orderValidated();

private:
    void setupUI();
    void setupDatabase();
    void applyStyles();
    QWidget* createProductCard(int productId, const QString &nom, const QString &description,
                              const QString &imagePath, double prixVente, int stock, int seuilAlerte);

    QLineEdit *searchInput;
    QPushButton *btnAdd;
    QPushButton *btnOrder;
    QPushButton *btnRefresh;
    QScrollArea *scrollArea;
    QWidget *productsContainer;
    QGridLayout *productsGrid;
    
    QString userRole;
    int userId;
    OrderDialog *orderDialog;
};

#endif // PRODUCTSPAGE_H
