#include "thememanager.h"
#include <QSettings>
#include <QApplication>
#include <QFile>
#include <QTextStream>

ThemeManager& ThemeManager::instance()
{
    static ThemeManager _instance;
    return _instance;
}

ThemeManager::ThemeManager()
    : QObject(), m_currentTheme(DarkMode)
{
    // Force le mode sombre uniquement
    m_currentTheme = DarkMode;
    loadThemePreference();
}

ThemeManager::Theme ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

void ThemeManager::setTheme(Theme theme)
{
    // Force toujours le mode sombre
    theme = DarkMode;
    
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        saveThemePreference();
        
        // Charge toujours le fichier dark theme
        QString qssFile = ":/themes/dark_theme.qss";
        
        QFile file(qssFile);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&file);
            QString stylesheet = stream.readAll();
            file.close();
            qApp->setStyleSheet(stylesheet);
        }
        
        emit themeChanged(theme);
    }
}

void ThemeManager::toggleTheme()
{
    // Ne fait rien - le mode sombre est forcé
    return;
}

// Couleurs générales
QColor ThemeManager::backgroundColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.background : darkColors.background;
}

QColor ThemeManager::surfaceColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.surface : darkColors.surface;
}

QColor ThemeManager::surfaceAltColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.surfaceAlt : darkColors.surfaceAlt;
}

QColor ThemeManager::textColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.text : darkColors.text;
}

QColor ThemeManager::textSecondaryColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.textSecondary : darkColors.textSecondary;
}

QColor ThemeManager::textTertiaryColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.textTertiary : darkColors.textTertiary;
}

QColor ThemeManager::borderColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.border : darkColors.border;
}

QColor ThemeManager::borderLightColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.borderLight : darkColors.borderLight;
}

// Couleurs primaires
QColor ThemeManager::primaryColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.primary : darkColors.primary;
}

QColor ThemeManager::primaryHoverColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.primaryHover : darkColors.primaryHover;
}

QColor ThemeManager::primaryPressedColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.primaryPressed : darkColors.primaryPressed;
}

QColor ThemeManager::primaryLightColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.primaryLight : darkColors.primaryLight;
}

// Couleurs d'état
QColor ThemeManager::dangerColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.danger : darkColors.danger;
}

QColor ThemeManager::dangerHoverColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.dangerHover : darkColors.dangerHover;
}

QColor ThemeManager::dangerPressedColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.dangerPressed : darkColors.dangerPressed;
}

QColor ThemeManager::successColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.success : darkColors.success;
}

QColor ThemeManager::warningColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.warning : darkColors.warning;
}

QColor ThemeManager::infoColor() const
{
    return (m_currentTheme == LightMode) ? lightColors.info : darkColors.info;
}

// Éléments spécifiques
QColor ThemeManager::panelBackground() const
{
    return (m_currentTheme == LightMode) ? QColor("#ffffff") : QColor("#1a2a3a");
}

QColor ThemeManager::cardBackground() const
{
    return (m_currentTheme == LightMode) ? QColor("#f8fafc") : QColor("#1f3346");
}

QColor ThemeManager::cardBorder() const
{
    return (m_currentTheme == LightMode) ? QColor("#e2e8f0") : QColor("#3d5567");
}

QColor ThemeManager::tableHeaderBackground() const
{
    return (m_currentTheme == LightMode) ? QColor("#f1f5f9") : QColor("#1a2a3a");
}

QColor ThemeManager::tableRowAltBackground() const
{
    return (m_currentTheme == LightMode) ? QColor("#f8fafc") : QColor("#1f3346");
}

QColor ThemeManager::tableRowHoverBackground() const
{
    return (m_currentTheme == LightMode) ? QColor("#f1f5f9") : QColor("#2d4456");
}

QColor ThemeManager::inputBackground() const
{
    return (m_currentTheme == LightMode) ? QColor("#ffffff") : QColor("#1f3346");
}

QColor ThemeManager::inputBorder() const
{
    return (m_currentTheme == LightMode) ? QColor("#e2e8f0") : QColor("#3d5567");
}

QColor ThemeManager::inputFocusBorder() const
{
    return primaryColor();
}

QColor ThemeManager::disabledColor() const
{
    return (m_currentTheme == LightMode) ? QColor("#cbd5e1") : QColor("#64748b");
}

QString ThemeManager::getTableStylesheet() const
{
    QString bgColor = backgroundColor().name();
    QString surfaceColor_str = surfaceColor().name();
    QString textColor_str = textColor().name();
    QString textSecondary = textSecondaryColor().name();
    QString borderColor_str = borderColor().name();
    QString headerBg = tableHeaderBackground().name();
    
    return QString(
        "QTableWidget {"
        "   background: %1;"
        "   border: 1px solid %2;"
        "   border-radius: 8px;"
        "   font-size: 13px;"
        "   color: %3;"
        "   gridline-color: %2;"
        "}"
        "QTableWidget::item {"
        "   padding: 12px 16px;"
        "   border: none;"
        "}"
        "QTableWidget::item:selected {"
        "   background: %2;"
        "   color: %3;"
        "}"
        "QHeaderView::section {"
        "   background: %4;"
        "   color: %3;"
        "   padding: 12px 16px;"
        "   border: none;"
        "   border-bottom: 2px solid %2;"
        "   font-weight: 600;"
        "   font-size: 12px;"
        "   text-align: left;"
        "}"
    ).arg(surfaceColor_str, borderColor_str, textColor_str, headerBg);
}

QString ThemeManager::getInputStylesheet() const
{
    QString inputBg = inputBackground().name();
    QString inputBrd = inputBorder().name();
    QString focusBrd = inputFocusBorder().name();
    QString textCol = textColor().name();
    
    return QString(
        "QLineEdit, QTextEdit, QComboBox {"
        "   background: %1;"
        "   color: %3;"
        "   border: 2px solid %2;"
        "   border-radius: 6px;"
        "   padding: 10px;"
        "   font-size: 13px;"
        "}"
        "QLineEdit:focus, QTextEdit:focus, QComboBox:focus {"
        "   border: 2px solid %4;"
        "}"
    ).arg(inputBg, inputBrd, textCol, focusBrd);
}

QString ThemeManager::getButtonStylesheet(const QString &color) const
{
    QColor btnColor, btnHover, btnPressed;
    
    if (color == "primary") {
        btnColor = primaryColor();
        btnHover = primaryHoverColor();
        btnPressed = primaryPressedColor();
    } else if (color == "danger") {
        btnColor = dangerColor();
        btnHover = dangerHoverColor();
        btnPressed = dangerPressedColor();
    } else if (color == "success") {
        btnColor = successColor();
        btnHover = successColor();
        btnPressed = successColor();
    } else {
        btnColor = primaryColor();
        btnHover = primaryHoverColor();
        btnPressed = primaryPressedColor();
    }
    
    return QString(
        "QPushButton {"
        "   background: %1;"
        "   color: white;"
        "   border: 2px solid %1;"
        "   border-radius: 6px;"
        "   padding: 10px 20px;"
        "   font-weight: 600;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: %2;"
        "   border-color: %2;"
        "}"
        "QPushButton:pressed {"
        "   background: %3;"
        "   border-color: %3;"
        "}"
        "QPushButton:disabled {"
        "   background: %4;"
        "   color: %5;"
        "   border-color: %4;"
        "}"
    ).arg(btnColor.name(), btnHover.name(), btnPressed.name(), 
          disabledColor().name(), textTertiaryColor().name());
}

QString ThemeManager::getCompleteStylesheet() const
{
    QString bgColor = backgroundColor().name();
    QString surfaceColor_str = surfaceColor().name();
    QString textColor_str = textColor().name();
    QString textSecondary = textSecondaryColor().name();
    QString borderColor_str = borderColor().name();
    QString primaryColor_str = primaryColor().name();
    QString primaryHover_str = primaryHoverColor().name();
    QString primaryPressed_str = primaryPressedColor().name();
    
    return QString(
        "QMainWindow, QDialog, QWidget, QFrame { background: %1; color: %2; }"
        "%3%4%5"
    ).arg(bgColor, textColor_str, getTableStylesheet(), getInputStylesheet(), 
          getButtonStylesheet());
}

QString ThemeManager::getCardStylesheet() const
{
    QString cardBg = cardBackground().name();
    QString cardBrd = cardBorder().name();
    QString textCol = textColor().name();
    
    return QString(
        "background: %1;"
        "border: 1px solid %2;"
        "border-radius: 8px;"
        "padding: 16px;"
        "color: %3;"
    ).arg(cardBg, cardBrd, textCol);
}

QString ThemeManager::getDialogStylesheet() const
{
    QString bgColor = backgroundColor().name();
    QString textCol = textColor().name();
    QString surfaceCol = surfaceColor().name();
    
    return QString(
        "QDialog {"
        "   background: %1;"
        "   color: %2;"
        "}"
        "QLabel {"
        "   color: %2;"
        "}"
        "QGroupBox {"
        "   background: %3;"
        "   color: %2;"
        "   border: 1px solid %1;"
        "   border-radius: 6px;"
        "   padding: 12px;"
        "   margin-top: 12px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 10px;"
        "   padding: 0 3px 0 3px;"
        "}"
    ).arg(bgColor, textCol, surfaceCol);
}

void ThemeManager::saveThemePreference()
{
    QSettings settings("GestionVente", "GestionVenteMateriel");
    settings.setValue("theme", (m_currentTheme == LightMode) ? "light" : "dark");
}

void ThemeManager::loadThemePreference()
{
    QSettings settings("GestionVente", "GestionVenteMateriel");
    QString theme = settings.value("theme", "light").toString();
    m_currentTheme = (theme == "dark") ? DarkMode : LightMode;
}
