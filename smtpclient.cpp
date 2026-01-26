#include "smtpclient.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QHostInfo>
#include <QThread>

SmtpClient::SmtpClient(QObject *parent)
    : QObject(parent), socket(nullptr), smtpPort(587)
{
    socket = new QSslSocket(this);
}

SmtpClient::~SmtpClient()
{
    if (socket && socket->isOpen()) {
        socket->disconnectFromHost();
    }
}

void SmtpClient::setSmtpServer(const QString &server, int port)
{
    smtpServer = server;
    smtpPort = port;
}

void SmtpClient::setCredentials(const QString &email, const QString &password)
{
    senderEmail = email;
    senderPassword = password;
}

void SmtpClient::setSenderInfo(const QString &name)
{
    senderName = name;
}

QString SmtpClient::base64Encode(const QString &str)
{
    return str.toLatin1().toBase64();
}

QString SmtpClient::getMimeType(const QString &filePath)
{
    QString extension = filePath.right(filePath.length() - filePath.lastIndexOf(".") - 1).toLower();
    if (extension == "pdf") return "application/pdf";
    if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
    if (extension == "png") return "image/png";
    return "application/octet-stream";
}

QString SmtpClient::encodeFileToBase64(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << filePath;
        return QString();
    }
    
    QByteArray fileData = file.readAll();
    file.close();
    
    return fileData.toBase64();
}

bool SmtpClient::sendCommand(const QString &cmd)
{
    if (!socket->isOpen()) {
        qWarning() << "Socket is not open";
        return false;
    }
    
    qDebug() << "Sending command:" << cmd;
    int bytesWritten = socket->write((cmd + "\r\n").toLatin1());
    qDebug() << "Bytes written:" << bytesWritten;
    socket->flush();
    
    // Ajouter un petit délai pour laisser le serveur traiter
    QThread::msleep(100);
    
    return true;  // La donnée a été écrite avec succès
}

QString SmtpClient::readResponse()
{
    QString response;
    
    do {
        if (!socket->waitForReadyRead(5000)) {
            qWarning() << "Timeout waiting for response";
            break;
        }
        response += QString::fromLatin1(socket->readAll());
    } while (socket->bytesAvailable() > 0);
    
    return response;
}

bool SmtpClient::sendEmailWithAttachment(
    const QString &recipientEmail,
    const QString &subject,
    const QString &body,
    const QString &attachmentPath,
    bool isHtml)
{
    qDebug() << "=== DEBUT ENVOI EMAIL ===";
    qDebug() << "Recipient:" << recipientEmail;
    qDebug() << "Subject:" << subject;
    qDebug() << "Attachment:" << attachmentPath;
    qDebug() << "SMTP Server:" << smtpServer << "Port:" << smtpPort;
    qDebug() << "Sender Email:" << senderEmail;
    
    if (smtpServer.isEmpty() || senderEmail.isEmpty()) {
        qWarning() << "SMTP configuration not set";
        return false;
    }
    
    if (!QFile::exists(attachmentPath)) {
        qWarning() << "Attachment file not found:" << attachmentPath;
        return false;
    }
    
    // Connexion au serveur SMTP
    qDebug() << "Connecting to" << smtpServer << "on port" << smtpPort;
    socket->connectToHost(smtpServer, smtpPort);
    
    if (!socket->waitForConnected(10000)) {
        qWarning() << "Failed to connect to SMTP server";
        return false;
    }
    
    qDebug() << "✓ Connected to SMTP server";
    
    // Attendre la réponse du serveur
    QString response = readResponse();
    qDebug() << "Server response:" << response.trimmed();
    
    if (!response.startsWith("220")) {
        qWarning() << "Unexpected server response";
        socket->disconnectFromHost();
        return false;
    }
    
    // EHLO (avant STARTTLS)
    qDebug() << "Sending EHLO command...";
    if (!sendCommand("EHLO " + QHostInfo::localHostName())) {
        qWarning() << "Failed to send EHLO";
        socket->disconnectFromHost();
        return false;
    }
    response = readResponse();
    qDebug() << "EHLO response:" << response.trimmed();
    
    // STARTTLS
    qDebug() << "Sending STARTTLS...";
    if (!sendCommand("STARTTLS")) {
        qWarning() << "Failed to send STARTTLS";
        socket->disconnectFromHost();
        return false;
    }
    response = readResponse();
    if (!response.startsWith("220")) {
        qWarning() << "STARTTLS failed:" << response.trimmed();
        socket->disconnectFromHost();
        return false;
    }
    qDebug() << "✓ STARTTLS successful";
    
    // Passer à SSL/TLS
    socket->startClientEncryption();
    if (!socket->waitForEncrypted(10000)) {
        qWarning() << "Failed to establish encryption";
        socket->disconnectFromHost();
        return false;
    }
    qDebug() << "✓ Encryption established";
    
    // EHLO à nouveau après TLS
    qDebug() << "Sending EHLO after TLS...";
    if (!sendCommand("EHLO " + QHostInfo::localHostName())) {
        qWarning() << "Failed to send EHLO after TLS";
        socket->disconnectFromHost();
        return false;
    }
    response = readResponse();
    qDebug() << "EHLO after TLS response:" << response.trimmed();
    qDebug() << "✓ EHLO after TLS successful";
    
    // AUTH LOGIN
    qDebug() << "Sending AUTH LOGIN...";
    if (!sendCommand("AUTH LOGIN")) {
        qWarning() << "Failed to send AUTH LOGIN";
        socket->disconnectFromHost();
        return false;
    }
    response = readResponse();
    if (!response.startsWith("334")) {
        qWarning() << "AUTH LOGIN failed:" << response.trimmed();
        socket->disconnectFromHost();
        return false;
    }
    qDebug() << "✓ AUTH LOGIN initiated";
    
    // Envoyer l'email encodé en base64
    qDebug() << "Sending email address...";
    if (!sendCommand(base64Encode(senderEmail))) {
        qWarning() << "Failed to send email address";
        socket->disconnectFromHost();
        return false;
    }
    response = readResponse();
    if (!response.startsWith("334")) {
        qWarning() << "Email address rejected:" << response.trimmed();
        socket->disconnectFromHost();
        return false;
    }
    qDebug() << "✓ Email address accepted";
    
    // Envoyer le mot de passe encodé en base64
    qDebug() << "Sending password...";
    if (!sendCommand(base64Encode(senderPassword))) {
        qWarning() << "Failed to send password";
        socket->disconnectFromHost();
        return false;
    }
    response = readResponse();
    if (!response.startsWith("235")) {
        qWarning() << "Authentication failed:" << response.trimmed();
        socket->disconnectFromHost();
        return false;
    }
    qDebug() << "✓ Authentication successful";
    
    // MAIL FROM
    qDebug() << "Sending MAIL FROM...";
    if (!sendCommand("MAIL FROM:<" + senderEmail + ">")) {
        qWarning() << "Failed to send MAIL FROM";
        socket->disconnectFromHost();
        return false;
    }
    response = readResponse();
    if (!response.startsWith("250")) {
        qWarning() << "MAIL FROM failed:" << response.trimmed();
        socket->disconnectFromHost();
        return false;
    }
    qDebug() << "✓ MAIL FROM accepted";
    
    // RCPT TO
    qDebug() << "Sending RCPT TO...";
    if (!sendCommand("RCPT TO:<" + recipientEmail + ">")) {
        qWarning() << "Failed to send RCPT TO";
        socket->disconnectFromHost();
        return false;
    }
    response = readResponse();
    if (!response.startsWith("250")) {
        qWarning() << "RCPT TO failed:" << response.trimmed();
        socket->disconnectFromHost();
        return false;
    }
    qDebug() << "✓ RCPT TO accepted";
    
    // DATA
    qDebug() << "Sending DATA...";
    if (!sendCommand("DATA")) {
        qWarning() << "Failed to send DATA";
        socket->disconnectFromHost();
        return false;
    }
    response = readResponse();
    if (!response.startsWith("354")) {
        qWarning() << "DATA failed:" << response.trimmed();
        socket->disconnectFromHost();
        return false;
    }
    qDebug() << "✓ DATA accepted";
    
    // Construire le message MIME avec pièce jointe
    QFileInfo fileInfo(attachmentPath);
    QString fileName = fileInfo.fileName();
    QString mimeType = getMimeType(attachmentPath);
    QString fileBase64 = encodeFileToBase64(attachmentPath);
    
    QString contentType = isHtml ? "text/html" : "text/plain";
    
    QString message = QString(
        "From: %1 <%2>\r\n"
        "To: %3\r\n"
        "Subject: %4\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: multipart/mixed; boundary=\"===============boundary123456789==\"\r\n"
        "Content-Transfer-Encoding: 7bit\r\n"
        "\r\n"
        "--===============boundary123456789==\r\n"
        "Content-Type: %9; charset=\"utf-8\"\r\n"
        "Content-Transfer-Encoding: 7bit\r\n"
        "\r\n"
        "%5\r\n"
        "\r\n"
        "--===============boundary123456789==\r\n"
        "Content-Type: %6; name=\"%7\"\r\n"
        "Content-Transfer-Encoding: base64\r\n"
        "Content-Disposition: attachment; filename=\"%7\"\r\n"
        "\r\n"
        "%8\r\n"
        "\r\n"
        "--===============boundary123456789==--\r\n"
        ".\r\n"
    )
    .arg(senderName.isEmpty() ? "System" : senderName)
    .arg(senderEmail)
    .arg(recipientEmail)
    .arg(subject)
    .arg(body)
    .arg(mimeType)
    .arg(fileName)
    .arg(fileBase64)
    .arg(contentType);
    
    // Envoyer le message
    qDebug() << "Sending email message...";
    socket->write(message.toLatin1());
    socket->flush();
    
    response = readResponse();
    if (!response.startsWith("250")) {
        qWarning() << "Message sending failed:" << response.trimmed();
        socket->disconnectFromHost();
        return false;
    }
    qDebug() << "✓ Message sent successfully";
    
    // QUIT
    qDebug() << "Sending QUIT...";
    sendCommand("QUIT");
    socket->waitForBytesWritten(5000);
    socket->disconnectFromHost();
    
    qDebug() << "✓ Email sent successfully!";
    return true;
}
