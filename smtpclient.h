#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QString>
#include <QSslSocket>
#include <QObject>

class SmtpClient : public QObject
{
    Q_OBJECT

public:
    SmtpClient(QObject *parent = nullptr);
    ~SmtpClient();

    // Configuration SMTP
    void setSmtpServer(const QString &server, int port = 587);
    void setCredentials(const QString &email, const QString &password);
    void setSenderInfo(const QString &senderName);

    // Envoyer email avec pièce jointe
    bool sendEmailWithAttachment(
        const QString &recipientEmail,
        const QString &subject,
        const QString &body,
        const QString &attachmentPath,
        bool isHtml = false
    );

private:
    QString base64Encode(const QString &str);
    QString getMimeType(const QString &filePath);
    QString encodeFileToBase64(const QString &filePath);
    bool sendCommand(const QString &cmd);
    QString readResponse();

    QSslSocket *socket;
    QString smtpServer;
    int smtpPort;
    QString senderEmail;
    QString senderPassword;
    QString senderName;
};

#endif // SMTPCLIENT_H

