#include "logindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QPixmap>
#include <QFont>
#include <QBrush>
#include <QPalette>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    userId(-1)
{
    setWindowTitle("GestionVente - Connexion");
    setModal(true);
    setMinimumSize(1200, 700);
    
    // Style global sombre
    setStyleSheet(
        "QDialog {"
        "   background: #0f172a;"
        "}"
    );
    
    setupUI();
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ========== PARTIE GAUCHE: FORMULAIRE ==========
    QWidget *leftWidget = new QWidget();
    leftWidget->setStyleSheet("background: #0f172a;");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(60, 60, 60, 60);
    leftLayout->setSpacing(30);

    // Logo/Title
    QLabel *logoLabel = new QLabel("GestionVente");
    logoLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #e2e8f0;"
        "letter-spacing: 2px;"
    );
    leftLayout->addWidget(logoLabel);

    leftLayout->addSpacing(40);

    // Welcome text
    QLabel *welcomeLabel = new QLabel("Bienvenue");
    welcomeLabel->setStyleSheet(
        "font-size: 32px;"
        "font-weight: 700;"
        "color: #f1f5f9;"
        "letter-spacing: -0.5px;"
    );
    leftLayout->addWidget(welcomeLabel);

    QLabel *subtitleLabel = new QLabel("Entrez vos identifiants ci-dessous");
    subtitleLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #94a3b8;"
    );
    leftLayout->addWidget(subtitleLabel);

    leftLayout->addSpacing(20);

    // Email input
    QLabel *emailLabelTitle = new QLabel("Nom d'utilisateur");
    emailLabelTitle->setStyleSheet(
        "font-size: 12px;"
        "font-weight: 600;"
        "color: #cbd5e1;"
        "text-transform: uppercase;"
        "letter-spacing: 0.5px;"
    );
    leftLayout->addWidget(emailLabelTitle);

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("vous@example.com");
    emailEdit->setMinimumHeight(48);
    emailEdit->setStyleSheet(
        "QLineEdit {"
        "   border: 1px solid #334155;"
        "   border-radius: 8px;"
        "   padding: 12px 16px;"
        "   background: #1e293b;"
        "   color: #f1f5f9;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #3b82f6;"
        "   background: #1e293b;"
        "}"
        "QLineEdit::placeholder {"
        "   color: #64748b;"
        "}"
    );
    emailEdit->setText("admin@example.com");
    leftLayout->addWidget(emailEdit);

    // Password input
    QLabel *passwordLabelTitle = new QLabel("Mot de passe");
    passwordLabelTitle->setStyleSheet(
        "font-size: 12px;"
        "font-weight: 600;"
        "color: #cbd5e1;"
        "text-transform: uppercase;"
        "letter-spacing: 0.5px;"
    );
    leftLayout->addWidget(passwordLabelTitle);

    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("Entrez 4 caractères ou plus");
    passwordEdit->setMinimumHeight(48);
    passwordEdit->setStyleSheet(
        "QLineEdit {"
        "   border: 1px solid #334155;"
        "   border-radius: 8px;"
        "   padding: 12px 16px;"
        "   background: #1e293b;"
        "   color: #f1f5f9;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #3b82f6;"
        "   background: #1e293b;"
        "}"
        "QLineEdit::placeholder {"
        "   color: #64748b;"
        "}"
    );
    passwordEdit->setText("admin123");
    leftLayout->addWidget(passwordEdit);

    // Forgot password link
    QHBoxLayout *forgotLayout = new QHBoxLayout();
    forgotLayout->addStretch();
    
    forgotButton = new QPushButton("Mot de passe oublié ?");
    forgotButton->setFlat(true);
    forgotButton->setStyleSheet(
        "QPushButton {"
        "   color: #3b82f6;"
        "   border: none;"
        "   background: transparent;"
        "   font-size: 12px;"
        "   text-decoration: underline;"
        "}"
        "QPushButton:hover {"
        "   color: #60a5fa;"
        "}"
    );
    connect(forgotButton, &QPushButton::clicked, this, &LoginDialog::onForgotPassword);
    forgotLayout->addWidget(forgotButton);
    leftLayout->addLayout(forgotLayout);

    leftLayout->addSpacing(10);

    // Login button
    loginButton = new QPushButton("Se connecter");
    loginButton->setMinimumHeight(48);
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setStyleSheet(
        "QPushButton {"
        "   background: #3b82f6;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background: #2563eb;"
        "}"
        "QPushButton:pressed {"
        "   background: #1d4ed8;"
        "}"
    );
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    leftLayout->addWidget(loginButton);

    // Error message
    errorLabel = new QLabel("");
    errorLabel->setStyleSheet(
        "color: #ef4444;"
        "font-size: 13px;"
    );
    errorLabel->setWordWrap(true);
    leftLayout->addWidget(errorLabel);

    leftLayout->addStretch();

    leftWidget->setMinimumWidth(420);
    leftWidget->setMaximumWidth(500);
    mainLayout->addWidget(leftWidget, 0);

    // ========== PARTIE DROITE: IMAGE/BRANDING ==========
    QWidget *rightWidget = new QWidget();
    rightWidget->setStyleSheet(
        "background-image: url(:/images/bg_login_right.png);"
        "background-position: center;"
        "background-repeat: no-repeat;"
        "background-attachment: fixed;"
    );
    
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(40, 40, 40, 40);
    rightLayout->setSpacing(0);
    rightLayout->setAlignment(Qt::AlignCenter);
    
    // Content layout
    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(15);
    contentLayout->setAlignment(Qt::AlignCenter);
    
    rightLayout->addLayout(contentLayout, 1);

    // Right side content
    contentLayout->addStretch();

    QLabel *rightTitleLabel = new QLabel("Gestion Optimisée");
    rightTitleLabel->setAlignment(Qt::AlignCenter);
    rightTitleLabel->setStyleSheet(
        "font-size: 36px;"
        "font-weight: 700;"
        "color: #f1f5f9;"
        "letter-spacing: -0.5px;"
    );
    contentLayout->addWidget(rightTitleLabel);

    QLabel *rightHighlightLabel = new QLabel("des Ventes");
    rightHighlightLabel->setAlignment(Qt::AlignCenter);
    rightHighlightLabel->setStyleSheet(
        "font-size: 36px;"
        "font-weight: 700;"
        "color: #ec4899;"
        "letter-spacing: -0.5px;"
    );
    contentLayout->addWidget(rightHighlightLabel);

    contentLayout->addSpacing(20);

    QLabel *rightDescLabel = new QLabel(
        "Gérez vos commandes, clients et produits avec efficacité. "
        "Suivez vos ventes en temps réel et notifiez vos clients automatiquement. "
        "Une solution complète pour votre commerce de matériel."
    );
    rightDescLabel->setAlignment(Qt::AlignCenter);
    rightDescLabel->setWordWrap(true);
    rightDescLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #cbd5e1;"
        "line-height: 1.6;"
    );
    rightDescLabel->setMaximumWidth(380);
    contentLayout->addWidget(rightDescLabel, 0, Qt::AlignCenter);

    contentLayout->addSpacing(30);

    QPushButton *moreBtn = new QPushButton("Découvrir Plus");
    moreBtn->setMaximumWidth(220);
    moreBtn->setMinimumHeight(40);
    moreBtn->setCursor(Qt::PointingHandCursor);
    moreBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #f1f5f9;"
        "   border: 1px solid #475569;"
        "   border-radius: 6px;"
        "   font-size: 13px;"
        "   font-weight: 600;"
        "   padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(30, 41, 59, 0.5);"
        "   border-color: #64748b;"
        "}"
    );

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(moreBtn);
    btnLayout->addStretch();
    contentLayout->addLayout(btnLayout);

    contentLayout->addStretch();

    mainLayout->addWidget(rightWidget, 1);

    setLayout(mainLayout);
}

void LoginDialog::onLoginClicked()
{
    QString email = emailEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        errorLabel->setText("Veuillez remplir tous les champs.");
        return;
    }

    if (authenticate(email, password)) {
        accept();
    } else {
        errorLabel->setText("Email ou mot de passe incorrect.");
    }
}

void LoginDialog::onForgotPassword()
{
    QMessageBox::information(this, "Mot de passe oublié", 
        "Veuillez contacter votre administrateur pour réinitialiser votre mot de passe.");
}

bool LoginDialog::authenticate(const QString &email, const QString &password)
{
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

    QSqlQuery query;
    query.prepare("SELECT id_user, role FROM USERS WHERE email = :email AND mot_de_passe = :password AND actif = 1");
    query.bindValue(":email", email);
    query.bindValue(":password", QString(hashedPassword));

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'authentification:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        userId = query.value(0).toInt();
        userRole = query.value(1).toString();
        return true;
    }

    return false;
}