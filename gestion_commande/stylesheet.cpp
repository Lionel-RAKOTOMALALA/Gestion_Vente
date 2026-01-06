// stylesheet.cpp
#include "stylesheet.h"

namespace Stylesheet {
    // Construire les styles manuellement, sans utiliser `arg()` ni raw string literal
    // Utiliser la concaténation de QString pour garder une syntaxe lisible.

    QString buttonStyle() {
        QString s = "QPushButton { background-color: ";
        s += QString::fromLatin1(primaryColor);
        s += "; color: ";
        s += QString::fromLatin1(textColor);
        s += "; border-radius: ";
        s += QString::number(borderRadius);
        s += "px; padding: 6px 12px; }";

        s += "QPushButton:hover { background-color: ";
        s += QString::fromLatin1(accentColor);
        s += "; }";

        s += "QPushButton:pressed { background-color: #b26a00; }";
        return s;
    }

    QString tableStyle() {
        QString s = "QTableView { background-color: ";
        s += QString::fromLatin1(lightBgColor);
        s += "; color: #222; gridline-color: #ddd; selection-background-color: ";
        s += QString::fromLatin1(accentColor);
        s += "; selection-color: ";
        s += QString::fromLatin1(textColor);
        s += "; }";

        s += "QHeaderView::section { background-color: ";
        s += QString::fromLatin1(primaryColor);
        s += "; color: ";
        s += QString::fromLatin1(textColor);
        s += "; padding: 4px; }";
        return s;
    }

    QString appStyle() {
        QString s;
        s += buttonStyle();
        s += tableStyle();
        s += "QMainWindow { background-color: #f4f6f8; }";
        return s;
    }
}
