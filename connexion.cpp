
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
            // Définir les permissions de lecture/écriture
            QFile::setPermissions(dbPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::WriteGroup | QFile::ReadOther);
            qDebug() << "Base de données copiée depuis les ressources vers:" << dbPath;
        } else {
            qDebug() << "Fichier de base de données introuvable dans les ressources:" << resourcePath;
            return false;
        }
    }
    
    // S'assurer que le fichier existant a les bonnes permissions
    QFile::setPermissions(dbPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::WriteGroup | QFile::ReadOther);
    
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
                          "date_creation DATETIME DEFAULT CURRENT_TIMESTAMP,"
                          "photo_profile TEXT"
                          ");";
    if (!query.exec(createTable)) {
        qDebug() << "Erreur lors de la création de la table USERS:" << query.lastError().text();
        return false;
    }

    qDebug() << "Table USERS créée ou déjà existante.";

    // Vérifier si la colonne photo_profile existe, sinon la créer (migration)
    bool hasPhotoColumn = false;
    if (query.exec("PRAGMA table_info(USERS);")) {
        while (query.next()) {
            QString colName = query.value(1).toString(); // name column is at index 1
            if (colName.compare("photo_profile", Qt::CaseInsensitive) == 0) {
                hasPhotoColumn = true;
                break;
            }
        }
    }

    if (!hasPhotoColumn) {
        if (!query.exec("ALTER TABLE USERS ADD COLUMN photo_profile TEXT DEFAULT '';") ) {
            qDebug() << "Erreur lors de l'ajout de la colonne photo_profile:" << query.lastError().text();
            // ne pas échouer la connexion pour autant, on continue
        } else {
            qDebug() << "Colonne photo_profile ajoutée à la table USERS.";
        }
    }

    // Insérer un utilisateur par défaut si la table est vide
    query.exec("SELECT COUNT(*) FROM USERS");
    if (query.next() && query.value(0).toInt() == 0) {
        QString hashedPassword = QCryptographicHash::hash(QString("admin123").toUtf8(), QCryptographicHash::Sha256).toHex();
        query.prepare("INSERT INTO USERS (nom, email, mot_de_passe, role, photo_profile) VALUES (?, ?, ?, ?, ?)");
        query.addBindValue("Admin");
        query.addBindValue("admin@example.com");
        query.addBindValue(hashedPassword);
        query.addBindValue("ADMIN");
        query.addBindValue("");
        if (!query.exec()) {
            qDebug() << "Erreur lors de l'insertion de l'utilisateur par défaut:" << query.lastError().text();
            return false;
        }
        qDebug() << "Utilisateur par défaut inséré.";
    }

    return true;
}
