#ifndef CONNEXION_H
#define CONNEXION_H

#include <QtSql/QSqlDatabase>

class connexion
{
public:
    bool ouvrirConnexion(); //pour lier la base de donnée à l'application
    void fermerConnexion(); //manily connexionuyjh,n

private:
    QSqlDatabase db;
};

#endif // CONNEXION_H
