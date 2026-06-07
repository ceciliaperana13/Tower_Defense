#include "State/GameState.hpp"
#include "State/MenuState.hpp"
#include "State/PauseState.hpp"
#include "State/GameOverState.hpp"
#include "State/VictoryState.hpp"
#include "Controller/SoundManager.hpp"
#include "Model/Enemy.hpp"
#include "Game.hpp"

GameState::GameState(sf::RenderWindow& window, bool demoMode)
    : m_window(window)
    , m_demoMode(demoMode)
    , m_waveManager(
        m_demoMode ? "../assets/data/waves_test_victory.json"
                       : "../assets/data/waves.json",
        "../assets/data/enemy_values.json",
        [&]() {
            Map tmp;
            return tmp.getWaypoints(MAP_SCALE);
        }()
      )
    , m_timer(400.f)
    , m_castle(
        "../assets/sprites/big_buildings/castle.png",
        [&]() {
            Map tmp;
            auto wp = tmp.getWaypoints(MAP_SCALE);
            return wp.empty() ? sf::Vector2f(900.f, 550.f) : wp.back();
        }(),
        20
      )
    , m_towerController()
    , m_gameView(window, m_map, m_waveManager, m_timer, m_towerController)
{
}

void GameState::onEnter() {
    Enemy::loadHeartTexture("../assets/sprites/icons/heart.png");
    m_towerController.loadFromJson("../assets/data/tower_values.json");
    m_towerController.setMap(&m_map, MAP_SCALE);
    m_gameView.setLives(m_castle.getLives());
    m_waveManager.startNextWave();
    SoundManager::getInstance().playMusic("game");
}

void GameState::onExit() {
    SoundManager::getInstance().stopMusic();
}

// ─── Events 
void GameState::handleEvents(const sf::Event& event) {
    if (m_ended) return;

    auto view = GameView::makeLetterboxView(
        m_window.getSize().x, m_window.getSize().y);

    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) {
            // Escape ferme la popup si ouverte, sinon ouvre la pause
            if (m_gameView.hasConfirmPending()) {
                m_towerController.clearSelection();
                m_gameView.cancelConfirm();
            } else {
                Game::getInstance().getStateManager()
                    .push(std::make_unique<PauseState>(m_window));
            }
            return;
        }
        if (kp->code == sf::Keyboard::Key::Space &&
            m_waveManager.isWaveComplete())
        {
            m_waveManager.startNextWave();
        }
    }

    if (const auto* mm = event.getIf<sf::Event::MouseMoved>()) {
        auto wp = m_window.mapPixelToCoords(mm->position, view);
        m_towerController.setGhostPosition(wp);
        m_gameView.updateHoverAt(wp);
    }

    if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
        auto wp = m_window.mapPixelToCoords(mb->position, view);
        if (mb->button == sf::Mouse::Button::Left)
            handleMouseClick(wp);
        if (mb->button == sf::Mouse::Button::Right) {
            m_towerController.clearSelection();
            m_gameView.cancelConfirm();
        }
    }
}

// ─── handleMouseClick
void GameState::handleMouseClick(sf::Vector2f wp) {
    // Popup open: delegate all clicks to it
    if (m_gameView.hasConfirmPending()) {
        m_gameView.handleConfirmClick(wp);
        return;
    }

    // UI buttons (back, sell)
    MenuAction act = m_gameView.handleClickAt(wp);
    if (act == MenuAction::Exit) {
        flushScore();
        goBackToMenu();
        return;
    }
    if (act == MenuAction::SellTower) {
        m_towerController.sellSelectedTower();
        return;
    }

    // Tower button clicked (basic or upgrade)
    std::string type = m_gameView.getTowerTypeAt(wp);
    if (!type.empty()) {
        if (type == "basic") {
            // Start placing a new basic tower (ghost follows cursor)
            m_towerController.clearSelection();
            m_towerController.selectTower(type);
        } else if (m_towerController.hasUpgradeTarget()) {
            // Upgrade the selected tower — confirmation popup
            m_gameView.showConfirm(type, true, wp);
        }
        return;
    }

    // Click on the map
    if (wp.y < MAP_H) {
        if (m_towerController.hasSelection()) {
            // Ghost active: place or popup
            std::string sel = m_towerController.getSelectedType();
            if (sel == "basic") {
                m_towerController.placeTower(wp);
            } else {
                sf::Vector2f snapped = m_towerController.getSnappedPosition(wp);
                m_towerController.clearSelection();
                m_gameView.showConfirm(sel, false, snapped);
            }
        } else {
            // No ghost: try to select an existing tower
            int idx = m_towerController.getTowerIndexAt(wp);
            if (idx >= 0) {
                m_towerController.selectTowerForUpgrade(idx);
            } else {
                m_towerController.clearSelection();
                m_gameView.cancelConfirm();
            }
        }
    }
}

// ─── Update 
void GameState::update(float dt) {
    if (m_ended) return;

    for (auto& e : m_waveManager.getActiveEnemies())
        if (e->isDead() && !e->hasReached())
            m_towerController.addCoins(e->getReward());

    m_waveManager.update(dt);

    int totalReached = m_waveManager.getTotalReached();
    int delta        = totalReached - m_prevReached;
    if (delta > 0) {
        m_castle.loseLife(delta);
        m_gameView.setLives(m_castle.getLives());
        m_prevReached = totalReached;
    }

    m_towerController.update(dt, m_waveManager.getActiveEnemies());
    m_timer.update(dt);
    m_gameView.update(dt);

    checkEndConditions();
}
// ─── checkEndConditions 
void GameState::checkEndConditions() {
    auto& sm = Game::getInstance().getStateManager();

    if (m_castle.isDead()) {
        flushScore();
        m_ended = true;
        sm.change(std::make_unique<GameOverState>(
            m_window,
            m_towerController.getCoins(),
            m_waveManager.getTotalKills(),
            m_waveManager.getCurrentWaveId()
        ));
        return;
    }

    if (m_timer.remaining() <= 0.f) {
        flushScore();
        m_ended = true;
        sm.change(std::make_unique<GameOverState>(
            m_window,
            m_towerController.getCoins(),
            m_waveManager.getTotalKills(),
            m_waveManager.getCurrentWaveId()
        ));
        return;
    }

    if (m_waveManager.isWaveComplete() &&
        m_waveManager.getCurrentWaveId() == m_waveManager.getTotalWaves() &&
        m_waveManager.getActiveEnemies().empty() &&
        !m_castle.isDead())
    {
        flushScore();
        m_ended = true;
        sm.change(std::make_unique<VictoryState>(
            m_window,
            m_towerController.getCoins(),
            m_waveManager.getTotalKills(),
            m_waveManager.getCurrentWaveId()
        ));
    }
}

// ─── Render 
void GameState::render(sf::RenderWindow& window) {
    auto view = GameView::makeLetterboxView(
        window.getSize().x, window.getSize().y);
    window.setView(view);

    m_gameView.render();
    m_castle.render(window);

    m_towerController.render(window);
}

// ─── Helpers 
void GameState::flushScore() {
    ScoreData sd;
    sd.playerName = "Player";
    sd.score      = m_towerController.getCoins();
    sd.enemyCount = m_waveManager.getTotalKills();
    sd.wave       = m_waveManager.getCurrentWaveId();
    Game::getInstance().getSaveController().saveScore(sd);
}

void GameState::goBackToMenu() {
    m_ended = true;
    Game::getInstance().getStateManager()
        .change(std::make_unique<MenuState>(m_window));
}