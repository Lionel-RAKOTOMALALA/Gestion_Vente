// stylesheet.h
#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QString>

namespace Stylesheet {
    // Variables simples, inline pour pouvoir les modifier dans le header
    // Utiliser des `const char*` réduit le coût d'initialisation et rend la lecture simple.
    inline const char *primaryColor = "#2f3b52"; // couleur principale
    inline const char *accentColor  = "#ff9800"; // couleur d'accent
    inline const char *textColor    = "#ffffff"; // couleur texte
    inline const char *lightBgColor = "#ffffff"; // fond clair
    inline const int borderRadius   = 6;          // arrondis

    // Fonctions renvoyant des styles en QString (templates simples dans la .cpp)
    QString appStyle();
    QString buttonStyle();
    QString tableStyle();
}

#endif // STYLESHEET_H
