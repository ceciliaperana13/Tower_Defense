#include "State/GameState.hpp"
#include "State/MenuState.hpp"
#include "State/PauseState.hpp"
#include "State/GameOverState.hpp"
#include "State/VictoryState.hpp"
#include "Controller/SoundManager.hpp"
#include "Model/Enemy.hpp"
#include "Game.hpp"

GameState::GameState(sf::RenderWindow& window)
    : m_window(window)
    , m_waveManager(
        "../assets/data/waves.json",
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
    m_upgradeRing.setRadius(28.f);
    m_upgradeRing.setOrigin({ 28.f, 28.f });
    m_upgradeRing.setFillColor(sf::Color::Transparent);
    m_upgradeRing.setOutlineColor(sf::Color::Yellow);
    m_upgradeRing.setOutlineThickness(3.f);
}

void GameState::onEnter() {
    Enemy::loadHeartTexture("../assets/sprites/icons/heart.png");
    m_towerController.loadFromJson("../assets/data/tower_values.json");
    m_gameView.setLives(m_castle.getLives());
    m_waveManager.startNextWave();
    SoundManager::getInstance().playMusic("game");
}

void GameState::onExit() {
    SoundManager::getInstance().stopMusic();
}

void GameState::handleEvents(const sf::Event& event) {
    if (m_ended) return;

    auto view = GameView::makeLetterboxView(
        m_window.getSize().x, m_window.getSize().y);

    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) {
            Game::getInstance().getStateManager()
                .push(std::make_unique<PauseState>(m_window));
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
            m_showUpgradeRing = false;
        }
    }
}

void GameState::handleMouseClick(sf::Vector2f wp) {
    std::string type = m_gameView.getTowerTypeAt(wp);

    if (!type.empty()) {
        if (m_towerController.hasUpgradeTarget()) {
            if (type != "basic")
                m_towerController.upgradeTower(type);
            m_showUpgradeRing = false;
        } else {
            m_towerController.selectTower(type);
        }
        return;
    }

    MenuAction act = m_gameView.handleClickAt(wp);

    if (act == MenuAction::Exit) {
        flushScore();
        goBackToMenu();
        return;
    }

    if (act == MenuAction::SellTower) {
        m_towerController.sellSelectedTower();
        m_showUpgradeRing = false;
        return;
    }

    if (act == MenuAction::None && wp.y < MAP_H) {
        if (m_towerController.hasSelection()) {
            m_towerController.placeTower(wp);
            m_showUpgradeRing = false;
        } else {
            int idx = m_towerController.getTowerIndexAt(wp);
            if (idx >= 0) {
                m_towerController.selectTowerForUpgrade(idx);
                m_showUpgradeRing = true;
                m_upgradeRingPos  = wp;
            } else {
                m_towerController.clearSelection();
                m_showUpgradeRing = false;
            }
        }
    }
}

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

void GameState::render(sf::RenderWindow& window) {
    auto view = GameView::makeLetterboxView(
        window.getSize().x, window.getSize().y);
    window.setView(view);

    m_gameView.render();
    m_castle.render(window);

    if (m_showUpgradeRing && m_towerController.hasUpgradeTarget()) {
        m_upgradeRing.setPosition(m_upgradeRingPos);
        window.draw(m_upgradeRing);
    }

    m_towerController.render(window);
}

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