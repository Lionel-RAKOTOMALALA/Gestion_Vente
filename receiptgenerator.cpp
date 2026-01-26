#include "receiptgenerator.h"
#include "smtpclient.h"
#include "smtpconfig.h"
#include <QPdfWriter>
#include <QPainter>
#include <QPageSize>
#include <QTextDocument>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QDebug>

ReceiptGenerator::ReceiptGenerator()
{
}

bool ReceiptGenerator::generatePDF(const ReceiptData &data, const QString &outputPath)
{
    // Créer le nom du fichier
    QString fileName = outputPath;
    if (fileName.isEmpty()) {
        QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        fileName = QString("%1/Recu_Commande_%2_%3.pdf")
            .arg(documentsPath)
            .arg(data.order.id)
            .arg(QDateTime::currentDateTime().toString("ddMMyyyy_hhmmss"));
    }

    // Générer le HTML
    QString html = generateHTML(data);

    // Créer un QTextDocument et charger le HTML
    QTextDocument doc;
    doc.setHtml(html);

    // Créer le PDF writer
    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize::A4);
    pdfWriter.setTitle("Reçu de Paiement");
    pdfWriter.setCreator("Gestion Vente Materiel");

    // Imprimer le document en PDF
    doc.print(&pdfWriter);

    // Ouvrir le PDF après génération
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));

    return true;
}

QString ReceiptGenerator::generateHTML(const ReceiptData &data)
{
    // Calculer le total
    double totalItems = 0.0;
    for (const OrderItem &item : data.items) {
        totalItems += item.total;
    }

    // Générer les lignes des produits
    QString itemsHTML;
    for (const OrderItem &item : data.items) {
        itemsHTML += QString(
            "<tr>"
            "<td>%1</td>"
            "<td align='center'>%2</td>"
            "<td align='right'>%3 Ar</td>"
            "<td align='right'><b>%4 Ar</b></td>"
            "</tr>"
        ).arg(item.productName)
         .arg(item.quantity)
         .arg(QString::number(item.unitPrice, 'f', 2))
         .arg(QString::number(item.total, 'f', 2));
    }

    // Template HTML simplifié (compatible avec QTextDocument)
    QString htmlTemplate = QString(
        "<html>"
        "<head><meta charset='UTF-8'></head>"
        "<body style='font-family: Arial, sans-serif; margin: 20px;'>"
        ""
        "<div style='text-align: center; background-color: #1e293b; color: #10b981; padding: 20px; margin-bottom: 20px; border-radius: 5px;'>"
        "<h1 style='margin: 0;'>✓ REÇU DE PAIEMENT</h1>"
        "</div>"
        ""
        "<table style='width: 100%; border-collapse: collapse;'>"
        "<tr><td style='width: 50%; vertical-align: top;'>"
        "<b style='color: #1e293b; border-bottom: 2px solid #10b981;'>INFORMATIONS CLIENT</b><br>"
        "<small>"
        "Nom: <b>%1 %2</b><br>"
        "Téléphone: %3<br>"
        "Email: %4<br>"
        "Adresse: %5<br>"
        "</small>"
        "</td><td style='width: 50%; vertical-align: top; padding-left: 20px;'>"
        "<b style='color: #1e293b; border-bottom: 2px solid #10b981;'>COMMANDE N°%6</b><br>"
        "<small>"
        "Date: %7<br>"
        "Statut: <b>%8</b><br>"
        "</small>"
        "</td></tr>"
        "</table>"
        ""
        "<br>"
        "<b style='color: #1e293b; border-bottom: 2px solid #10b981;'>DÉTAILS DES ACHATS</b>"
        "<table style='width: 100%; border-collapse: collapse; margin-top: 10px;'>"
        "<tr style='background-color: #1e293b; color: white;'>"
        "<th style='padding: 10px; text-align: left;'>Produit</th>"
        "<th style='padding: 10px; text-align: center;'>Quantité</th>"
        "<th style='padding: 10px; text-align: right;'>Prix Unit.</th>"
        "<th style='padding: 10px; text-align: right;'>Total</th>"
        "</tr>"
        "%9"
        "<tr style='background-color: #f0fdf4; font-weight: bold; color: #10b981;'>"
        "<td colspan='3' style='padding: 10px; text-align: right;'>TOTAL:</td>"
        "<td style='padding: 10px; text-align: right;'>%10</td>"
        "</tr>"
        "</table>"
        ""
        "<br>"
        "<b style='color: #1e293b; border-bottom: 2px solid #10b981;'>PAIEMENT</b>"
        "<table style='width: 100%; margin-top: 10px;'>"
        "<tr><td style='width: 50%;'>Mode: <b>ESPÈCES</b></td><td style='width: 50%;'>Date: %11</td></tr>"
        "<tr><td colspan='2'><b style='color: #10b981; font-size: 16px;'>Montant payé: %12</b></td></tr>"
        "<tr><td colspan='2'>Reste à payer: <b style='color: #f59e0b;'>%13</b></td></tr>"
        "<tr><td colspan='2'>Statut paiement: <b>%14</b></td></tr>"
        "</table>"
        ""
        "<div style='text-align: center; margin-top: 30px; padding-top: 20px; border-top: 2px solid #e2e8f0; color: #666; font-size: 12px;'>"
        "Merci pour votre achat | Ticket généré le %14"
        "</div>"
        ""
        "</body>"
        "</html>"
    )
    .arg(data.client.nom)
    .arg(data.client.prenom)
    .arg(data.client.telephone.isEmpty() ? "N/A" : data.client.telephone)
    .arg(data.client.email.isEmpty() ? "N/A" : data.client.email)
    .arg(data.client.adresse.isEmpty() ? "N/A" : data.client.adresse)
    .arg(data.order.id)
    .arg(data.order.date.toString("dd/MM/yyyy hh:mm"))
    .arg(data.order.statut == "EN_COURS" ? "En cours" : "Payée")
    .arg(itemsHTML)
    .arg(QString::number(totalItems, 'f', 2) + " Ar")
    .arg(data.payment.date.toString("dd/MM/yyyy hh:mm"))
    .arg(QString::number(data.payment.montant, 'f', 2) + " Ar")
    .arg(QString::number(data.remainingAmount, 'f', 2) + " Ar")
    .arg(data.payment.statut)
    .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));

    return htmlTemplate;
}

bool ReceiptGenerator::sendReceiptByEmail(const QString &recipientEmail, const QString &pdfFilePath, const ReceiptData &data)
{
    qDebug() << "=== sendReceiptByEmail() called ===";
    qDebug() << "Recipient email:" << recipientEmail;
    qDebug() << "PDF file path:" << pdfFilePath;
    
    if (recipientEmail.isEmpty()) {
        qWarning() << "Email recipient is empty";
        return false;
    }

    if (!QFile::exists(pdfFilePath)) {
        qWarning() << "PDF file does not exist:" << pdfFilePath;
        return false;
    }
    
    qDebug() << "PDF file exists: OK";

    // Créer le corps de l'email au format HTML
    QString orderStatut = data.order.statut == "EN_COURS" ? "En cours" : "Payée";
    
    // Calculer le montant restant
    double remainingAmount = data.order.total - data.payment.montant;
    QString statusColor = (remainingAmount <= 0) ? "#10b981" : "#f59e0b";
    QString statusBgColor = (remainingAmount <= 0) ? "#f0fdf4" : "#fffbeb";
    
    // Déterminer l'icône et le message de statut
    QString statusIcon = (remainingAmount <= 0) ? "✓" : "⏳";
    
    QString emailBody = QString(
        "<!DOCTYPE html>"
        "<html lang='fr'>"
        "<head>"
        "    <meta charset='UTF-8'>"
        "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "    <style>"
        "        body {"
        "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
        "            line-height: 1.6;"
        "            color: #333;"
        "            margin: 0;"
        "            padding: 0;"
        "            background-color: #f9fafb;"
        "        }"
        "        .container {"
        "            max-width: 600px;"
        "            margin: 0 auto;"
        "            background-color: #ffffff;"
        "            border-radius: 8px;"
        "            overflow: hidden;"
        "            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);"
        "        }"
        "        .header {"
        "            background: linear-gradient(135deg, #1e293b 0%, #0f172a 100%);"
        "            color: white;"
        "            padding: 40px 20px;"
        "            text-align: center;"
        "        }"
        "        .header h1 {"
        "            margin: 0;"
        "            font-size: 28px;"
        "            font-weight: 700;"
        "        }"
        "        .header p {"
        "            margin: 10px 0 0 0;"
        "            font-size: 14px;"
        "            opacity: 0.9;"
        "        }"
        "        .content {"
        "            padding: 40px 30px;"
        "        }"
        "        .greeting {"
        "            font-size: 16px;"
        "            margin-bottom: 25px;"
        "        }"
        "        .greeting strong {"
        "            color: #1e293b;"
        "        }"
        "        .section {"
        "            margin-bottom: 30px;"
        "        }"
        "        .section-title {"
        "            font-size: 14px;"
        "            font-weight: 700;"
        "            color: #1e293b;"
        "            text-transform: uppercase;"
        "            letter-spacing: 0.5px;"
        "            margin-bottom: 15px;"
        "            padding-bottom: 10px;"
        "            border-bottom: 2px solid #10b981;"
        "        }"
        "        .info-grid {"
        "            display: grid;"
        "            grid-template-columns: 1fr 1fr;"
        "            gap: 20px;"
        "        }"
        "        .info-item {"
        "            background-color: #f3f4f6;"
        "            padding: 15px;"
        "            border-radius: 6px;"
        "            border-left: 4px solid #10b981;"
        "        }"
        "        .info-item label {"
        "            font-size: 12px;"
        "            color: #64748b;"
        "            text-transform: uppercase;"
        "            letter-spacing: 0.5px;"
        "            display: block;"
        "            margin-bottom: 5px;"
        "        }"
        "        .info-item value {"
        "            font-size: 14px;"
        "            font-weight: 700;"
        "            color: #1e293b;"
        "            display: block;"
        "        }"
        "        .status-badge {"
        "            display: inline-block;"
        "            background-color: %1;"
        "            color: %2;"
        "            padding: 8px 16px;"
        "            border-radius: 20px;"
        "            font-weight: 600;"
        "            font-size: 13px;"
        "        }"
        "        .financial-summary {"
        "            background: linear-gradient(135deg, #f9fafb 0%, #f3f4f6 100%);"
        "            padding: 20px;"
        "            border-radius: 8px;"
        "            margin: 20px 0;"
        "        }"
        "        .summary-row {"
        "            display: flex;"
        "            justify-content: space-between;"
        "            padding: 10px 0;"
        "            font-size: 14px;"
        "        }"
        "        .summary-row.total {"
        "            border-top: 2px solid #e2e8f0;"
        "            padding-top: 15px;"
        "            font-weight: 700;"
        "            font-size: 16px;"
        "            color: #1e293b;"
        "        }"
        "        .summary-row.remaining {"
        "            color: #f59e0b;"
        "            font-weight: 700;"
        "        }"
        "        .summary-row.paid {"
        "            color: #10b981;"
        "            font-weight: 700;"
        "        }"
        "        .label {"
        "            color: #64748b;"
        "        }"
        "        .amount {"
        "            font-weight: 700;"
        "            color: #1e293b;"
        "        }"
        "        .payment-details {"
        "            background-color: %1;"
        "            border-left: 4px solid %2;"
        "            padding: 15px;"
        "            border-radius: 6px;"
        "            margin: 20px 0;"
        "        }"
        "        .payment-details-title {"
        "            font-weight: 700;"
        "            color: %2;"
        "            margin-bottom: 10px;"
        "            font-size: 14px;"
        "        }"
        "        .payment-detail-item {"
        "            font-size: 13px;"
        "            margin: 5px 0;"
        "            color: #1e293b;"
        "        }"
        "        .footer {"
        "            background-color: #f9fafb;"
        "            padding: 20px 30px;"
        "            text-align: center;"
        "            border-top: 1px solid #e2e8f0;"
        "            font-size: 12px;"
        "            color: #64748b;"
        "        }"
        "        .divider {"
        "            height: 1px;"
        "            background-color: #e2e8f0;"
        "            margin: 20px 0;"
        "        }"
        "        .cta-button {"
        "            display: inline-block;"
        "            background: linear-gradient(135deg, #10b981 0%, #059669 100%);"
        "            color: white;"
        "            padding: 12px 30px;"
        "            border-radius: 6px;"
        "            text-decoration: none;"
        "            font-weight: 600;"
        "            font-size: 14px;"
        "            margin: 20px 0;"
        "        }"
        "    </style>"
        "</head>"
        "<body>"
        "    <div class='container'>"
        "        <div class='header'>"
        "            <h1>Reçu de Paiement</h1>"
        "            <p>Commande N°%3 - %4</p>"
        "        </div>"
        "        <div class='content'>"
        "            <div class='greeting'>"
        "                Bonjour <strong>%5 %6</strong>,<br>"
        "                Merci pour votre confiance ! Veuillez trouver ci-dessous le détail de votre commande."
        "            </div>"
        "            <div class='section'>"
        "                <div class='section-title'>Détails de la Commande</div>"
        "                <div class='info-grid'>"
        "                    <div class='info-item'>"
        "                        <label>Numéro de Commande</label>"
        "                        <value>#%3</value>"
        "                    </div>"
        "                    <div class='info-item'>"
        "                        <label>Date</label>"
        "                        <value>%4</value>"
        "                    </div>"
        "                    <div class='info-item'>"
        "                        <label>Adresse</label>"
        "                        <value>%7</value>"
        "                    </div>"
        "                    <div class='info-item'>"
        "                        <label>Téléphone</label>"
        "                        <value>%8</value>"
        "                    </div>"
        "                </div>"
        "            </div>"
        "            <div class='section'>"
        "                <div class='section-title'>Résumé Financier</div>"
        "                <div class='financial-summary'>"
        "                    <div class='summary-row'>"
        "                        <span class='label'>Montant Total :</span>"
        "                        <span class='amount'>%9</span>"
        "                    </div>"
        "                    <div class='summary-row paid'>"
        "                        <span class='label'>Montant Payé :</span>"
        "                        <span class='amount'>%10</span>"
        "                    </div>"
        "                    <div class='summary-row remaining'>"
        "                        <span class='label'>Reste à Payer :</span>"
        "                        <span class='amount'>%11</span>"
        "                    </div>"
        "                </div>"
        "            </div>"
        "            <div class='payment-details'>"
        "                <div class='payment-details-title'>"
        "                    Statut: <span class='status-badge'>%12</span>"
        "                </div>"
        "                <div class='payment-detail-item'>"
        "                    <strong>Mode de paiement :</strong> Mode à confirmer"
        "                </div>"
        "                <div class='payment-detail-item'>"
        "                    <strong>Référence paiement :</strong> %3-%13"
        "                </div>"
        "            </div>"
        "            <div class='divider'></div>"
        "            <div class='section'>"
        "                <p style='color: #64748b; font-size: 13px; margin: 0;'>"
        "                    Si vous avez des questions concernant votre commande, n'hésitez pas à nous contacter."
        "                </p>"
        "            </div>"
        "        </div>"
        "        <div class='footer'>"
        "            <p style='margin: 0; margin-bottom: 10px;'>"
        "                <strong style='color: #1e293b;'>Gestion Vente Matériel</strong>"
        "            </p>"
        "            <p style='margin: 0;'>"
        "                Soldat.rd2lazamana@gmail.com | 034 27 902 53"
        "            </p>"
        "            <p style='margin-top: 15px; border-top: 1px solid #cbd5e1; padding-top: 15px; color: #94a3b8;'>"
        "             Merci pour votre achat !"
        "            </p>"
        "        </div>"
        "    </div>"
        "</body>"
        "</html>"
    )
    .arg(statusBgColor)  // 1 - couleur de fond
    .arg(statusColor)    // 2 - couleur du texte
    .arg(data.order.id)  // 3
    .arg(data.order.date.toString("dd/MM/yyyy HH:mm"))  // 4
    .arg(data.client.nom)  // 5
    .arg(data.client.prenom)  // 6
    .arg(data.client.adresse.isEmpty() ? "N/A" : data.client.adresse)  // 7
    .arg(data.client.telephone.isEmpty() ? "N/A" : data.client.telephone)  // 8
    .arg(QString::number(data.order.total, 'f', 2) + " Ar")  // 9
    .arg(QString::number(data.payment.montant, 'f', 2) + " Ar")  // 10
    .arg(QString::number(remainingAmount, 'f', 2) + " Ar")  // 11
    .arg(orderStatut)  // 12
    .arg(QDateTime::currentDateTime().toString("ddMMyyyyhhmmss"));  // 13

    // Créer un client SMTP
    SmtpClient smtpClient;
    
    // Configuration SMTP à partir du fichier config
    qDebug() << "Configuring SMTP...";
    smtpClient.setSmtpServer(SmtpConfig::SMTP_SERVER, SmtpConfig::SMTP_PORT);
    smtpClient.setCredentials(SmtpConfig::SMTP_EMAIL, SmtpConfig::SMTP_PASSWORD);
    smtpClient.setSenderInfo(SmtpConfig::SENDER_NAME);

    // Envoyer l'email avec pièce jointe
    QString subject = QString("Reçu de paiement - Commande N°%1").arg(data.order.id);
    
    qDebug() << "Attempting to send email...";
    bool success = smtpClient.sendEmailWithAttachment(
        recipientEmail,
        subject,
        emailBody,
        pdfFilePath,
        true  // isHtml = true pour envoyer du contenu HTML
    );

    if (success) {
        qDebug() << "✓ Email sent successfully to" << recipientEmail;
    } else {
        qWarning() << "✗ Failed to send email to" << recipientEmail;
    }

    return success;
}

ReceiptData ReceiptGenerator::getReceiptData(int orderId)
{
    qDebug() << "=== getReceiptData() called with orderId:" << orderId << "===";
    ReceiptData data;

    QSqlQuery query;

    // Récupérer infos commande et client
    query.prepare("SELECT c.nom, c.prenom, c.telephone, c.email, c.adresse, "
                  "cmd.id_commande, cmd.date_commande, cmd.total, cmd.statut, "
                  "u.nom as vendeur "
                  "FROM COMMANDES cmd "
                  "LEFT JOIN CLIENTS c ON cmd.id_client = c.id_client "
                  "LEFT JOIN USERS u ON cmd.id_user = u.id_user "
                  "WHERE cmd.id_commande = ?");
    query.addBindValue(orderId);

    if (query.exec() && query.next()) {
        data.client.nom = query.value("nom").toString();
        data.client.prenom = query.value("prenom").toString();
        data.client.telephone = query.value("telephone").toString();
        data.client.email = query.value("email").toString();
        data.client.adresse = query.value("adresse").toString();

        data.order.id = query.value("id_commande").toInt();
        data.order.date = query.value("date_commande").toDateTime();
        data.order.total = query.value("total").toDouble();
        data.order.statut = query.value("statut").toString();

        data.vendeurName = query.value("vendeur").toString();
        
        qDebug() << "✓ Order data retrieved:";
        qDebug() << "  Client:" << data.client.nom << data.client.prenom;
        qDebug() << "  Email:" << data.client.email;
        qDebug() << "  Order ID:" << data.order.id;
        qDebug() << "  Total:" << data.order.total;
    } else {
        qWarning() << "✗ Failed to retrieve order data:" << query.lastError().text();
        return data;
    }

    // Récupérer détails commande
    query.prepare("SELECT dc.id_produit, p.nom_produit, p.prix_vente, dc.quantite, dc.total "
                  "FROM DETAILS_COMMANDE dc "
                  "LEFT JOIN PRODUITS p ON dc.id_produit = p.id_produit "
                  "WHERE dc.id_commande = ?");
    query.addBindValue(orderId);

    if (query.exec()) {
        int itemCount = 0;
        while (query.next()) {
            OrderItem item;
            item.productName = query.value("nom_produit").toString();
            item.unitPrice = query.value("prix_vente").toDouble();
            item.quantity = query.value("quantite").toInt();
            item.total = query.value("total").toDouble();
            data.items.append(item);
            itemCount++;
        }
        qDebug() << "✓ Order details retrieved:" << itemCount << "items";
    } else {
        qWarning() << "✗ Failed to retrieve order details:" << query.lastError().text();
    }

    // Récupérer infos paiement
    query.prepare("SELECT id_paiement, montant, date_paiement, statut "
                  "FROM PAIEMENTS "
                  "WHERE id_commande = ?");
    query.addBindValue(orderId);

    if (query.exec() && query.next()) {
        data.payment.id = query.value("id_paiement").toInt();
        data.payment.montant = query.value("montant").toDouble();
        data.payment.date = query.value("date_paiement").toDateTime();
        data.payment.statut = query.value("statut").toString();
        
        qDebug() << "✓ Payment data retrieved:";
        qDebug() << "  Payment ID:" << data.payment.id;
        qDebug() << "  Amount:" << data.payment.montant;
    } else {
        qWarning() << "✗ Failed to retrieve payment data:" << query.lastError().text();
    }

    return data;
}
