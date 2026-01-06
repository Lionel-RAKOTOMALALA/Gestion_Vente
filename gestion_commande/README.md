# Stylesheet centralisé pour l'application

Ce projet contient un fichier `stylesheet.h` + `stylesheet.cpp` qui déclare des variables simples et des fonctions utilitaires permettant de styler l'application Qt avec une syntaxe plus lisible.

Structure
- `stylesheet.h` : variables déclarées (comme `primaryColor`, `accentColor`, `lightBgColor`, `borderRadius`) et déclarations de fonctions (ex. `buttonStyle()`, `tableStyle()`, `appStyle()`).
- `stylesheet.cpp` : implémentation des variables et fonctions renvoyant des `QString` de styles (chaînes `setStyleSheet`).

Comment l'utiliser
- Pour appliquer un style global à l'application (en `main.cpp`) :
  - Inclure `#include "stylesheet.h"` et appeler `a.setStyleSheet(Stylesheet::appStyle());`.
- Pour appliquer un style à un widget spécifique (dans une `QWidget` ou `QMainWindow`) :
  - `ui->myBtn->setStyleSheet(Stylesheet::buttonStyle());`
  - `ui->myTableView->setStyleSheet(Stylesheet::tableStyle());`
  - Les styles sont maintenant construits à partir de templates lisibles (raw string literals) dans `stylesheet.cpp`.

Personnaliser les styles
- Ouvrez `stylesheet.cpp` et modifiez les couleurs et constantes en haut du fichier (ex. `primaryColor`, `accentColor`).
- Les fonctions utilisent ces variables pour construire des styles réutilisables.

Exécution rapide (PowerShell)
```powershell
cd 'C:\Users\Lando ANDRIANANJA\Documents\gestion_commande'
qmake
mingw32-make
```

Test
- Compilez et lancez l'application. Vérifier l'apparence des boutons et des tables.

Remarques et améliorations possibles
- Les couleurs et variables sont des `inline const char*` dans `stylesheet.h` — c'est simple à lire et à modifier.
- Si vous voulez pouvoir modifier les variables à l'exécution (p.ex. switch thème clair/sombre), remplacez `inline const char*` par des variables modifiables (p.ex. `inline QString`) et ajoutez des setters pour re-construire les templates.
- Vous pouvez ajouter des utilitaires supplémentaires (polices, tailles, icônes, palette Qt) et centraliser le thème de l'application.

