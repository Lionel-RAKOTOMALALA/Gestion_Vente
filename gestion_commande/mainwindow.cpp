#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ajout_produit.h"
#include "dialog.h"
#include "connexion.h"
#include <QSqlError>
#include <QSqlQuery>
#include "stylesheet.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Appliquer styles spécifiques sur des widgets précis
    ui->produitView->setStyleSheet(Stylesheet::tableStyle());
    ui->AjoutBtn->setStyleSheet(Stylesheet::buttonStyle());

    reloadData();
    QSqlQuery query;
    query.prepare("SELECT stock, design from produit ");
    query.bindValue(":design", "Sambosa" );
    query.exec();

    //parcours d'une reponse d'une requete
    while(query.next()){

        qDebug() << query.value(1).toString();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::affichDonnee(){
    //ouvrir la base de données
    connexion conn;
    if (!conn.ouvrirConnexion()) {
        qDebug() << "Impossible d'ouvrir la base";
        return;
    }
    if(modelClient == nullptr){
        modelClient = new QSqlQueryModel(this);
        ui->clientView->setModel(model);
    }


}
void MainWindow::reloadData(){
    // 1. Ouvrir la connexion
    connexion conn;
    if (!conn.ouvrirConnexion()) {
        qDebug() << "Impossible d'ouvrir la base";
        return;
    }

    // 2. Si le modèle n'existe pas encore → on le crée
    if (model == nullptr) {
        model = new QSqlQueryModel(this);
        ui->produitView->setModel(model);

        // Mise en forme
        ui->produitView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }

    // 3. Écrire ta requête SQL manuelle
    QString sql = "SELECT id_pro, design, prix, stock FROM produit ORDER BY id_pro DESC";

    // 4. Exécuter la requête
    model->setQuery(sql);

    // 5. Vérifier les erreurs SQL
    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL: " << model->lastError().text();
    }

    // (Optionnel) renommer les colonnes
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Design");
    model->setHeaderData(2, Qt::Horizontal, "Prix");
    model->setHeaderData(3, Qt::Horizontal, "Stock");
}
void MainWindow::on_AjoutBtn_clicked()
{
    Ajout_produit ajouter;
    ajouter.exec();
}

