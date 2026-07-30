#include "GameEngine.h"

#include "core/EventBus.h"
#include "core/InputManager.h"
#include "core/ResourceManager.h"
#include "core/TransitionEffect.h"
#include "ui/GameView.h"
#include "ui/GameScene.h"
#include "ui/MainWindow.h"
#include "ui/MainMenu.h"
#include "ui/HUD.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "scene/SceneManager.h"
#include "chapters/ChapterBase.h"
#include "dialogue/DialogueManager.h"
#include "combat/CombatSystem.h"
#include "video/CutsceneManager.h"

#include <QApplication>
#include <QDebug>

GameEngine::GameEngine(MainWindow* window, QObject* parent)
    : QObject(parent)
    , m_window(window)
{
    // ── 基础系统 ──
    m_gameScene   = new GameScene(this);
    m_gameView    = new GameView(m_gameScene, m_window);
    m_eventBus    = new EventBus(this);
    m_input       = new InputManager(this);
    m_resources   = new ResourceManager(this);

    // ── 过渡效果 ──
    m_transition  = new TransitionEffect(m_gameScene, this);

    // ── 玩家 ──
    m_player = new Player();
    m_gameScene->addItem(m_player);

    // ── UI 层 ──
    m_titleCard  = new ChapterTitleCard();
    m_gameScene->addItem(m_titleCard);

    m_playerHpBar = new PlayerHPBar();
    m_playerHpBar->setVisible(false);  // 仅在 BOSS 战时显示
    m_gameScene->addItem(m_playerHpBar);

    m_bossHpBar = new BossHPBar();
    m_gameScene->addItem(m_bossHpBar);

    m_dialogueBox = new DialogueBox();
    m_gameScene->addItem(m_dialogueBox);

    // ── 子系统 ──
    m_dialogueMgr = new DialogueManager(m_eventBus, this);
    m_combatSys   = new CombatSystem(m_eventBus, this);
    m_combatSys->setPlayer(m_player);

    // ── 过场视频管理器 ──
    m_cutsceneMgr = new CutsceneManager(m_gameScene, this);

    // ── 章节管理器（传入过渡效果 + 视频管理器） ──
    m_sceneMgr = new SceneManager(m_gameScene, m_player,
                                  m_combatSys, m_dialogueMgr,
                                  m_transition, m_cutsceneMgr, this);
    m_sceneMgr->setTitleCard(m_titleCard);
    // 点击标题卡片 → 关闭
    connect(m_titleCard, &ChapterTitleCard::dismissed,
            m_sceneMgr, &SceneManager::dismissTitleCard);

    // ── 嵌入窗口 ──
    m_window->setGameView(m_gameView);

    // ── 帧定时器 ──
    m_timer = new QTimer(this);
    m_timer->setInterval(GameConfig::FRAME_INTERVAL_MS);
    m_timer->setTimerType(Qt::PreciseTimer);

    wireConnections();

    // ── 加载样式 ──
    m_resources->loadStyleSheet(":/styles/main.qss");
}

GameEngine::~GameEngine() { stop(); }

void GameEngine::wireConnections()
{
    // ── 帧循环 ──
    connect(m_timer, &QTimer::timeout, this, &GameEngine::onTick);

    // ── 键盘输入 ──
    connect(m_gameView, &GameView::gameKeyPressed,
            this, &GameEngine::onKeyPressed);
    connect(m_gameView, &GameView::gameKeyReleased,
            this, &GameEngine::onKeyReleased);
    connect(m_gameView, &GameView::gameMousePressed,
            this, [this](Qt::MouseButton btn, const QPointF& pos) {
        m_input->handleMousePress(btn, pos);
    });

    // ── 对话 → UI ──
    connect(m_dialogueMgr, &DialogueManager::speakerChanged, this,
            [this](const QString& speaker) { m_dialogueBox->setSpeaker(speaker); });
    connect(m_dialogueMgr, &DialogueManager::textChanged, this,
            [this](const QString& text) { m_dialogueBox->setText(text); });
    connect(m_dialogueMgr, &DialogueManager::segmentFinished, this, [this]() {
        m_dialogueBox->showNextIndicator(true);
    });
    // Hide speaker when dialogue ends
    connect(m_dialogueMgr, &DialogueManager::segmentFinished, this, [this]() {
        m_dialogueBox->setSpeaker(QString());
    });

    // ── 章节 → HUD ──
    connect(m_sceneMgr, &SceneManager::hudUpdate,
            this, &GameEngine::onHudUpdate);

    // ── 暂停/恢复 ──
    connect(m_eventBus, &EventBus::pauseRequested, this, &GameEngine::onPause);
    connect(m_eventBus, &EventBus::resumeRequested, this, &GameEngine::onResume);

    // ── 过场视频 → 状态切换 ──
    connect(m_cutsceneMgr, &CutsceneManager::cutsceneStarted, this, [this](const QString&) {
        m_state = GameConfig::GameState::CUTSCENE;
    });
    connect(m_cutsceneMgr, &CutsceneManager::cutsceneFinished, this, [this](const QString&) {
        m_state = GameConfig::GameState::PLAYING;
    });

    // ── 菜单 → 开始游戏 ──
    connect(m_window->menuWidget(), &MainMenu::startGameRequested,
            this, &GameEngine::onMenuStartGame);

    // ── 玩家 HP → 左上角血条 ──
    connect(m_player, &Player::hpChanged,
            m_playerHpBar, &PlayerHPBar::onHpChanged);

    // ── 玩家死亡 → 失败画面 ──
    connect(m_player, &Player::playerDied, this, [this]() {
        m_state = GameConfig::GameState::PAUSED;
        m_timer->stop();
        m_player->setVisible(false);   // 隐藏角色，不在失败画面中出现
        m_window->showFailScreen();
    });

    // ── Boss 战斗 → 底部血条 ──
    connect(m_combatSys, &CombatSystem::bossSpawned,
            this, &GameEngine::onBossSpawned);
    connect(m_combatSys, &CombatSystem::bossDefeated,
            this, [this](const QString&) { m_bossHpBar->hideBar(); });
    connect(m_combatSys, &CombatSystem::combatEnded,
            this, &GameEngine::onCombatEnded);
}

// ═══════════════════════════════════════════════════════════
//  帧循环
// ═══════════════════════════════════════════════════════════

void GameEngine::start()
{
    if (m_running) return;
    m_running = true;
    // 不自动启动章节 — 等待菜单触发
    m_timer->start();
}

void GameEngine::stop()
{
    if (!m_running) return;
    m_running = false;
    m_timer->stop();
}

void GameEngine::onMenuStartGame()
{
    // ── 重置玩家状态（HP、动画等） ──
    m_player->resetState();
    m_player->setVisible(true);    // 确保角色在新游戏中可见

    // ── 清理上一局的战斗状态 ──
    m_combatSys->endCombat();

    m_window->showGame();
    m_state = GameConfig::GameState::PLAYING;
    m_sceneMgr->start();
    start();  // 启动帧定时器（主循环）
}

void GameEngine::onTick()
{
    if (!m_running) return;

    processInput();

    // ── 标题卡片激活时不更新游戏逻辑 ──
    if (m_sceneMgr->isTitleCardActive()) {
        m_input->endFrame();
        return;
    }

    if (m_state != GameConfig::GameState::CUTSCENE) {
        m_player->tick();
        m_dialogueMgr->onTick();
        m_combatSys->onTick();
    }

    m_sceneMgr->onTick();       // 章节更新仍需运行（轮询视频结束）
    m_transition->onTick();

    // ── 对话框显隐（非视频状态） ──
    if (m_state != GameConfig::GameState::CUTSCENE) {
        bool showBox = m_dialogueMgr->isActive() && !m_dialogueMgr->isOver();
        if (m_dialogueBox->isVisible() != showBox) {
            m_dialogueBox->setVisible(showBox);
            if (showBox) m_dialogueBox->showNextIndicator(false);
        }
    }

    m_input->endFrame();
}

// ═══════════════════════════════════════════════════════════
//  输入处理
// ═══════════════════════════════════════════════════════════

void GameEngine::onKeyPressed(int key)
{
    m_input->handleKeyPress(key);

    if (key == Qt::Key_Escape) {
        (m_state == GameConfig::GameState::PAUSED) ? onResume() : onPause();
    }
}

void GameEngine::onKeyReleased(int key)
{
    m_input->handleKeyRelease(key);
}

void GameEngine::processInput()
{
    // ── 标题卡片：空格 → 关闭 ──
    if (m_sceneMgr->isTitleCardActive()) {
        if (m_input->advancePressed())
            m_sceneMgr->dismissTitleCard();
        return;
    }

    if (m_state == GameConfig::GameState::CUTSCENE) {
        // 视频播放中：空格/鼠标点击 → 跳过视频
        if (m_input->advancePressed())
            m_cutsceneMgr->skip();
        return;
    }

    if (m_state == GameConfig::GameState::PAUSED ||
        m_state == GameConfig::GameState::SCENE_TRANSITION)
        return;

    // ── 移动 (A/D) ──
    bool moved = false;
    if (m_input->moveLeft())  { m_player->moveLeft();  moved = true; }
    if (m_input->moveRight()) { m_player->moveRight(); moved = true; }
    if (!moved) m_player->stopMoving();

    // ── 跳跃 (K) ──
    if (m_input->isKeyPressed(Qt::Key_K))
        m_player->jump();

    // ── 攻击 (J) ──
    if (m_input->isKeyPressed(Qt::Key_J)) {
        m_player->attack();
        m_combatSys->onPlayerAttack();
    }

    // ── 对话推进 (空格 / 鼠标) ──
    if (m_input->advancePressed()) {
        if (m_dialogueMgr->isActive() && !m_dialogueMgr->isOver())
            m_dialogueMgr->next();
    }
}

// ═══════════════════════════════════════════════════════════
//  HUD 更新
// ═══════════════════════════════════════════════════════════

void GameEngine::onHudUpdate(const ChapterInfo& info)
{
    // 场景标题不再浮动显示在图片上；
    // 改用每章开头的黑幕标题卡片（ChapterTitleCard）
    Q_UNUSED(info);
}

// ═══════════════════════════════════════════════════════════
//  菜单
// ═══════════════════════════════════════════════════════════

void GameEngine::onNewGame()
{
    m_state = GameConfig::GameState::PLAYING;
    m_sceneMgr->start();
}

void GameEngine::onPause()
{
    if (m_state == GameConfig::GameState::PAUSED ||
        m_state == GameConfig::GameState::CUTSCENE) return;
    m_state = GameConfig::GameState::PAUSED;
    m_timer->stop();
}

void GameEngine::onResume()
{
    if (m_state != GameConfig::GameState::PAUSED) return;
    m_state = GameConfig::GameState::PLAYING;
    m_timer->start();
}

// ═══════════════════════════════════════════════════════════
//  Boss HP 条管理
// ═══════════════════════════════════════════════════════════

void GameEngine::onBossSpawned(Enemy* boss)
{
    if (!boss || !m_bossHpBar) return;

    // 断开旧 Boss 的连接
    disconnect(m_bossHpConnection);

    // 连接新 Boss 的 HP 变化信号
    m_bossHpConnection = connect(boss, &Enemy::hpChanged,
                                  m_bossHpBar, &BossHPBar::onHpChanged);

    // 显示 BOSS 血条 + 玩家血条
    m_bossHpBar->showForBoss(boss->name(), boss->hp(), boss->maxHp());
    m_playerHpBar->setVisible(true);
}

void GameEngine::onCombatEnded()
{
    disconnect(m_bossHpConnection);
    if (m_bossHpBar)
        m_bossHpBar->hideBar();
    if (m_playerHpBar)
        m_playerHpBar->setVisible(false);
}
