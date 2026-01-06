#include "paymentspage.h"
#include <QVBoxLayout>
#include <QLabel>

PaymentsPage::PaymentsPage(QWidget *parent) : QFrame(parent)
{
    setObjectName("paymentsPage");
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *title = new QLabel("Gestion des Paiements", this);
    title->setObjectName("titleH1");
    layout->addWidget(title);
    // Ajouter des widgets pour la gestion des paiements ici
    layout->addStretch();
}