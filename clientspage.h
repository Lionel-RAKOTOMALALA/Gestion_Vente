#ifndef CLIENTSPAGE_H
#define CLIENTSPAGE_H

#include <QFrame>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

class ClientsPage : public QFrame
{
    Q_OBJECT

public:
    explicit ClientsPage(QWidget *parent = nullptr);

private slots:
    void onAddClient();
    void onEditClient(int clientId);
    void onDeleteClient(int clientId);
    void onSearchTextChanged(const QString &text);
    void onFirstPage();
    void onPreviousPage();
    void onNextPage();
    void onLastPage();

public slots:
    void onThemeChanged();

private:
    void setupUI();
    void setupDatabase();
    void loadClients();
    void applyStyles();
    void updatePaginationControls();
    QWidget* createActionButtons(int clientId);
    
    QTableWidget *tableWidget;
    QLineEdit *searchInput;
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

#endif // CLIENTSPAGE_H