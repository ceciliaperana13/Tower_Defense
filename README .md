# MainMenu & SettingsMenu

Documentation technique des classes de menu pour **Defend the Castle**.

---

## MainMenu

### Description
Écran d'accueil du jeu. Affiche le titre, les boutons de navigation et l'icône settings. Joue une musique de fond en boucle.

### Fichiers
- `include/View/MainMenu.hpp`
- `source/View/MainMenu.cpp`

### Dépendances
- SFML 3 — Graphics, Audio
- `assets/sprites/background.jpg`
- `assets/sprites/setting.png`
- `assets/sprites/buttons/new_game.png`
- `assets/sprites/buttons/leaderboard.png`
- `assets/sprites/buttons/options.png`
- `assets/sprites/buttons/quit_game.png`
- `assets/sound/music/medieval_music.ogg`
- Police système `C:/Windows/Fonts/arialbd.ttf`

### Utilisation

```cpp
sf::RenderWindow window(sf::VideoMode({ 1080u, 720u }), "Defend the Castle");
GameSettings settings;

MainMenu menu(window);
MenuAction action = menu.run(); // bloquant — retourne quand l'utilisateur clique
```

### Valeurs de retour (`MenuAction`)

| Valeur | Déclencheur |
|---|---|
| `MenuAction::NewGame` | Bouton "New Game" |
| `MenuAction::Scoreboard` | Bouton "Leaderboard" |
| `MenuAction::Settings` | Bouton "Options" ou icône engrenage |
| `MenuAction::Exit` | Bouton "Quit Game" ou touche Échap |
| `MenuAction::None` | Aucune action (état initial) |

### Structure interne

```
MainMenu
├── loadAssets()       — charge police, background, titre, boutons, musique
├── buildButtons()     — crée les 4 boutons avec textures PNG
├── handleEvents()     — gère clics souris et touches clavier
├── update()           — effet hover sur les boutons
└── render()           — dessine background → titre → boutons → icône
```

### Comportement hover
Quand la souris survole un bouton, `setFillColor(180, 180, 180)` assombrit légèrement la texture. Au repos : `sf::Color::White` (texture normale).

### Note importante — pointeurs de texture
Après `push_back` dans le vector, les pointeurs internes de `sf::RectangleShape` vers les textures sont réappliqués manuellement :
```cpp
for (auto& btn : m_buttons)
    btn.shape.setTexture(&btn.texture);
```
Sans cela les boutons apparaissent blancs (dangling pointer).

---

## SettingsMenu

### Description
Panneau de paramètres superposé au menu principal (overlay). Permet de régler le volume musique, le volume effets sonores, le mode plein écran et la langue.

### Fichiers
- `include/View/SettingsMenu.hpp`
- `source/View/SettingsMenu.cpp`

### Dépendances
- SFML 3 — Graphics
- Police système `C:/Windows/Fonts/arialbd.ttf`
- Aucun asset image requis (UI entièrement dessinée en code)

### Utilisation

```cpp
// Appelé depuis main.cpp quand action == MenuAction::Settings
SettingsMenu sm(window, settings);
sm.run(); // bloquant — retourne quand l'utilisateur ferme le panneau
```

### Structure de données — `GameSettings`

```cpp
struct GameSettings {
    float musicVolume  { 50.f };   // 0 à 100
    float sfxVolume    { 50.f };   // 0 à 100
    bool  fullscreen   { false };
    int   languageIdx  { 0 };      // index dans LANGUAGES

    static const std::vector<std::string> LANGUAGES; // {"Francais", "English", "Espanol"}
};
```

### Contrôles UI

| Élément | Interaction |
|---|---|
| Slider Musique | Clic + glisser |
| Slider Effets sonores | Clic + glisser |
| Checkbox Plein écran | Clic — recrée la fenêtre immédiatement |
| Boutons `<` `>` Langue | Clic pour cycler |
| Bouton ✕ rouge | Ferme le panneau |
| Touche Échap | Ferme le panneau |

### Plein écran
Le toggle recrée la fenêtre SFML avec `m_window.create()` puis rappelle `buildUI()` pour recalculer les positions du panneau :

```cpp
m_window.create(mode, "Defend the Castle",
    m_settings.fullscreen ? sf::State::Fullscreen : sf::State::Windowed);
buildUI(); // recalcul des positions après redimensionnement
```

### Structure interne

```
SettingsMenu
├── buildUI()              — calcule toutes les zones de hit (FloatRect)
├── handleEvents()         — clics, drag sliders, Échap
├── update()               — met à jour les valeurs pendant le drag
├── render()               — overlay + panneau + tous les widgets
│   ├── drawSliderRow()    — helper : track + fill + knob + valeur
│   └── drawLabel()        — helper : texte label à gauche
└── Actions
    ├── onMusicSlider()
    ├── onSfxSlider()
    ├── onFullscreenToggle()
    ├── onLanguageNext/Prev()
```

### Style visuel
- Fond du panneau : `rgb(15, 15, 25)` opacité 240
- Bordures et accents : `rgb(180, 140, 60)` (doré)
- Sliders remplis : `rgb(220, 175, 60)`
- Overlay sombre : noir à 150/255

---

## Compilation

```powershell
g++ source/main.cpp source/View/MainMenu.cpp source/View/SettingsMenu.cpp \
    -o main.exe \
    -Iinclude -Iinclude/View \
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

## Structure des fichiers

```
Tower_Defense/
├── assets/
│   ├── sound/music/medieval_music.ogg
│   └── sprites/
│       ├── background.jpg
│       ├── setting.png
│       └── buttons/
│           ├── new_game.png
│           ├── leaderboard.png
│           ├── options.png
│           └── quit_game.png
└── script/
    ├── include/View/
    │   ├── MainMenu.hpp
    │   └── SettingsMenu.hpp
    └── source/View/
        ├── MainMenu.cpp
        └── SettingsMenu.cpp
```
