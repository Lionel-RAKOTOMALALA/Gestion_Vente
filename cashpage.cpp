#include "cashpage.h"
#include <QVBoxLayout>
#include <QLabel>

CashPage::CashPage(QWidget *parent) : QFrame(parent)
{
    setObjectName("cashPage");
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *title = new QLabel("Gestion de la Caisse", this);
    title->setObjectName("titleH1");
    layout->addWidget(title);
    // Ajouter des widgets pour la gestion de la caisse ici
    layout->addStretch();
}