#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "sidebar.h"
#include "thememanager.h"
#include <QLabel>
#include <QToolButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DashboardPage;
class UsersPage;
class ClientsPage;
class ProductsPage;
class OrdersPage;
class PaymentsPage;
class StockMovementPage;
class ProfilePanel;

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
    void onProfileRequested();
    void onProfileUpdated(int userId);

private:
    void applyTheme();
    void applyThemeToAllPages();
    void updateProfilePhoto(int userId);

    Ui::MainWindow *ui;
    Sidebar *sidebar;
    QStackedWidget *stackedWidget;
    QWidget *topBar;
    QLabel *topTitleLabel;
    QToolButton *profileButton;
    int currentUserId;
    int ordersPageIndex;
    int profilePageIndex;
    DashboardPage *dashboardPage;
    ProductsPage *productsPage;
    OrdersPage *ordersPage;
    ClientsPage *clientsPage;
    ProfilePanel *profilePanel;
    StockMovementPage *stockMovementPage;

signals:
    void logoutRequested();
};
#endif // MAINWINDOW_H
