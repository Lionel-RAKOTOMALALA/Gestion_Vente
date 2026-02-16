#ifndef PROFILEPANEL_H
#define PROFILEPANEL_H

#include <QFrame>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QComboBox>

class ProfilePanel : public QFrame
{
    Q_OBJECT

public:
    explicit ProfilePanel(int userId, QWidget *parent = nullptr);
    void loadUserData();

signals:
    void profileUpdated(int userId);

private slots:
    void onSelectPhoto();
    void onEditProfile();
    void onSaveProfile();
    void onCancelEdit();
    void onChangePassword();
    void displayPhotoPreview();

private:
    void setupUI();
    void setupEditMode(bool isEdit);
    void applyStyles();
    bool validateInput();
    void loadPhotoFromDatabase();
    void loadPhotoProfile();
    
    int currentUserId;
    QString currentPhotoPath;
    QString originalPhotoPath;
    bool isEditMode;
    
    // Display mode widgets
    QLabel *lblPhotoDisplay;
    QLabel *lblNomDisplay;
    QLabel *lblEmailDisplay;
    QLabel *lblRoleDisplay;
    QLabel *lblDateCreationDisplay;
    QLabel *lblStatusDisplay;
    QPushButton *btnEditProfile;
    QPushButton *btnChangePassword;
    
    // Edit mode widgets
    QLineEdit *txtNomEdit;
    QLineEdit *txtEmailEdit;
    QComboBox *cboRoleEdit;
    QLabel *lblPhotoEdit;
    QPushButton *btnSelectPhoto;
    QPushButton *btnSaveProfile;
    QPushButton *btnCancelEdit;
    
    // Container widgets
    QWidget *displayContainer;
    QWidget *editContainer;
};

#endif // PROFILEPANEL_H
