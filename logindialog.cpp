#include "logindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    userId(-1)
{
    setWindowTitle("Connexion");
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Connexion à l'application");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(titleLabel);

    QHBoxLayout *emailLayout = new QHBoxLayout();
    QLabel *emailLabel = new QLabel("Email:");
    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("Entrez votre email");
    emailLayout->addWidget(emailLabel);
    emailLayout->addWidget(emailEdit);
    layout->addLayout(emailLayout);

    QHBoxLayout *passwordLayout = new QHBoxLayout();
    QLabel *passwordLabel = new QLabel("Mot de passe:");
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("Entrez votre mot de passe");
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordEdit);
    layout->addLayout(passwordLayout);

    // Set default values
    emailEdit->setText("rakotomalalalionel32@gmail.com");
    passwordEdit->setText("lionel3208");

    loginButton = new QPushButton("Se connecter");
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    layout->addWidget(loginButton);

    errorLabel = new QLabel("");
    errorLabel->setStyleSheet("color: red;");
    layout->addWidget(errorLabel);

    setLayout(layout);
}

LoginDialog::~LoginDialog()
{
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

bool LoginDialog::authenticate(const QString &email, const QString &password)
{
    // Hacher le mot de passe
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

    QSqlQuery query;
    query.prepare("SELECT id_user, role FROM USERS WHERE email = ? AND mot_de_passe = ? AND actif = 1");
    query.addBindValue(email);
    query.addBindValue(QString(hashedPassword));

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