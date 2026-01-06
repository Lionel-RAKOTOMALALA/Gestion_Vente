#ifndef USERSPAGE_H
#define USERSPAGE_H

#include <QFrame>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

class UsersPage : public QFrame
{
    Q_OBJECT

public:
    explicit UsersPage(QWidget *parent = nullptr);

private slots:
    void onAddUser();
    void onEditUser(int userId);
    void onDeleteUser(int userId);
    void onSearchTextChanged(const QString &text);
    void onFilterRoleChanged(const QString &role);
    void onFirstPage();
    void onPreviousPage();
    void onNextPage();
    void onLastPage();

private:
    void setupUI();
    void setupDatabase();
    void loadUsers();
    void applyStyles();
    void updatePaginationControls();
    QWidget* createActionButtons(int userId);
    QWidget* createRoleBadge(const QString &role);
    
    QTableWidget *tableWidget;
    QLineEdit *searchInput;
    QComboBox *roleFilter;
    QPushButton *btnAdd;
    QPushButton *btnRefresh;
    QPushButton *btnFirstPage;
    QPushButton *btnPrevPage;
    QPushButton *btnNextPage;
    QPushButton *btnLastPage;
    QLabel *lblPageInfo;
    
    int currentPage;
    int itemsPerPage;
    int totalItems;
};

#endif // USERSPAGE_H
