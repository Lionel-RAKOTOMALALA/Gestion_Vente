#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "sidebar.h"
#include "productspage.h"
#include "orderspage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &userRole, int userId, QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void logoutRequested();

private slots:
    void onLogoutRequested();
    void onPageChanged(int index);

private:
    Ui::MainWindow *ui;
    Sidebar *sidebar;
    QStackedWidget *stackedWidget;
    ProductsPage *productsPage;
    OrdersPage *ordersPage;
    int ordersPageIndex;
    int currentUserId;
};
#endif // MAINWINDOW_H
