#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getUserRole() const { return userRole; }
    int getUserId() const { return userId; }

private slots:
    void onLoginClicked();

private:
    QLineEdit *emailEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QLabel *errorLabel;

    QString userRole;
    int userId;

    bool authenticate(const QString &email, const QString &password);
};

#endif // LOGINDIALOG_H