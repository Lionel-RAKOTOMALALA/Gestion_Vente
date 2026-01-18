#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QString>
#include <QColor>
#include <QMap>
#include <QObject>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme {
        LightMode,
        DarkMode
    };

    static ThemeManager& instance();
    
    Theme currentTheme() const;
    void setTheme(Theme theme);
    void toggleTheme();
    
    // Couleurs générales
    QColor backgroundColor() const;
    QColor surfaceColor() const;
    QColor surfaceAltColor() const;
    QColor textColor() const;
    QColor textSecondaryColor() const;
    QColor textTertiaryColor() const;
    QColor borderColor() const;
    QColor borderLightColor() const;
    
    // Couleurs primaires
    QColor primaryColor() const;
    QColor primaryHoverColor() const;
    QColor primaryPressedColor() const;
    QColor primaryLightColor() const;
    
    // Couleurs d'état
    QColor dangerColor() const;
    QColor dangerHoverColor() const;
    QColor dangerPressedColor() const;
    QColor successColor() const;
    QColor warningColor() const;
    QColor infoColor() const;
    
    // Couleurs spécifiques pour les éléments
    QColor panelBackground() const;
    QColor cardBackground() const;
    QColor cardBorder() const;
    QColor tableHeaderBackground() const;
    QColor tableRowAltBackground() const;
    QColor tableRowHoverBackground() const;
    QColor inputBackground() const;
    QColor inputBorder() const;
    QColor inputFocusBorder() const;
    QColor disabledColor() const;
    
    // Stylesheets complets
    QString getTableStylesheet() const;
    QString getInputStylesheet() const;
    QString getButtonStylesheet(const QString &color = "primary") const;
    QString getCompleteStylesheet() const;
    QString getCardStylesheet() const;
    QString getDialogStylesheet() const;
    
    // Sauvegarde/Récupération de la préférence
    void saveThemePreference();
    void loadThemePreference();

signals:
    void themeChanged(Theme theme);

private:
    ThemeManager();
    ~ThemeManager() = default;
    
    Theme m_currentTheme;
    
    struct LightColors {
        QColor background = QColor("#ffffff");
        QColor surface = QColor("#f8fafc");
        QColor surfaceAlt = QColor("#f1f5f9");
        QColor text = QColor("#0f172a");
        QColor textSecondary = QColor("#475569");
        QColor textTertiary = QColor("#94a3b8");
        QColor border = QColor("#e2e8f0");
        QColor borderLight = QColor("#f1f5f9");
        QColor primary = QColor("#2563eb");
        QColor primaryHover = QColor("#1d4ed8");
        QColor primaryPressed = QColor("#1e40af");
        QColor primaryLight = QColor("#dbeafe");
        QColor danger = QColor("#dc2626");
        QColor dangerHover = QColor("#b91c1c");
        QColor dangerPressed = QColor("#991b1b");
        QColor success = QColor("#16a34a");
        QColor warning = QColor("#ea580c");
        QColor info = QColor("#0284c7");
    } lightColors;
    
    struct DarkColors {
        QColor background = QColor("#0d1b2a");      // Deep navy blue base
        QColor surface = QColor("#1a2a3a");         // Slightly elevated surface
        QColor surfaceAlt = QColor("#243447");      // Alt surface variation
        QColor text = QColor("#f0f7ff");            // Light blue-tinted white
        QColor textSecondary = QColor("#b0c4de");   // Medium gray-blue
        QColor textTertiary = QColor("#7a8fa3");    // Darker gray-blue
        QColor border = QColor("#2d4456");          // Deep blue border
        QColor borderLight = QColor("#3d5567");     // Lighter border
        QColor primary = QColor("#4b9eff");         // Modern bright blue
        QColor primaryHover = QColor("#5faeff");    // Hover state
        QColor primaryPressed = QColor("#3a7dd1");  // Pressed state
        QColor primaryLight = QColor("#1e3a4c");    // Light tint
        QColor danger = QColor("#ff6b6b");          // Modern red
        QColor dangerHover = QColor("#ff8888");     // Hover red
        QColor dangerPressed = QColor("#cc5555");   // Pressed red
        QColor success = QColor("#26d07c");         // Modern green
        QColor warning = QColor("#ff9d3c");         // Modern orange
        QColor info = QColor("#00d4ff");            // Modern cyan
    } darkColors;
};

#endif // THEMEMANAGER_H
