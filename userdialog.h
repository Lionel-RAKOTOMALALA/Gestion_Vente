#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(QWidget *parent = nullptr, int userId = -1);

private slots:
    void onSave();
    void onCancel();
    void onSelectPhoto();

private:
    void setupUI();
    void loadUser(int userId);
    bool validateInput();
    void displayPhotoPreview();
    
    int currentUserId;
    QString photoPath;
    QLineEdit *txtNom;
    QLineEdit *txtEmail;
    QLineEdit *txtPassword;
    QComboBox *cboRole;
    QCheckBox *chkActif;
    QLabel *lblPhotoPreview;
    QPushButton *btnSelectPhoto;
    QPushButton *btnSave;
    QPushButton *btnCancel;
};

#endif // USERDIALOG_H