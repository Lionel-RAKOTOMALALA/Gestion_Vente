#ifndef CONNEXION_H
#define CONNEXION_H

#include <QSqlDatabase>

class Connexion
{
public:
    Connexion();
    static bool createConnection();
};

#endif // CONNEXION_H
