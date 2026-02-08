
#include "connexion.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

Connexion::Connexion() {}

bool Connexion::createConnection()
{
    // Obtenir le répertoire pour stocker la base de données
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    
    // Créer le répertoire s'il n'existe pas
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QString dbPath = appDataPath + "/VenteMaterielInfo.db";
    
    // Si la base de données n'existe pas, la copier depuis les ressources
    if (!QFile::exists(dbPath)) {
        QString resourcePath = ":/database/database/VenteMaterielInfo.db";
        QFile resourceFile(resourcePath);
        if (resourceFile.exists()) {
            if (!QFile::copy(resourcePath, dbPath)) {
                qDebug() << "Erreur lors de la copie de la base de données depuis les ressources:" << dbPath;
                return false;
            }
            qDebug() << "Base de données copiée depuis les ressources vers:" << dbPath;
        } else {
            qDebug() << "Fichier de base de données introuvable dans les ressources:" << resourcePath;
            return false;
        }
    }
    
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Erreur de connexion à la base de données:" << db.lastError().text();
        return false;
    }

    qDebug() << "Connexion à la base de données réussie ôô (chemin:" << dbPath << ")";

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
