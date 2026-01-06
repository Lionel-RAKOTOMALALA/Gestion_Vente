
#include "connexion.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QDebug>

Connexion::Connexion() {}

bool Connexion::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:/VenteMaterielInfo.db");

    if (!db.open()) {
        qDebug() << "Erreur de connexion à la base de données:" << db.lastError().text();
        return false;
    }

    qDebug() << "Connexion à la base de données réussie ôô";

    // Créer la table USERS si elle n'existe pas
    QSqlQuery query;
    QString createTable = "CREATE TABLE IF NOT EXISTS USERS ("
                          "id_user INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "nom TEXT NOT NULL,"
                          "email TEXT NOT NULL UNIQUE,"
                          "mot_de_passe TEXT NOT NULL,"
                          "role TEXT NOT NULL CHECK (role IN ('ADMIN','VENDEUR','CAISSIER')),"
                          "actif INTEGER DEFAULT 1,"
                          "date_creation DATETIME DEFAULT CURRENT_TIMESTAMP"
                          ");";
    if (!query.exec(createTable)) {
        qDebug() << "Erreur lors de la création de la table USERS:" << query.lastError().text();
        return false;
    }

    qDebug() << "Table USERS créée ou déjà existante.";

    // Insérer un utilisateur par défaut si la table est vide
    query.exec("SELECT COUNT(*) FROM USERS");
    if (query.next() && query.value(0).toInt() == 0) {
        QString hashedPassword = QCryptographicHash::hash(QString("admin123").toUtf8(), QCryptographicHash::Sha256).toHex();
        query.prepare("INSERT INTO USERS (nom, email, mot_de_passe, role) VALUES (?, ?, ?, ?)");
        query.addBindValue("Admin");
        query.addBindValue("admin@example.com");
        query.addBindValue(hashedPassword);
        query.addBindValue("ADMIN");
        if (!query.exec()) {
            qDebug() << "Erreur lors de l'insertion de l'utilisateur par défaut:" << query.lastError().text();
            return false;
        }
        qDebug() << "Utilisateur par défaut inséré.";
    }

    return true;
}
