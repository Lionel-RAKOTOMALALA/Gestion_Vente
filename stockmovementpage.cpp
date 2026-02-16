#include "stockmovementpage.h"
#include "thememanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <QInputDialog>
#include <QSpinBox>
#include <QDate>
#include <QPdfWriter>
#include <QPainter>
#include <QStandardPaths>
#include <QFileDialog>

StockMovementPage::StockMovementPage(const QString &userRole, int userId, QWidget *parent)
    : QFrame(parent), userRole(userRole), userId(userId)
{
    setObjectName("stockMovementPage");
    setupDatabase();
    setupUI();
    applyStyles();
    loadStatistics();
    loadStockMovements();
}

void StockMovementPage::setupDatabase()
{
    QSqlDatabase db = QSqlDatabase::database();
    
    if (!db.isOpen()) {
        qWarning() << "Database is not open";
        return;
    }

    QSqlQuery query;
    
    // Create STOCK_MOVEMENTS table
    QString createMovementTable = "CREATE TABLE IF NOT EXISTS STOCK_MOVEMENTS ("
                                  "id_movement INTEGER PRIMARY KEY AUTOINCREMENT,"
                                  "id_product INTEGER NOT NULL,"
                                  "type TEXT NOT NULL CHECK (type IN ('ENTREE', 'SORTIE', 'AJUSTEMENT')),"
                                  "quantite INTEGER NOT NULL,"
                                  "motif TEXT,"
                                  "date_mouvement DATETIME DEFAULT CURRENT_TIMESTAMP,"
                                  "id_user INTEGER NOT NULL,"
                                  "FOREIGN KEY(id_product) REFERENCES PRODUITS(id_produit),"
                                  "FOREIGN KEY(id_user) REFERENCES USERS(id_user)"
                                  ");";
    
    if (!query.exec(createMovementTable)) {
        qDebug() << "Error creating STOCK_MOVEMENTS table:" << query.lastError().text();
    } else {
        qDebug() << "STOCK_MOVEMENTS table created or already exists.";
    }

    // Ensure PRODUITS table exists and has a stock column
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='PRODUITS'");
    bool produitsExists = false;
    if (query.next()) produitsExists = true;

    if (!produitsExists) {
        qDebug() << "PRODUITS table not found, creating a minimal PRODUITS table.";
        QString createProduits = "CREATE TABLE IF NOT EXISTS PRODUITS ("
                                 "id_produit INTEGER PRIMARY KEY AUTOINCREMENT,"
                                 "nom_produit TEXT NOT NULL,"
                                 "description TEXT,"
                                 "prix_vente REAL DEFAULT 0,"
                                 "stock INTEGER DEFAULT 0"
                                 ");";
        if (!query.exec(createProduits)) {
            qDebug() << "Error creating PRODUITS table:" << query.lastError().text();
        } else {
            qDebug() << "Minimal PRODUITS table created.";
        }
    } else {
        // Add stock column to PRODUITS if it doesn't exist
        query.exec("PRAGMA table_info(PRODUITS)");
        bool hasStock = false;
        while (query.next()) {
            if (query.value(1).toString() == "stock") {
                hasStock = true;
                break;
            }
        }
        
        if (!hasStock) {
            if (!query.exec("ALTER TABLE PRODUITS ADD COLUMN stock INTEGER DEFAULT 0")) {
                qDebug() << "Could not add stock column to PRODUITS (might already exist)";
            }
        }
    }
}

void StockMovementPage::setupUI()
{
    ThemeManager& theme = ThemeManager::instance();
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Header
    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(6);
    
    QLabel *title = new QLabel("Gestion des Mouvements de Stock", this);
    title->setObjectName("titleH1");
    QFont titleFont = title->font();
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    title->setFont(titleFont);
    
    QLabel *subtitle = new QLabel("Gérez les entrées, sorties et ajustements de stock", this);
    subtitle->setObjectName("subtitle");
    subtitle->setStyleSheet(QString("color: %1;").arg(theme.textSecondaryColor().name()));
    QFont subtitleFont = subtitle->font();
    subtitleFont.setPointSize(12);
    subtitle->setFont(subtitleFont);
    
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addLayout(headerLayout);

    // Statistics Cards
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);

    // Card 1: Total Produits
    QFrame *card1 = new QFrame(this);
    card1->setObjectName("statCard");
    card1->setStyleSheet(QString(
        "QFrame#statCard {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #10b981, stop:1 #059669);"
        "   border-radius: 14px;"
        "   padding: 24px;"
        "}"
    ));
    QVBoxLayout *card1Layout = new QVBoxLayout(card1);
    QLabel *label1 = new QLabel("Nombre de Produits", this);
    label1->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 13px;");
    totalProductsLabel = new QLabel("0", this);
    totalProductsLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    card1Layout->addWidget(label1);
    card1Layout->addWidget(totalProductsLabel);
    statsLayout->addWidget(card1);

    // Card 2: Mouvements du jour
    QFrame *card2 = new QFrame(this);
    card2->setObjectName("statCard");
    card2->setStyleSheet(QString(
        "QFrame#statCard {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #3b82f6, stop:1 #1d4ed8);"
        "   border-radius: 14px;"
        "   padding: 24px;"
        "}"
    ));
    QVBoxLayout *card2Layout = new QVBoxLayout(card2);
    QLabel *label2 = new QLabel("Mouvements Aujourd'hui", this);
    label2->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 13px;");
    movementsCountLabel = new QLabel("0", this);
    movementsCountLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    card2Layout->addWidget(label2);
    card2Layout->addWidget(movementsCountLabel);
    statsLayout->addWidget(card2);

    // Card 3: Produits en rupture
    QFrame *card3 = new QFrame(this);
    card3->setObjectName("statCard");
    card3->setStyleSheet(QString(
        "QFrame#statCard {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ef4444, stop:1 #dc2626);"
        "   border-radius: 14px;"
        "   padding: 24px;"
        "}"
    ));
    QVBoxLayout *card3Layout = new QVBoxLayout(card3);
    QLabel *label3 = new QLabel("Produits en Rupture", this);
    label3->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 13px;");
    lowStockLabel = new QLabel("0", this);
    lowStockLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    card3Layout->addWidget(label3);
    card3Layout->addWidget(lowStockLabel);
    statsLayout->addWidget(card3);

    mainLayout->addLayout(statsLayout);
    QHBoxLayout *filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(16);
    
    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("🔍 Rechercher par produit...");
    searchInput->setMinimumHeight(44);
    connect(searchInput, &QLineEdit::textChanged, this, &StockMovementPage::onSearchTextChanged);
    
    filterType = new QComboBox(this);
    filterType->addItems({"Tous", "ENTREE", "SORTIE", "AJUSTEMENT"});
    filterType->setMinimumHeight(44);
    connect(filterType, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &StockMovementPage::onFilterTypeChanged);
    
    startDateEdit = new QDateEdit(this);
    startDateEdit->setDate(QDate::currentDate().addMonths(-1));
    startDateEdit->setMinimumHeight(48);
    startDateEdit->setMinimumWidth(140);
    startDateEdit->setCalendarPopup(true);
    connect(startDateEdit, &QDateEdit::dateChanged, this, &StockMovementPage::onFilterDateChanged);
    
    endDateEdit = new QDateEdit(this);
    endDateEdit->setDate(QDate::currentDate());
    endDateEdit->setMinimumHeight(48);
    endDateEdit->setMinimumWidth(140);
    endDateEdit->setCalendarPopup(true);
    connect(endDateEdit, &QDateEdit::dateChanged, this, &StockMovementPage::onFilterDateChanged);
    
    btnRefresh = new QPushButton("🔄 Actualiser", this);
    btnRefresh->setMinimumHeight(48);
    btnRefresh->setMinimumWidth(150);
    connect(btnRefresh, &QPushButton::clicked, this, &StockMovementPage::refreshMovements);
    
    btnExportPDF = new QPushButton("📄 Exporter PDF", this);
    btnExportPDF->setMinimumHeight(48);
    btnExportPDF->setMinimumWidth(150);
    connect(btnExportPDF, &QPushButton::clicked, this, &StockMovementPage::onExportPDF);
    
    filterLayout->addWidget(searchInput, 1);
    filterLayout->addWidget(filterType);
    filterLayout->addWidget(startDateEdit);
    filterLayout->addWidget(endDateEdit);
    filterLayout->addWidget(btnRefresh);
    filterLayout->addWidget(btnExportPDF);
    
    mainLayout->addLayout(filterLayout);

    // Table
    movementsTable = new QTableWidget(this);
    movementsTable->setColumnCount(6);
    movementsTable->setHorizontalHeaderLabels({"ID", "Produit", "Type", "Quantité", "Motif", "Date"});
    movementsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    movementsTable->setMinimumHeight(400);
    movementsTable->setWordWrap(true);
    movementsTable->setAlternatingRowColors(true);
    QHeaderView *h = movementsTable->horizontalHeader();
    h->setHighlightSections(false);
    h->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    // Prefer a mixed resize policy: product and motif stretch, others fixed/interactive
    h->setSectionResizeMode(0, QHeaderView::Fixed);     // ID
    h->setSectionResizeMode(1, QHeaderView::Stretch);   // Produit
    h->setSectionResizeMode(2, QHeaderView::Fixed);     // Type
    h->setSectionResizeMode(3, QHeaderView::Fixed);     // Quantité
    h->setSectionResizeMode(4, QHeaderView::Stretch);   // Motif
    h->setSectionResizeMode(5, QHeaderView::Fixed);     // Date
    // sensible starting widths
    movementsTable->setColumnWidth(0, 60);
    movementsTable->setColumnWidth(2, 200);
    movementsTable->setColumnWidth(3, 90);
    movementsTable->setColumnWidth(5, 200);
    
    mainLayout->addWidget(movementsTable, 1);

    setLayout(mainLayout);
}

void StockMovementPage::applyStyles()
{
    ThemeManager& theme = ThemeManager::instance();
    QString bgColor = (theme.currentTheme() == ThemeManager::LightMode) 
        ? theme.surfaceColor().name()
        : theme.surfaceAltColor().name();
    QString borderColor = theme.borderColor().name();
    
    QString tableStyle = QString(
        "QTableWidget { "
        "background-color: %1; "
        "border: 1px solid %2; "
        "border-radius: 8px; "
        "gridline-color: %2; "
        "} "
        "QHeaderView::section { "
        "background-color: %1; "
        "padding: 6px; "
        "border: none; "
        "border-bottom: 1px solid %2; "
        "}"
    ).arg(bgColor, borderColor);
    
    movementsTable->setStyleSheet(tableStyle);
    
    QString inputStyle = QString(
        "QLineEdit, QComboBox, QDateEdit { "
        "background-color: %1; "
        "border: 1px solid %2; "
        "border-radius: 6px; "
        "padding: 8px 12px; "
        "color: %3; "
        "font-size: 14px; "
        "font-weight: 500; "
        "} "
        "QLineEdit:focus, QComboBox:focus, QDateEdit:focus { "
        "border: 2px solid #3b82f6; "
        "background-color: %1; "
        "} "
        "QDateEdit::up-button, QDateEdit::down-button { "
        "width: 28px; "
        "background-color: %2; "
        "border: none; "
        "padding: 2px; "
        "} "
        "QDateEdit::up-button:hover, QDateEdit::down-button:hover { "
        "background-color: #3b82f6; "
        "} "
        "QDateEdit::up-arrow, QDateEdit::down-arrow { "
        "width: 12px; "
        "height: 12px; "
        "}"
    ).arg(bgColor, borderColor, theme.textColor().name());
    
    searchInput->setStyleSheet(inputStyle);
    filterType->setStyleSheet(inputStyle);
    startDateEdit->setStyleSheet(inputStyle);
    endDateEdit->setStyleSheet(inputStyle);
    
    QString buttonStyle = "QPushButton { background-color: #3b82f6; color: white; border-radius: 6px; font-weight: bold; } "
                         "QPushButton:hover { background-color: #2563eb; }";
    btnRefresh->setStyleSheet(buttonStyle);
    btnExportPDF->setStyleSheet(buttonStyle);
}

void StockMovementPage::loadStockMovements()
{
    populateMovementsTable();
}

void StockMovementPage::populateMovementsTable(const QString &filterType, const QDate &startDate, const QDate &endDate)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    
    QString searchText = searchInput->text();
    QString typeFilter = (this->filterType->currentText() == "Tous") ? "" : this->filterType->currentText();
    
    QString queryStr = "SELECT sm.id_movement, p.nom_produit, sm.type, sm.quantite, sm.motif, sm.date_mouvement "
                      "FROM STOCK_MOVEMENTS sm "
                      "JOIN PRODUITS p ON sm.id_product = p.id_produit "
                      "WHERE 1=1";
    
    if (!searchText.isEmpty()) {
        queryStr += " AND p.nom_produit LIKE '%" + searchText + "%'";
    }
    
    if (!typeFilter.isEmpty()) {
        queryStr += " AND sm.type = '" + typeFilter + "'";
    }
    
    if (startDate.isValid() && endDate.isValid()) {
        queryStr += " AND DATE(sm.date_mouvement) BETWEEN '" + startDate.toString(Qt::ISODate) + 
                   "' AND '" + endDate.toString(Qt::ISODate) + "'";
    }
    
    queryStr += " ORDER BY sm.date_mouvement DESC";
    
    if (!query.exec(queryStr)) {
        QString err = query.lastError().text();
        qDebug() << "Error loading movements: " << err << " SQL:" << queryStr;
        QMessageBox::critical(this, "Erreur lors du chargement des mouvements", err);
        return;
    }
    
    movementsTable->setRowCount(0);
    int row = 0;
    
    while (query.next()) {
        movementsTable->insertRow(row);
        
        QTableWidgetItem *idItem = new QTableWidgetItem(query.value(0).toString());
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
        idItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        idItem->setToolTip(query.value(0).toString());
        movementsTable->setItem(row, 0, idItem);

        QString prodText = query.value(1).toString();
        QTableWidgetItem *productItem = new QTableWidgetItem(prodText);
        productItem->setFlags(productItem->flags() & ~Qt::ItemIsEditable);
        productItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        productItem->setToolTip(prodText);
        movementsTable->setItem(row, 1, productItem);

        QString type = query.value(2).toString();
        QString typeEmoji = (type == "ENTREE") ? "📥" : (type == "SORTIE") ? "📤" : "⚙️";
        QTableWidgetItem *typeItem = new QTableWidgetItem(typeEmoji + " " + type);
        typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);
        typeItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        typeItem->setToolTip(type);
        movementsTable->setItem(row, 2, typeItem);

        QTableWidgetItem *quantityItem = new QTableWidgetItem(query.value(3).toString());
        quantityItem->setFlags(quantityItem->flags() & ~Qt::ItemIsEditable);
        quantityItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        quantityItem->setToolTip(query.value(3).toString());
        movementsTable->setItem(row, 3, quantityItem);

        QString motifText = query.value(4).toString();
        QTableWidgetItem *motifItem = new QTableWidgetItem(motifText);
        motifItem->setFlags(motifItem->flags() & ~Qt::ItemIsEditable);
        motifItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        motifItem->setToolTip(motifText);
        movementsTable->setItem(row, 4, motifItem);

        QString dateStr = query.value(5).toDateTime().toString("dd/MM/yyyy HH:mm");
        QTableWidgetItem *dateItem = new QTableWidgetItem(dateStr);
        dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
        dateItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        dateItem->setToolTip(dateStr);
        movementsTable->setItem(row, 5, dateItem);
        
        row++;
    }

    // Make rows fit their content height and ensure columns present information completely
    movementsTable->resizeRowsToContents();
    movementsTable->horizontalHeader()->setStretchLastSection(false);
    // Reapply fixed column widths after resizing rows (don't use resizeColumnsToContents as it compresses fixed columns)
    movementsTable->setColumnWidth(0, 60);
    movementsTable->setColumnWidth(2, 200);
    movementsTable->setColumnWidth(3, 90);
    movementsTable->setColumnWidth(5, 200);
}


// Edit/delete handlers removed: stock movements are immutable and cannot be changed from the UI

void StockMovementPage::onSearchTextChanged(const QString &text)
{
    populateMovementsTable();
}

void StockMovementPage::onFilterTypeChanged(int index)
{
    populateMovementsTable();
}

void StockMovementPage::onFilterDateChanged()
{
    populateMovementsTable();
}

void StockMovementPage::refreshMovements()
{
    loadStockMovements();
}

void StockMovementPage::updateProductStock(int productId, int quantity, const QString &type)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    
    if (type == "ENTREE") {
        query.prepare("UPDATE PRODUITS SET stock = stock + ? WHERE id_produit = ?");
    } else if (type == "SORTIE") {
        query.prepare("UPDATE PRODUITS SET stock = stock - ? WHERE id_produit = ?");
    }
    // AJUSTEMENT ne change pas le stock directement, juste enregistre
    
    if (type != "AJUSTEMENT") {
        query.addBindValue(quantity);
        query.addBindValue(productId);
        
        if (!query.exec()) {
            qDebug() << "Error updating product stock:" << query.lastError().text();
        }
    }
}

void StockMovementPage::onExportPDF()
{
    // Récupérer les dates et filtres actuels
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();
    QString typeFilter = (filterType->currentText() == "Tous") ? "" : filterType->currentText();
    QString searchText = searchInput->text();

    // Dialog de sauvegarde
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getSaveFileName(this,
        "Exporter les mouvements en PDF",
        QString("%1/Mouvements_Stock_%2_a_%3.pdf")
            .arg(documentsPath)
            .arg(startDate.toString("dd_MM_yyyy"))
            .arg(endDate.toString("dd_MM_yyyy")),
        "PDF (*.pdf)");

    if (fileName.isEmpty()) return;

    // Récupérer les données filtrées
    QSqlQuery query;
    QString queryStr = "SELECT sm.id_movement, p.nom_produit, sm.type, sm.quantite, sm.motif, sm.date_mouvement "
                      "FROM STOCK_MOVEMENTS sm "
                      "JOIN PRODUITS p ON sm.id_product = p.id_produit "
                      "WHERE 1=1";

    if (!searchText.isEmpty()) {
        queryStr += " AND p.nom_produit LIKE '%" + searchText + "%'";
    }
    if (!typeFilter.isEmpty()) {
        queryStr += " AND sm.type = '" + typeFilter + "'";
    }
    if (startDate.isValid() && endDate.isValid()) {
        queryStr += " AND DATE(sm.date_mouvement) BETWEEN '" + startDate.toString(Qt::ISODate) +
                   "' AND '" + endDate.toString(Qt::ISODate) + "'";
    }
    queryStr += " ORDER BY sm.date_mouvement DESC";

    if (!query.exec(queryStr)) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la récupération des données: " + query.lastError().text());
        return;
    }

    // Créer le PDF
    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize::A4);
    pdfWriter.setPageMargins(QMarginsF(10, 10, 10, 10));

    QPainter painter(&pdfWriter);
    if (!painter.isActive()) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le PDF");
        return;
    }

    const qreal leftMargin = 10.0;
    const qreal rightMargin = 10.0;
    const qreal topMargin = 10.0;
    const qreal bottomMargin = 10.0;

    int pageWidth = pdfWriter.width();
    int pageHeight = pdfWriter.height();
    qreal contentX = leftMargin;
    qreal contentY = topMargin;
    qreal contentWidth = pageWidth - leftMargin - rightMargin;
    qreal contentHeight = pageHeight - topMargin - bottomMargin;

    // Fonts
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    QFont normalFont;
    normalFont.setPointSize(10);
    QFont headerFont = normalFont;
    headerFont.setBold(true);

    (void)normalFont; // metrics will be taken from painter to match PDF DPI
    (void)headerFont;
    (void)titleFont;

    // Positions et colonnes (pourcentage du contenu)
    qreal wID = contentWidth * 0.06;      // ID
    qreal wProduct = contentWidth * 0.28; // Produit
    qreal wType = contentWidth * 0.14;    // Type
    qreal wQty = contentWidth * 0.08;     // Quantité
    qreal wMotif = contentWidth * 0.30;   // Motif
    qreal wDate = contentWidth * 0.14;    // Date (augmentée pour visibilité)

    // Precompute column X positions to avoid cumulative float drift
    qreal colX[6];
    colX[0] = contentX;
    colX[1] = colX[0] + wID;
    colX[2] = colX[1] + wProduct;
    colX[3] = colX[2] + wType;
    colX[4] = colX[3] + wQty;
    colX[5] = colX[4] + wMotif;

    auto drawHeader = [&](qreal &y) {
        painter.setFont(titleFont);
        QFontMetricsF fmTitlePainter = painter.fontMetrics();
        painter.drawText(QRectF(contentX, y, contentWidth, fmTitlePainter.height()), Qt::AlignLeft, "Rapport des Mouvements de Stock");
        y += fmTitlePainter.height() + 6;

        painter.setFont(normalFont);
        QFontMetricsF fmNormalPainter = painter.fontMetrics();
        QString infoText = QString("Période: %1 à %2")
            .arg(startDate.toString("dd/MM/yyyy"))
            .arg(endDate.toString("dd/MM/yyyy"));
        if (!typeFilter.isEmpty()) {
            infoText += QString(" | Type: %1").arg(typeFilter);
        }
        painter.drawText(QRectF(contentX, y, contentWidth, fmNormalPainter.height()), Qt::AlignLeft, infoText);
        y += fmNormalPainter.height() + 8;

        // Table header
        painter.setFont(headerFont);
        QFontMetricsF fmHeaderPainter = painter.fontMetrics();
        painter.drawText(QRectF(colX[0], y, wID, fmHeaderPainter.height()), Qt::AlignLeft | Qt::AlignVCenter, "ID");
        painter.drawText(QRectF(colX[1], y, wProduct, fmHeaderPainter.height()), Qt::AlignLeft | Qt::AlignVCenter, "Produit");
        painter.drawText(QRectF(colX[2], y, wType, fmHeaderPainter.height()), Qt::AlignLeft | Qt::AlignVCenter, "Type");
        painter.drawText(QRectF(colX[3], y, wQty, fmHeaderPainter.height()), Qt::AlignLeft | Qt::AlignVCenter, "Qté");
        painter.drawText(QRectF(colX[4], y, wMotif, fmHeaderPainter.height()), Qt::AlignLeft | Qt::AlignVCenter, "Motif");
        painter.drawText(QRectF(colX[5], y, wDate, fmHeaderPainter.height()), Qt::AlignLeft | Qt::AlignVCenter, "Date");
        y += fmHeaderPainter.height() + 4;
        painter.drawLine(QPointF(contentX, y), QPointF(contentX + contentWidth, y));
        y += 6;
        painter.setFont(normalFont);
    };

    qreal y = contentY;
    drawHeader(y);

    const qreal maxRowAvailable = contentY + contentHeight;
    int rowIndex = 0;

    while (query.next()) {
        QString id = query.value(0).toString();
        QString produit = query.value(1).toString();
        QString type = query.value(2).toString();
        QString quantite = query.value(3).toString();
        QString motif = query.value(4).toString();
        QString date = query.value(5).toDateTime().toString("dd/MM/yyyy HH:mm");

        // Calculer les hauteurs nécessaires pour chaque cellule avec word wrap
        painter.setFont(normalFont);
        QFontMetricsF fm = painter.fontMetrics();
        qreal hID = fm.boundingRect(QRectF(0, 0, wID, contentHeight), Qt::TextWordWrap, id).height();
        qreal hProd = fm.boundingRect(QRectF(0, 0, wProduct, contentHeight), Qt::TextWordWrap, produit).height();
        qreal hType = fm.boundingRect(QRectF(0, 0, wType, contentHeight), Qt::TextWordWrap, type).height();
        qreal hQty = fm.boundingRect(QRectF(0, 0, wQty, contentHeight), Qt::TextWordWrap, quantite).height();
        qreal hMotif = fm.boundingRect(QRectF(0, 0, wMotif, contentHeight), Qt::TextWordWrap, motif).height();
        qreal hDate = fm.boundingRect(QRectF(0, 0, wDate, contentHeight), Qt::TextWordWrap, date).height();

        qreal rowHf = qMax(qMax(qMax(hID, hProd), qMax(hType, hQty)), qMax(hMotif, hDate));
        int rowH = int(rowHf) + 8; // padding

        // Nouvelle page si nécessaire
        if (y + rowH > maxRowAvailable) {
            painter.end();
            pdfWriter.newPage();
            if (!painter.begin(&pdfWriter)) {
                QMessageBox::critical(this, "Erreur", "Impossible de poursuivre le dessin sur la nouvelle page PDF");
                return;
            }
            y = contentY;
            drawHeader(y);
        }

        // Alternating row background
        if ((rowIndex % 2) == 0) {
            painter.save();
            painter.setPen(Qt::NoPen);
            painter.fillRect(QRectF(contentX, y, contentWidth, rowH + 6), QColor(245, 245, 245));
            painter.restore();
        }

        // Dessiner les cellules
        painter.setFont(normalFont);
        painter.drawText(QRectF(colX[0], y, wID, rowH), Qt::AlignLeft | Qt::TextWordWrap, id);
        painter.drawText(QRectF(colX[1], y, wProduct, rowH), Qt::AlignLeft | Qt::TextWordWrap, produit);
        painter.drawText(QRectF(colX[2], y, wType, rowH), Qt::AlignLeft | Qt::TextWordWrap, type);
        painter.drawText(QRectF(colX[3], y, wQty, rowH), Qt::AlignRight | Qt::AlignVCenter, quantite);
        painter.drawText(QRectF(colX[4], y, wMotif, rowH), Qt::AlignLeft | Qt::TextWordWrap, motif);
        painter.drawText(QRectF(colX[5], y, wDate, rowH), Qt::AlignRight | Qt::AlignVCenter, date);

        y += rowH + 6;
        
        // subtle horizontal separator
        QPen thinPen(QColor(200,200,200));
        thinPen.setWidthF(0.5);
        QPen oldPen = painter.pen();
        painter.setPen(thinPen);
        painter.drawLine(QPointF(contentX, y-3), QPointF(contentX + contentWidth, y-3));
        painter.setPen(oldPen);

        rowIndex++;
    }

    painter.end();

    QMessageBox::information(this, "Succès", QString("PDF exporté avec succès:\n%1").arg(fileName));
}

void StockMovementPage::loadStatistics()
{
    QSqlQuery query;
    
    // Total products
    query.prepare("SELECT COUNT(*) FROM PRODUITS");
    int totalProducts = 0;
    if (query.exec()) {
        if (query.next()) {
            totalProducts = query.value(0).toInt();
        }
    }
    totalProductsLabel->setText(QString::number(totalProducts));
    
    // Movements today
    query.prepare("SELECT COUNT(*) FROM STOCK_MOVEMENTS WHERE DATE(date_mouvement) = DATE(?)");
    query.addBindValue(QDateTime::currentDateTime());
    int movementsToday = 0;
    if (query.exec()) {
        if (query.next()) {
            movementsToday = query.value(0).toInt();
        }
    }
    movementsCountLabel->setText(QString::number(movementsToday));
    
    // Low stock products (stock <= 5)
    query.prepare("SELECT COUNT(*) FROM PRODUITS WHERE stock <= 5");
    int lowStockCount = 0;
    if (query.exec()) {
        if (query.next()) {
            lowStockCount = query.value(0).toInt();
        }
    }
    lowStockLabel->setText(QString::number(lowStockCount));
}
