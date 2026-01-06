#ifndef AJOUT_PRODUIT_H
#define AJOUT_PRODUIT_H

#include <QDialog>

namespace Ui {
class Ajout_produit;
}

class Ajout_produit : public QDialog
{
    Q_OBJECT

public:
    explicit Ajout_produit(QWidget *parent = nullptr);
    ~Ajout_produit();

private slots:
    void on_annulerBtn_clicked();

    void on_ajouterBtn_clicked();

private:
    Ui::Ajout_produit *ui;
};

#endif // AJOUT_PRODUIT_H
