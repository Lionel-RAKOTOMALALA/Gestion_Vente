/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

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

class Ui_Dialog
{
public:
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *designEdit;
    QLineEdit *prixEdit;
    QLineEdit *stockEdit;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_2;
    QPushButton *confirmAjoutBtn;
    QLabel *label_4;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(400, 300);
        formLayoutWidget = new QWidget(Dialog);
        formLayoutWidget->setObjectName("formLayoutWidget");
        formLayoutWidget->setGeometry(QRect(30, 80, 341, 121));
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

        designEdit = new QLineEdit(formLayoutWidget);
        designEdit->setObjectName("designEdit");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, designEdit);

        prixEdit = new QLineEdit(formLayoutWidget);
        prixEdit->setObjectName("prixEdit");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, prixEdit);

        stockEdit = new QLineEdit(formLayoutWidget);
        stockEdit->setObjectName("stockEdit");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, stockEdit);

        horizontalLayoutWidget = new QWidget(Dialog);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(50, 220, 311, 51));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_2 = new QPushButton(horizontalLayoutWidget);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout->addWidget(pushButton_2);

        confirmAjoutBtn = new QPushButton(horizontalLayoutWidget);
        confirmAjoutBtn->setObjectName("confirmAjoutBtn");

        horizontalLayout->addWidget(confirmAjoutBtn);

        label_4 = new QLabel(Dialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 20, 171, 16));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("Dialog", "Design", nullptr));
        label_2->setText(QCoreApplication::translate("Dialog", "Prix", nullptr));
        label_3->setText(QCoreApplication::translate("Dialog", "Stock", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Dialog", "Annuler", nullptr));
        confirmAjoutBtn->setText(QCoreApplication::translate("Dialog", "confirmer l'ajout", nullptr));
        label_4->setText(QCoreApplication::translate("Dialog", "Ajout de stock de produit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
