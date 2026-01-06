#include "mainwindow.h"
#include "connexion.h"
#include "stylesheet.h"
#include "logindialog.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Appliquer le style global
    a.setStyleSheet(StyleSheet::getStyleSheet());
    qDebug() << "QSS appliqué, longueur:" << StyleSheet::getStyleSheet().length();

    if (!Connexion::createConnection()) {
        return -1;
    }

    while (true) {
        LoginDialog loginDialog;
        if (loginDialog.exec() != QDialog::Accepted) {
            break; // Quitter si connexion échouée ou annulée
        }

        MainWindow w(loginDialog.getUserRole(), loginDialog.getUserId());
        
        // Variable pour savoir si on doit recommencer la boucle de login
        bool restartLogin = false;
        
        // Connecter le signal de déconnexion pour recommencer la boucle
        QObject::connect(&w, &MainWindow::logoutRequested, [&restartLogin]() {
            restartLogin = true;
        });
        
        w.show();
        a.exec();
        
        // Si on ne doit pas recommencer, quitter l'application
        if (!restartLogin) {
            break;
        }
    }

    return 0;
}
