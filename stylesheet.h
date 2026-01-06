#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QString>

class StyleSheet
{
public:
    StyleSheet();

    static QString getStyleSheet();

private:
    // Couleurs primaires
    static const QString PRIMARY;
    static const QString SECONDARY;
    static const QString ACCENT;

    // Couleurs fonctionnelles
    static const QString SUCCESS;
    static const QString WARNING;
    static const QString ERROR;
    static const QString INFO;

    // Neutres
    static const QString BACKGROUND;
    static const QString SURFACE;
    static const QString BORDER;
    static const QString TEXT_PRIMARY;
    static const QString TEXT_SECONDARY;

    // Typographie
    static const QString FONT_FAMILY;
};

#endif // STYLESHEET_H
