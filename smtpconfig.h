#ifndef SMTPCONFIG_H
#define SMTPCONFIG_H

#include <QString>

class SmtpConfig
{
public:
    // Configuration SMTP - À modifier avec vos paramètres
    static constexpr const char* SMTP_SERVER = "smtp.gmail.com";
    static constexpr int SMTP_PORT = 587;
    
    // Identifiants SMTP - À remplacer avec votre email et mot de passe
    static constexpr const char* SMTP_EMAIL = "rakotomalalalionel32@gmail.com";
    static constexpr const char* SMTP_PASSWORD = "nvbsaozubtexuuzd";
    
    // Nom de l'expéditeur
    static constexpr const char* SENDER_NAME = "Gestion Vente Materiel";
};

#endif // SMTPCONFIG_H
