#include "stylesheet.h"
#include "thememanager.h"

const QString StyleSheet::PRIMARY = "#4b9eff";
const QString StyleSheet::SECONDARY = "#7a8fa3";
const QString StyleSheet::ACCENT = "#7c5cff";

const QString StyleSheet::SUCCESS = "#26d07c";
const QString StyleSheet::WARNING = "#ff9d3c";
const QString StyleSheet::ERROR = "#ff6b6b";
const QString StyleSheet::INFO = "#00d4ff";

const QString StyleSheet::BACKGROUND = "#0d1b2a";
const QString StyleSheet::SURFACE = "#1a2a3a";
const QString StyleSheet::BORDER = "#2d4456";
const QString StyleSheet::TEXT_PRIMARY = "#f0f7ff";
const QString StyleSheet::TEXT_SECONDARY = "#b0c4de";

// Typographie
const QString StyleSheet::FONT_FAMILY = "Segoe UI, system-ui, -apple-system, sans-serif";

StyleSheet::StyleSheet() {}

QString StyleSheet::getStyleSheet()
{
    ThemeManager& theme = ThemeManager::instance();
    
    QString bgColor = theme.backgroundColor().name();
    QString surfaceColor_str = theme.surfaceColor().name();
    QString textColor_str = theme.textColor().name();
    QString textSecondary = theme.textSecondaryColor().name();
    QString borderColor_str = theme.borderColor().name();
    QString primaryColor_str = theme.primaryColor().name();
    
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
        "/* Modern dashboard cards with sophisticated dark mode support */"
        "#dashboardCard {"
        "   background: %4;"
        "   border: 1px solid %5;"
        "   border-radius: 16px;"
        "   min-height: 240px;"
        "   max-height: 280px;"
        "   padding: 0px;"
        "   transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);"
        "}"

        "#dashboardCard:hover {"
        "   background: %5;"
        "   border-color: %6;"
        "   transform: translateY(-4px);"
        "}"
        ""
        "/* Subtle gradient overlays for color variety with improved visibility */"
        "#dashboardCard[colorClass=\"purple\"] {"
        "   background: linear-gradient(135deg, rgba(124, 92, 255, 0.15) 0%, rgba(147, 112, 255, 0.08) 100%), %4;"
        "   border: 1.5px solid rgba(124, 92, 255, 0.4);"
        "}"

        "#dashboardCard[colorClass=\"purple\"]:hover {"
        "   background: linear-gradient(135deg, rgba(124, 92, 255, 0.25) 0%, rgba(147, 112, 255, 0.15) 100%), %4;"
        "   border-color: rgba(124, 92, 255, 0.6);"
        "}"
        ""
        "#dashboardCard[colorClass=\"orange\"] {"
        "   background: linear-gradient(135deg, rgba(255, 157, 60, 0.15) 0%, rgba(255, 179, 102, 0.08) 100%), %4;"
        "   border: 1.5px solid rgba(255, 157, 60, 0.4);"
        "}"

        "#dashboardCard[colorClass=\"orange\"]:hover {"
        "   background: linear-gradient(135deg, rgba(255, 157, 60, 0.25) 0%, rgba(255, 179, 102, 0.15) 100%), %4;"
        "   border-color: rgba(255, 157, 60, 0.6);"
        "}"
        ""
        "#dashboardCard[colorClass=\"pink\"] {"
        "   background: linear-gradient(135deg, rgba(255, 107, 107, 0.15) 0%, rgba(255, 136, 136, 0.08) 100%), %4;"
        "   border: 1.5px solid rgba(255, 107, 107, 0.4);"
        "}"

        "#dashboardCard[colorClass=\"pink\"]:hover {"
        "   background: linear-gradient(135deg, rgba(255, 107, 107, 0.25) 0%, rgba(255, 136, 136, 0.15) 100%), %4;"
        "   border-color: rgba(255, 107, 107, 0.6);"
        "}"
        ""
        "#dashboardCard[colorClass=\"cyan\"] {"
        "   background: linear-gradient(135deg, rgba(0, 212, 255, 0.15) 0%, rgba(0, 230, 255, 0.08) 100%), %4;"
        "   border: 1.5px solid rgba(0, 212, 255, 0.4);"
        "}"

        "#dashboardCard[colorClass=\"cyan\"]:hover {"
        "   background: linear-gradient(135deg, rgba(0, 212, 255, 0.25) 0%, rgba(0, 230, 255, 0.15) 100%), %4;"
        "   border-color: rgba(0, 212, 255, 0.6);"
        "}"
        ""
        "#dashboardIcon {"
        "   color: %3;"
        "   background-color: transparent;"
        "   font-size: 48px;"
        "}"
        ""
        "#dashboardCardTitle {"
        "   color: %8;"
        "   font-weight: 600;"
        "   letter-spacing: 0.8px;"
        "   text-transform: uppercase;"
        "   font-size: 11px;"
        "}"
        ""
        "#dashboardCardValue {"
        "   color: %3;"
        "   font-weight: 700;"
        "   font-size: 36px;"
        "}"
        ""
        "/* Premium analytics panel */"
        "#analyticsPanel {"
        "   background-color: %4;"
        "   border: 1px solid %5;"
        "   border-radius: 16px;"
        "   padding: 0px;"
        "}"
        ""
        "#chartFrame {"
        "   background-color: %4;"
        "   border: 1px solid %5;"
        "   border-radius: 14px;"
        "   padding: 0px;"
        "}"
        ""
        "/* Enhanced typography hierarchy */"
        "#titleH1 {"
        "   color: %3;"
        "   font-weight: 700;"
        "   letter-spacing: -0.8px;"
        "   font-size: 32px;"
        "}"
        ""
        "#titleH2 {"
        "   color: %3;"
        "   font-weight: 600;"
        "   letter-spacing: 0.5px;"
        "   font-size: 20px;"
        "}"
        ""
        "#subtitle {"
        "   color: %8;"
        "   font-weight: 400;"
        "   line-height: 1.6;"
        "   font-size: 14px;"
        "}"
        ""
        "#placeholder {"
        "   color: %8;"
        "   font-weight: 400;"
        "   padding: 100px 40px;"
        "   font-size: 16px;"
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
        "#sidebarButton {"
        "   background-color: transparent;"
        "   color: %8;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 14px 18px;"
        "   text-align: left;"
        "   font-size: 14px;"
        "   font-weight: 500;"
        "   margin: 3px 0;"
        "   transition: all 0.2s ease;"
        "}"
        ""
        "#sidebarButton:hover {"
        "   background-color: rgba(75, 158, 255, 0.1);"
        "   color: %3;"
        "}"
        ""
        "#sidebarButton:checked {"
        "   background-color: %6;"
        "   color: white;"
        "   font-weight: 600;"
        "}"
    ).arg(FONT_FAMILY)      // %1
     .arg(bgColor)          // %2
     .arg(textColor_str)    // %3
     .arg(surfaceColor_str) // %4
     .arg(borderColor_str)  // %5
     .arg(primaryColor_str) // %6
     .arg(ACCENT)           // %7
     .arg(textSecondary);   // %8
}
