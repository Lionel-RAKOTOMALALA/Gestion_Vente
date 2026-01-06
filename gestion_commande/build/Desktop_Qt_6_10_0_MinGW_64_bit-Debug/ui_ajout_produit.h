/********************************************************************************
** Form generated from reading UI file 'ajout_produit.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AJOUT_PRODUIT_H
#define UI_AJOUT_PRODUIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Ajout_produit
{
public:
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *nomEdit;
    QLineEdit *prixEdit;
    QLineEdit *stockEdit;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *annulerBtn;
    QPushButton *ajouterBtn;
    QLabel *label_4;

    void setupUi(QDialog *Ajout_produit)
    {
        if (Ajout_produit->objectName().isEmpty())
            Ajout_produit->setObjectName("Ajout_produit");
        Ajout_produit->resize(389, 453);
        formLayoutWidget = new QWidget(Ajout_produit);
        formLayoutWidget->setObjectName("formLayoutWidget");
        formLayoutWidget->setGeometry(QRect(20, 100, 351, 111));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(formLayoutWidget);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        label_2 = new QLabel(formLayoutWidget);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        label_3 = new QLabel(formLayoutWidget);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_3);

        nomEdit = new QLineEdit(formLayoutWidget);
        nomEdit->setObjectName("nomEdit");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, nomEdit);

        prixEdit = new QLineEdit(formLayoutWidget);
        prixEdit->setObjectName("prixEdit");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, prixEdit);

        stockEdit = new QLineEdit(formLayoutWidget);
        stockEdit->setObjectName("stockEdit");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, stockEdit);

        horizontalLayoutWidget = new QWidget(Ajout_produit);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(60, 240, 291, 80));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        annulerBtn = new QPushButton(horizontalLayoutWidget);
        annulerBtn->setObjectName("annulerBtn");

        horizontalLayout->addWidget(annulerBtn);

        ajouterBtn = new QPushButton(horizontalLayoutWidget);
        ajouterBtn->setObjectName("ajouterBtn");

        horizontalLayout->addWidget(ajouterBtn);

        label_4 = new QLabel(Ajout_produit);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(30, 20, 151, 31));
        label_4->setStyleSheet(QString::fromUtf8("font-size: 20px;\n"
"color: blue;"));

        retranslateUi(Ajout_produit);

        QMetaObject::connectSlotsByName(Ajout_produit);
    } // setupUi

    void retranslateUi(QDialog *Ajout_produit)
    {
        Ajout_produit->setWindowTitle(QCoreApplication::translate("Ajout_produit", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("Ajout_produit", "Nom du produit:", nullptr));
        label_2->setText(QCoreApplication::translate("Ajout_produit", "Prix", nullptr));
        label_3->setText(QCoreApplication::translate("Ajout_produit", "Stock", nullptr));
        annulerBtn->setText(QCoreApplication::translate("Ajout_produit", "Annuler", nullptr));
        ajouterBtn->setText(QCoreApplication::translate("Ajout_produit", "Confirmer", nullptr));
        label_4->setText(QCoreApplication::translate("Ajout_produit", "Ajout de produit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Ajout_produit: public Ui_Ajout_produit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AJOUT_PRODUIT_H
