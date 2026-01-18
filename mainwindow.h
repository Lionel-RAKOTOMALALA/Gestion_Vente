#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "sidebar.h"
#include "thememanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DashboardPage;
class UsersPage;
class ClientsPage;
class ProductsPage;
class OrdersPage;
class PaymentsPage;
class CashPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &userRole, int userId, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLogoutRequested();
    void onPageChanged(int index);
    void onThemeToggled();
    void onThemeChanged(ThemeManager::Theme theme);

private:
    void applyTheme();
    void applyThemeToAllPages();

    Ui::MainWindow *ui;
    Sidebar *sidebar;
    QStackedWidget *stackedWidget;
    int currentUserId;
    int ordersPageIndex;
    ProductsPage *productsPage;
    OrdersPage *ordersPage;
    ClientsPage *clientsPage;

signals:
    void logoutRequested();
};
#endif // MAINWINDOW_H
