#include "connexion.h"
#include <QDebug>
#include <QSqlError>



bool connexion::ouvrirConnexion()
{
    // Driver SQLite
    db = QSqlDatabase::addDatabase("QSQLITE");

    // Chemin vers ta base SQLite
    db.setDatabaseName("D:\\gestion_commande.db");


    if(!db.open()) {
        qDebug() << " Erreur d'ouverture: " << db.lastError().text();
        return false;
    }

    qDebug() << "✅ connexion à SQLite réussie !";
    return true;
}

void connexion::fermerConnexion()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "🔒 connexion fermée.";
    }
}
