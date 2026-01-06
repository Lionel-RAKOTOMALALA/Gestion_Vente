#include "dialog.h"
#include "ui_dialog.h"
#include "connexion.h"
#include <QString>
#include "qdebug.h"
#include "stylesheet.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>


Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    // Appliquer le style aux boutons du dialog
    ui->confirmAjoutBtn->setStyleSheet(Stylesheet::buttonStyle());
    ui->pushButton_2->setStyleSheet(Stylesheet::buttonStyle());
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_2_clicked()
{
    this->close();
}


void Dialog::on_confirmAjoutBtn_clicked()
{
    connexion conn;
    conn.ouvrirConnexion();

    QString design = ui->designEdit->text();
    QString prix = ui->prixEdit->text();
    QString stock = ui->stockEdit->text();

    if(design.isEmpty() || prix.isEmpty() || stock.isEmpty()){
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs !");
        return;
    }

    QSqlQuery query;

    query.prepare("INSERT INTO produit (design, prix, stock) VALUES (:design, :prix, :stock)");

    query.bindValue(":design", design);
    query.bindValue(":prix", prix.toFloat());
    query.bindValue(":stock", stock.toInt());

    if (query.exec()) {
        QMessageBox::information(this, "Succès", "Produit ajouté avec succès !");
        this->close();  // ferme le dialog après ajout
    } else {
        QMessageBox::critical(this, "Erreur SQL", "L'ajout a échoué : " + query.lastError().text());
    }

}

