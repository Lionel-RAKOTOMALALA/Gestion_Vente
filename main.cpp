#include "mainwindow.h"
#include "connexion.h"
#include "stylesheet.h"
#include "logindialog.h"

#include <QApplication>
#include <QDebug>
#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QThread>
#include <QElapsedTimer>

// Fonction pour créer le splash screen avec une barre de progression
QPixmap createSplashPixmap(int progressPercent) {
    QPixmap pixmap(700, 450);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    
    // Fond avec dégradé linéaire
    QLinearGradient gradient(0, 0, 0, 450);
    gradient.setColorAt(0, QColor(41, 128, 185));      // Bleu ciel
    gradient.setColorAt(0.5, QColor(52, 152, 219));    // Bleu clair
    gradient.setColorAt(1, QColor(25, 95, 160));       // Bleu foncé
    
    painter.fillRect(0, 0, 700, 450, gradient);
    
    // Ajouter des éléments décoratifs (cercles partiels)
    painter.setOpacity(0.1);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawEllipse(-100, -100, 400, 400);
    painter.drawEllipse(400, 300, 350, 350);
    painter.setOpacity(1.0);
    
    // Bordure arrondie et élégante
    painter.setPen(QPen(QColor(255, 255, 255, 100), 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(15, 15, 670, 420, 15, 15);
    
    // Titre principal avec ombre
    QFont titleFont;
    titleFont.setFamily("Segoe UI");
    titleFont.setPointSize(42);
    titleFont.setBold(true);
    
    // Ombre du titre
    painter.setFont(titleFont);
    painter.setPen(QColor(0, 0, 0, 80));
    painter.drawText(40, 125, 640, 110, Qt::AlignHCenter | Qt::AlignVCenter, "Gestion Vente Matériel");
    
    // Titre en blanc
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(35, 120, 640, 110, Qt::AlignHCenter | Qt::AlignVCenter, "Gestion Vente Matériel");
    
    // Ligne décorative sous le titre
    painter.setPen(QPen(QColor(255, 255, 255, 150), 2));
    painter.drawLine(100, 220, 600, 220);
    
    // Sous-titre
    QFont subtitleFont;
    subtitleFont.setFamily("Segoe UI");
    subtitleFont.setPointSize(14);
    subtitleFont.setItalic(true);
    painter.setFont(subtitleFont);
    painter.setPen(QColor(220, 240, 255));
    painter.drawText(50, 240, 600, 30, Qt::AlignHCenter, "Application de gestion commerciale");
    
    // Version
    QFont versionFont;
    versionFont.setFamily("Segoe UI");
    versionFont.setPointSize(11);
    painter.setFont(versionFont);
    painter.setPen(QColor(180, 200, 230));
    painter.drawText(50, 310, 600, 20, Qt::AlignHCenter, "Version 1.0 - © 2026");
    
    // Barre de progression (fond)
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(50, 100, 150, 150));
    painter.drawRoundedRect(80, 360, 540, 8, 4, 4);
    
    // Barre de progression (avant - animation)
    int barWidth = (540 * progressPercent) / 100;
    QLinearGradient progressGradient(80, 360, 80 + barWidth, 360);
    progressGradient.setColorAt(0, QColor(52, 200, 220));
    progressGradient.setColorAt(1, QColor(100, 220, 255));
    painter.setBrush(progressGradient);
    painter.drawRoundedRect(80, 360, barWidth, 8, 4, 4);
    
    // Texte "Chargement..."
    QFont loadingFont;
    loadingFont.setFamily("Segoe UI");
    loadingFont.setPointSize(12);
    painter.setFont(loadingFont);
    painter.setPen(QColor(220, 240, 255));
    painter.drawText(50, 385, 600, 25, Qt::AlignHCenter, "⚙ Chargement en cours...");
    
    painter.end();
    return pixmap;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Créer et afficher le splash screen avec animation
    QSplashScreen splash(createSplashPixmap(0));
    splash.setWindowFlags(splash.windowFlags() | Qt::FramelessWindowHint);
    splash.show();
    a.processEvents();

    // Animation de la barre de progression (0 à 100%)
    QElapsedTimer timer;
    timer.start();
    int lastProgress = 0;
    
    while (timer.elapsed() < 3000) { // 3 secondes de durée
        int progress = (timer.elapsed() * 100) / 3000;
        if (progress != lastProgress) {
            splash.setPixmap(createSplashPixmap(progress));
            lastProgress = progress;
        }
        a.processEvents();
        QThread::msleep(50);
    }
    
    // Afficher 100% à la fin
    splash.setPixmap(createSplashPixmap(100));
    a.processEvents();

    // Appliquer le style global
    a.setStyleSheet(StyleSheet::getStyleSheet());
    qDebug() << "QSS appliqué, longueur:" << StyleSheet::getStyleSheet().length();

    if (!Connexion::createConnection()) {
        splash.finish(&splash);
        return -1;
    }

    while (true) {
        splash.showMessage("Initialisation de la connexion...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
        a.processEvents();
        QThread::msleep(500);
        
        LoginDialog loginDialog;
        splash.finish(&loginDialog);
        
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
