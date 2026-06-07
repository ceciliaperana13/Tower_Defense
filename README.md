# Tower Defense — Defend the Castle

A 2D tower defense game built in C++ with SFML. Defend your castle against waves of goblins, orcs and ogres by placing and upgrading towers with unique abilities.

---

## 1. Installation

### Requirements

- **C++17** or later
- **g++** (MinGW on Windows, g++ on Linux/macOS)
- **SFML 3.x** — Graphics, Window, Audio, System modules

### Installing SFML

**Windows (MinGW):**
Download the MinGW-compatible SFML 3 package from https://www.sfml-dev.org/download.php and extract it. Add the `bin` folder to your PATH.

**Linux:**
```bash
sudo apt install libsfml-dev
```

**macOS:**
```bash
brew install sfml
```

### Compilation

From the `script/` directory, run:

```bash
g++ -std=c++17 -I include \
  source/main.cpp source/Game.cpp \
  source/State/StateManager.cpp source/State/MenuState.cpp \
  source/State/GameState.cpp source/State/OptionsState.cpp \
  source/State/LeaderboardState.cpp source/State/PauseState.cpp \
  source/State/GameOverState.cpp source/State/VictoryState.cpp \
  source/State/SplashState.cpp source/State/IntroState.cpp \
  source/Controller/GameController.cpp source/Controller/SaveController.cpp \
  source/Controller/SoundManager.cpp source/Controller/TowerController.cpp \
  source/Model/Castle.cpp source/Model/CountdownTimer.cpp \
  source/Model/Enemy.cpp source/Model/Map.cpp source/Model/Projectile.cpp \
  source/Model/Tower.cpp source/Model/WaveManager.cpp \
  source/View/Button.cpp source/View/GameView.cpp source/View/Leaderboard.cpp \
  source/View/MainMenu.cpp source/View/SettingsMenu.cpp \
  source/View/TimerView.cpp source/View/WaveView.cpp \
  -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system \
  -o DefendTheCastle
```

Then run the executable from the **project root** (not from `script/`), so that relative asset paths resolve correctly:

```bash
cd ..
./script/DefendTheCastle   # Linux / macOS
script\DefendTheCastle.exe # Windows
```

---

## 2. How to Play

### Main Menu

| Button | Action |
|---|---|
| New Game | Choose a mode and start |
| Leaderboard | View best scores |
| Options | Audio, fullscreen, language |
| Quit Game | Exit |

**New Game** opens a mode picker:
- **Normal Mode** — full 15-wave campaign
- **Victory Demo** — single trivial wave, instant victory screen

### In-Game Controls

| Input | Action |
|---|---|
| Left click (basic button) | Select a basic tower to place |
| Left click (map, tower selected) | Place the tower |
| Left click (existing tower) | Select it for upgrade or sell |
| Right click | Cancel placement / deselect |
| Escape | Pause |
| F11 | Toggle fullscreen |

### Tower Placement

- Towers occupy a **2×2 tile** block.
- Highlighted cells show where placement is valid (green) or blocked (red).
- Blocked zones: the enemy path, the castle, the orc cave, trees, and existing towers.

### Tower Types

| Tower | Cost | Special ability |
|---|---|---|
| Basic | 50g | Standard single-target shots |
| Fire | 125g | Burns enemies — damage over time |
| Ice | 100g | Slows enemies to 35% speed for 3 s |
| Earth | 150g | Large area-of-effect explosion |
| Arcane | 140g | Hits every enemy in range simultaneously |

Upgrade a basic tower by clicking it, then choosing an upgrade type. Sell any tower for 50% of its purchase cost.

### Winning & Losing

Survive all 15 waves without losing all your lives. The castle loses a life for each enemy that reaches it. Running out of lives — or time — triggers defeat. Clearing all waves triggers victory.

---

## 3. Architecture & Design Patterns

The project follows a clean **MVC (Model-View-Controller)** architecture throughout.

### State Pattern — `IState` / `StateManager`

All game screens (menu, intro, game, pause, victory, game over…) are `IState` subclasses managed by a `StateManager` stack. Transitions use `change()` (replace) or `push()`/`pop()` (overlay). This isolates each screen's logic cleanly and makes adding new screens trivial.

### Singleton — `Game`, `SoundManager`

`Game` is a singleton that owns the `sf::RenderWindow`, the `StateManager`, the persistent `GameSettings`, and the `SaveController`. `SoundManager` is a singleton that centralises all audio playback. Both are accessible globally via `getInstance()` without passing references through every layer.

### Observer-like — `WaveManager` → `GameState`

`GameState` polls `WaveManager` every frame for wave completion and countdown state, decoupling the spawning logic from game-over conditions and UI updates.

### Factory (data-driven) — `Enemy::fromJson`, `TowerController::loadFromJson`

Enemies and towers are fully defined in JSON (`enemy_values.json`, `tower_values.json`, `waves.json`). The factory methods parse the JSON and construct the objects, making balancing and content changes possible without recompiling.

---

## 4. Authors

- **Cecilia Perana**
- **Yannis Sandoval**
- **Nelson Grac-Aubert**