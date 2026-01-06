#include "ajout_produit.h"
#include "ui_ajout_produit.h"
#include "connexion.h"
#include <QString>
#include <QMessageBox>
#include <QSqlQuery>

Ajout_produit::Ajout_produit(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Ajout_produit)
{
    ui->setupUi(this);
}

Ajout_produit::~Ajout_produit()
{
    delete ui;
}

void Ajout_produit::on_annulerBtn_clicked()
{
    this->close();
}


void Ajout_produit::on_ajouterBtn_clicked()
{
    connexion conn;
    conn.ouvrirConnexion();
    QString nom_produit = ui->nomEdit->text();
    QString prix = ui->prixEdit->text();
    QString stock = ui->stockEdit->text();

    if(nom_produit.isEmpty() || prix.isEmpty() || stock.isEmpty()){
        QMessageBox::information(this, "Erreur", "Veuillez remplir tous les champs!");
    }else{
        QSqlQuery query;
        query.prepare("INSERT INTO produit(design,prix,stock) VALUES(:design,:prix,:stock);");
        query.bindValue(":design",nom_produit);
        query.bindValue("prix", prix);
        query.bindValue(":stock",stock);
        if(query.exec()){
            QMessageBox::information(this,"Succès", "L'ajout s'est effectué avec succes");
        }else{
            QMessageBox::critical(this,"erreur", "Il y a une erreur lors de l'ajout du produit");
        }
        this->close();
    }

}

