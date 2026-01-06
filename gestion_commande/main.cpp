#include "mainwindow.h"
#include "connexion.h"
#include "stylesheet.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    connexion c;
    MainWindow w;


    a.setStyleSheet(Stylesheet::appStyle());
    w.show();
    // ICI : tes requêtes SQL ou ouverture de fenêtre

    return a.exec();
}
