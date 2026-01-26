#ifndef RECEIPTGENERATOR_H
#define RECEIPTGENERATOR_H

#include <QString>
#include <QDateTime>
#include <QVector>
#include "orderdialog.h"

struct ClientInfo {
    QString nom;
    QString prenom;
    QString telephone;
    QString email;
    QString adresse;
};

struct OrderInfo {
    int id;
    QDateTime date;
    double total;
    QString statut;
};

struct PaymentInfo {
    int id;
    QDateTime date;
    double montant;
    QString statut;
};

struct ReceiptData {
    ClientInfo client;
    OrderInfo order;
    PaymentInfo payment;
    QVector<OrderItem> items;
    QString vendeurName;
    double remainingAmount;  // Montant restant à payer
};

class ReceiptGenerator
{
public:
    ReceiptGenerator();
    
    // Générer le PDF et l'ouvrir/sauvegarder
    bool generatePDF(const ReceiptData &data, const QString &outputPath = "");
    
    // Envoyer le PDF par email
    bool sendReceiptByEmail(const QString &recipientEmail, const QString &pdfFilePath, const ReceiptData &data);
    
    // Récupérer les données de commande pour générer le reçu
    static ReceiptData getReceiptData(int orderId);

private:
    QString generateHTML(const ReceiptData &data);
};

#endif // RECEIPTGENERATOR_H
