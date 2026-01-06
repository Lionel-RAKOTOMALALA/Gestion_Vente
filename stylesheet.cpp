#include "stylesheet.h"

// Couleurs primaires
const QString StyleSheet::PRIMARY = "#2563eb";
const QString StyleSheet::SECONDARY = "#64748b";
const QString StyleSheet::ACCENT = "#8b5cf6";

// Couleurs fonctionnelles
const QString StyleSheet::SUCCESS = "#10b981";
const QString StyleSheet::WARNING = "#f59e0b";
const QString StyleSheet::ERROR = "#ef4444";
const QString StyleSheet::INFO = "#3b82f6";

// Neutres
const QString StyleSheet::BACKGROUND = "#f8fafc";
const QString StyleSheet::SURFACE = "#ffffff";
const QString StyleSheet::BORDER = "#e2e8f0";
const QString StyleSheet::TEXT_PRIMARY = "#0f172a";
const QString StyleSheet::TEXT_SECONDARY = "#64748b";

// Typographie
const QString StyleSheet::FONT_FAMILY = "Segoe UI, system-ui, -apple-system, sans-serif";

StyleSheet::StyleSheet() {}

QString StyleSheet::getStyleSheet()
{
    return QString(
        "* {"
        "   font-family: %1;"
        "}"
        ""
        "QWidget {"
        "   background-color: %2;"
        "   color: %3;"
        "}"
        ""
        "#mainWindow {"
        "   background-color: %2;"
        "}"
        ""
        "/* ==== DASHBOARD STYLES ==== */"
        ""
        "#dashboardPage {"
        "   background-color: %2;"
        "}"
        ""
        "/* Cartes dashboard avec glassmorphism */"
        "#dashboardCard {"
        "   background: rgba(30, 41, 59, 0.7);"
        "   border: 1px solid rgba(255, 255, 255, 0.1);"
        "   border-radius: 18px;"
        "   min-height: 180px;"
        "   max-height: 220px;"
        "}"

        "#dashboardCard:hover {"
        "   background: rgba(30, 41, 59, 0.85);"
        "   border-color: rgba(255, 255, 255, 0.2);"
        "}"
        ""
        "/* Dégradé violet avec glassmorphism */"
        "#dashboardCard[colorClass=\"purple\"] {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "       stop:0 rgba(124, 58, 237, 0.6), stop:1 rgba(167, 139, 250, 0.5));"
        "   border: 1px solid rgba(124, 58, 237, 0.3);"
        "}"

        "#dashboardCard[colorClass=\"purple\"]:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "       stop:0 rgba(124, 58, 237, 0.8), stop:1 rgba(167, 139, 250, 0.7));"
        "   border-color: rgba(124, 58, 237, 0.6);"
        "}"
        ""
        "/* Dégradé orange avec glassmorphism */"
        "#dashboardCard[colorClass=\"orange\"] {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "       stop:0 rgba(249, 115, 22, 0.6), stop:1 rgba(251, 146, 60, 0.5));"
        "   border: 1px solid rgba(249, 115, 22, 0.3);"
        "}"

        "#dashboardCard[colorClass=\"orange\"]:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "       stop:0 rgba(249, 115, 22, 0.8), stop:1 rgba(251, 146, 60, 0.7));"
        "   border-color: rgba(249, 115, 22, 0.6);"
        "}"
        ""
        "/* Dégradé rose avec glassmorphism */"
        "#dashboardCard[colorClass=\"pink\"] {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "       stop:0 rgba(219, 39, 119, 0.6), stop:1 rgba(244, 114, 182, 0.5));"
        "   border: 1px solid rgba(219, 39, 119, 0.3);"
        "}"

        "#dashboardCard[colorClass=\"pink\"]:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "       stop:0 rgba(219, 39, 119, 0.8), stop:1 rgba(244, 114, 182, 0.7));"
        "   border-color: rgba(219, 39, 119, 0.6);"
        "}"
        ""
        "/* Dégradé cyan avec glassmorphism */"
        "#dashboardCard[colorClass=\"cyan\"] {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "       stop:0 rgba(8, 145, 178, 0.6), stop:1 rgba(34, 211, 238, 0.5));"
        "   border: 1px solid rgba(8, 145, 178, 0.3);"
        "}"

        "#dashboardCard[colorClass=\"cyan\"]:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "       stop:0 rgba(8, 145, 178, 0.8), stop:1 rgba(34, 211, 238, 0.7));"
        "   border-color: rgba(8, 145, 178, 0.6);"
        "}"
        ""
        "/* Icônes et textes en blanc sur fond coloré */"
        "#dashboardIcon {"
        "   color: rgba(255, 255, 255, 0.98);"
        "   background-color: transparent;"
        "}"
        ""
        "#dashboardCardTitle {"
        "   color: rgba(255, 255, 255, 0.92);"
        "   font-weight: 500;"
        "   text-transform: uppercase;"
        "}"
        ""
        "#dashboardCardValue {"
        "   color: rgb(255, 255, 255);"
        "   font-weight: 700;"
        "}"
        ""
        "/* Panel analytique amélioré */"
        "#analyticsPanel {"
        "   background-color: %4;"
        "   border: 1px solid %5;"
        "   border-radius: 18px;"
        "}"
        ""
        "/* Titres avec meilleure hiérarchie */"
        "#titleH1 {"
        "   color: %3;"
        "   font-weight: 700;"
        "}"
        ""
        "#titleH2 {"
        "   color: %3;"
        "   font-weight: 600;"
        "}"
        ""
        "#subtitle {"
        "   color: %8;"
        "   font-weight: 400;"
        "}"
        ""
        "#placeholder {"
        "   color: %8;"
        "   font-weight: 400;"
        "   padding: 100px 40px;"
        "}"
        ""
        "/* ==== SIDEBAR STYLES ==== */"
        ""
        "#sidebar {"
        "   background-color: %4;"
        "   border-right: 1px solid %5;"
        "}"
        ""
        "#sidebarTitle {"
        "   color: %3;"
        "   font-size: 15px;"
        "   font-weight: 700;"
        "   letter-spacing: 1.8px;"
        "   padding: 14px 0;"
        "}"
        ""
        "/* Boutons sidebar modernisés */"
        "#sidebarButton {"
        "   background-color: transparent;"
        "   color: %8;"
        "   border: none;"
        "   border-radius: 10px;"
        "   padding: 13px 16px;"
        "   text-align: left;"
        "   font-size: 14px;"
        "   font-weight: 500;"
        "   margin: 2px 0;"
        "}"
        ""
        "#sidebarButton:hover {"
        "   background-color: %2;"
        "   color: %3;"
        "}"
        ""
        "#sidebarButton:checked {"
        "   background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "       stop:0 %6, stop:1 %7);"
        "   color: %4;"
        "   font-weight: 600;"
        "}"
    ).arg(FONT_FAMILY)      // %1
     .arg(BACKGROUND)       // %2
     .arg(TEXT_PRIMARY)     // %3
     .arg(SURFACE)          // %4
     .arg(BORDER)           // %5
     .arg(PRIMARY)          // %6
     .arg(ACCENT)           // %7
     .arg(TEXT_SECONDARY);  // %8
}
