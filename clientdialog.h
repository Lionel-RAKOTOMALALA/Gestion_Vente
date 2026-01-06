#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDialog(QWidget *parent = nullptr, int clientId = -1);

private slots:
    void onSave();
    void onCancel();

private:
    void setupUI();
    void loadClient(int clientId);
    bool validateInput();
    
    int currentClientId;
    QLineEdit *txtNom;
    QLineEdit *txtPrenom;
    QLineEdit *txtEmail;
    QLineEdit *txtTelephone;
    QLineEdit *txtAdresse;
    QPushButton *btnSave;
    QPushButton *btnCancel;
};

#endif // CLIENTDIALOG_H