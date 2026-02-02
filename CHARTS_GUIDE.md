# 📊 SYSTÈME DE CHARTS - GUIDE COMPLET

## 🎯 OBJECTIFS ATTEINTS

### ✅ Analyse complète des données métier
- Ventes par période
- Produits populaires
- Statut des commandes
- Performance des vendeurs
- Fidélité clients
- Encaissements

### ✅ Deux dashboards spécialisés
1. **ADMIN Dashboard**: Vue globale de l'entreprise
2. **VENDEUR Dashboard**: Vue personnalisée des performances

### ✅ 9 Charts actifs + 4 métriques KPI
- Bar Charts (2)
- Pie Charts (7)
- Stats Cards (4)

---

## 🔧 ARCHITECTURE TECHNIQUE

### Couche de Données: `ChartsManager`
```
┌─────────────────────────────┐
│    ChartsManager (classe)    │
├─────────────────────────────┤
│ - 10 méthodes de requêtes   │
│ - Structure ChartDataPoint  │
│ - Couleurs prédéfinies      │
│ - Filtrage par rôle         │
└─────────────────────────────┘
           │
           ▼
     Requêtes SQL
           │
           ▼
    ┌──────────────┐
    │ BD SQLite    │
    │ (COMMANDES,  │
    │  PRODUITS,   │
    │  CLIENTS)    │
    └──────────────┘
```

### Couche Présentation: `ChartsWidget`
```
┌──────────────────────────────┐
│   ChartsWidget (QFrame)       │
├──────────────────────────────┤
│ + showSalesMonthlyChart()    │
│ + showTopProductsChart()     │
│ + showPaymentStatusChart()   │
│ + showRevenueByVendorChart() │
│ + showVendorSalesChart()     │
│ + showVendorProductsChart()  │
│ + showVendorClientsChart()   │
│ + showVendorPaymentChart()   │
├──────────────────────────────┤
│ Utilise Qt Charts:           │
│ - QChart / QChartView        │
│ - QBarChart / QBarSeries     │
│ - QPieChart / QPieSeries     │
└──────────────────────────────┘
```

### Couche Logique: `DashboardPage`
```
┌──────────────────────────────┐
│   DashboardPage (QFrame)      │
├──────────────────────────────┤
│ + setUserRole(role, userId)  │
│ + setupAdminDashboard()      │
│ + setupVendorDashboard()     │
├──────────────────────────────┤
│ Layout:                      │
│ ├─ Header                    │
│ ├─ Stats Cards (4)           │
│ └─ TabWidget                 │
│    ├─ Tab 1: Chart           │
│    ├─ Tab 2: Chart           │
│    ├─ Tab 3: Chart           │
│    └─ Tab 4: Chart           │
└──────────────────────────────┘
```

---

## 📈 CHARTS PAR RÔLE

### 🔐 ADMIN Dashboard

#### Métriques KPI
| Card | Métrique | Icône | Données |
|------|----------|-------|---------|
| 1 | Chiffre d'Affaires | 💰 | SUM(total) WHERE statut='PAYEE' |
| 2 | Commandes | 📋 | COUNT(*) FROM COMMANDES |
| 3 | Clients | 🤝 | COUNT(*) FROM CLIENTS |
| 4 | Produits | 📦 | COUNT(*) FROM PRODUITS |

#### Charts Tabs
1. **Ventes Mensuelles** (Bar Chart)
   - 12 derniers mois
   - Tendance des ventes

2. **Top Produits** (Pie Chart)
   - Top 10 produits
   - Quantités vendues

3. **Statut Commandes** (Pie Chart)
   - Distribution EN_COURS / PAYEE / ANNULEE
   - Nombre de commandes

4. **Revenus Vendeurs** (Pie Chart)
   - Contribution de chaque vendeur
   - Montants totaux

---

### 💼 VENDEUR Dashboard

#### Métriques KPI
| Card | Métrique | Icône | Données |
|------|----------|-------|---------|
| 1 | Mes Ventes | 💵 | SUM(total) WHERE id_user=vendorId |
| 2 | Mes Commandes | 📋 | COUNT(*) WHERE id_user=vendorId |
| 3 | Mes Clients | 👥 | COUNT(DISTINCT id_client) |
| 4 | Non Payées | ⚠️ | COUNT(*) WHERE statut='EN_COURS' |

#### Charts Tabs
1. **Mes Ventes** (Bar Chart)
   - 6 derniers mois
   - Progression personnelle

2. **Produits Populaires** (Pie Chart)
   - Top 8 produits personnels
   - Quantités vendues

3. **Fidélité Clients** (Pie Chart)
   - Top 8 clients
   - Nombre de commandes

4. **Statut Ventes** (Pie Chart)
   - Montant par statut
   - Suivi des paiements

---

## 🗄️ REQUÊTES SQL UTILISÉES

### Ventes Mensuelles (ADMIN)
```sql
SELECT SUBSTR(DATE(c.date_commande), 1, 7) as mois,
       SUM(c.total) as total_ventes
FROM COMMANDES c
WHERE c.date_commande >= date('now', '-12 months')
GROUP BY SUBSTR(DATE(c.date_commande), 1, 7)
ORDER BY mois DESC
LIMIT 12
```

### Top Produits
```sql
SELECT p.nom_produit,
       SUM(dc.quantite) as total_quantite
FROM DETAILS_COMMANDE dc
JOIN PRODUITS p ON dc.id_produit = p.id_produit
GROUP BY p.id_produit
ORDER BY total_quantite DESC
LIMIT 10
```

### Revenus par Vendeur
```sql
SELECT u.nom as vendeur,
       SUM(c.total) as total_ventes,
       COUNT(c.id_commande) as nombre_commandes
FROM COMMANDES c
JOIN USERS u ON c.id_user = u.id_user
WHERE u.role = 'VENDEUR'
GROUP BY u.id_user
ORDER BY total_ventes DESC
```

### Ventes Vendeur Spécifique
```sql
SELECT SUBSTR(DATE(c.date_commande), 1, 7) as mois,
       SUM(c.total) as total_ventes
FROM COMMANDES c
WHERE c.id_user = :vendorId
AND c.date_commande >= date('now', '-6 months')
GROUP BY SUBSTR(DATE(c.date_commande), 1, 7)
ORDER BY mois DESC
```

### Fidélité Clients
```sql
SELECT cl.nom,
       COUNT(c.id_commande) as nombre_commandes,
       SUM(c.total) as montant_total
FROM COMMANDES c
JOIN CLIENTS cl ON c.id_client = cl.id_client
WHERE c.id_user = :vendorId
GROUP BY c.id_client
ORDER BY nombre_commandes DESC
LIMIT 8
```

---

## 🎨 PALETTE DE COULEURS

```css
/* Couleurs primaires pour charts */
Bleu:           #3B82F6 (59, 130, 246)   - Confiance, stabilité
Violet:         #8B5CF6 (139, 92, 246)   - Créativité, différenciation
Rose:           #EC4899 (236, 72, 153)   - Accent, attention
Rouge:          #F87171 (248, 113, 113)  - Alerte, critique
Vert:           #22C55E (34, 197, 94)    - Succès, positif
Jaune:          #FACC15 (250, 204, 21)   - En attente, neutre
Cyan:           #06B6D4 (6, 182, 212)    - Frais, moderne
Violet foncé:   #A855F7 (168, 85, 247)   - Premium, exclusif

/* Fond charts */
Fond sombre:    #0F172A (15, 23, 42)
```

---

## 📱 RESPONSIVITÉ

### Layouts
- Header: Titre + Sous-titre (flex)
- Cards: Grille 2x2 (auto-réarrangeable)
- Charts: Onglets (QTabWidget adaptable)
- Scrollable: Contenu sur grand écran

### Breakpoints
- Petit écran: Stacking
- Écran normal: 2x2 grille
- Grand écran: Tous visibles

---

## 🚀 PERFORMANCE

### Optimisations
1. **Requêtes**: Agrégations au niveau BD
2. **Couleurs**: Palette prédéfinie (pas de calcul)
3. **Rendu**: Antialiasing pour lissage
4. **Caching**: Données chargées une fois par onglet

### Temps de charge estimé
- Dashboard ADMIN: ~500ms
- Dashboard VENDEUR: ~400ms
- Changement d'onglet: ~200ms

---

## 🔒 SÉCURITÉ

### Contrôles d'accès
- ✅ Dashboard ADMIN: Voir toutes les données
- ✅ Dashboard VENDEUR: Voir uniquement ses données (filtré par `id_user`)
- ✅ Pas de requête non filtrée

### Validation
- Paramètres liés (`:vendorId`)
- Pas d'injection SQL possible
- Rôles vérifiés à l'entrée

---

## 📋 CHECKLIST D'IMPLÉMENTATION

### Phase 1: Données ✅
- [x] Créer ChartsManager
- [x] Implémenter 10 requêtes SQL
- [x] Tester les retours de données
- [x] Couleurs palettes

### Phase 2: Présentation ✅
- [x] Créer ChartsWidget
- [x] Implémenter 8 méthodes de charts
- [x] Styliser avec thème sombre
- [x] Ajouter légendes

### Phase 3: Logique ✅
- [x] Modifier DashboardPage
- [x] Ajouter setUserRole()
- [x] Créer setupAdminDashboard()
- [x] Créer setupVendorDashboard()
- [x] Implémenter TabWidget

### Phase 4: Intégration ✅
- [x] Mettre à jour .pro file
- [x] Modifier MainWindow
- [x] Tester activation par rôle
- [x] Documenter

---

## 🐛 DÉPANNAGE

### Chart vide
❌ Vérifier la BD a des données
❌ Vérifier la requête SQL
❌ Ajouter qDebug() pour traçage

### Couleurs incorrectes
❌ Vérifier QColor(R, G, B) valides
❌ Rafraîchir le projet (qmake)

### Performance lente
❌ Ajouter LIMIT aux requêtes
❌ Indexer les colonnes de filtrage
❌ Vérifier pas d'N+1 queries

---

## 📞 CONTACT & SUPPORT

Pour questions sur les charts:
1. Vérifier les requêtes SQL dans ChartsManager
2. Examiner ChartsWidget pour affichage
3. Consulter DashboardPage pour intégration
4. Lire CHARTS_ANALYSIS.md pour détails

---

**Dernière mise à jour**: 2025-01-26
**Statut**: ✅ PRODUCTION READY
