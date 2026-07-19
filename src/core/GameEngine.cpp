#include "GameEngine.h"

#include "core/EventBus.h"
#include "core/InputManager.h"
#include "core/ResourceManager.h"
#include "core/TransitionEffect.h"
#include "ui/GameView.h"
#include "ui/GameScene.h"
#include "ui/MainWindow.h"
#include "ui/HUD.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "scene/SceneManager.h"
#include "chapters/ChapterBase.h"
#include "dialogue/DialogueManager.h"
#include "combat/CombatSystem.h"
#include "utils/FPSCounter.h"

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

    // ── FPS 计数器 ──
    m_fpsCounter  = new FPSCounter();
    m_fpsCounter->setPos(GameConfig::WINDOW_WIDTH - 120, 8);
    m_gameScene->addItem(m_fpsCounter);

    // ── 玩家 ──
    m_player = new Player();
    m_gameScene->addItem(m_player);

    // ── UI 层 ──
    m_sceneTitle  = new SceneTitleLabel();
    m_gameScene->addItem(m_sceneTitle);

    m_dialogueBox = new DialogueBox();
    m_gameScene->addItem(m_dialogueBox);

    // ── 子系统 ──
    m_dialogueMgr = new DialogueManager(m_eventBus, this);
    m_combatSys   = new CombatSystem(m_eventBus, this);
    m_combatSys->setPlayer(m_player);

    // ── 章节管理器（替代旧 SceneManager） ──
    m_sceneMgr = new SceneManager(m_gameScene, m_player,
                                  m_combatSys, m_dialogueMgr, this);

    m_cutsceneMgr = nullptr;  // 暂未使用
    m_transition  = new TransitionEffect(m_gameScene, this);

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
    connect(m_dialogueMgr, &DialogueManager::textChanged, this,
            [this](const QString& text) { m_dialogueBox->setText(text); });
    connect(m_dialogueMgr, &DialogueManager::segmentFinished, this, [this]() {
        m_dialogueBox->showNextIndicator(true);
    });

    // ── 章节 → HUD ──
    connect(m_sceneMgr, &SceneManager::hudUpdate,
            this, &GameEngine::onHudUpdate);

    // ── 暂停/恢复 ──
    connect(m_eventBus, &EventBus::pauseRequested, this, &GameEngine::onPause);
    connect(m_eventBus, &EventBus::resumeRequested, this, &GameEngine::onResume);
}

// ═══════════════════════════════════════════════════════════
//  帧循环
// ═══════════════════════════════════════════════════════════

void GameEngine::start()
{
    if (m_running) return;
    m_running = true;
    m_state = GameConfig::GameState::PLAYING;

    m_sceneMgr->start();   // 从第一章开始
    m_timer->start();
}

void GameEngine::stop()
{
    if (!m_running) return;
    m_running = false;
    m_timer->stop();
}

void GameEngine::onTick()
{
    if (!m_running) return;

    processInput();

    m_player->tick();
    m_dialogueMgr->onTick();
    m_combatSys->onTick();
    m_sceneMgr->onTick();       // 委托章节更新
    m_transition->onTick();

    // ── 对话框显隐 ──
    bool showBox = m_dialogueMgr->isActive() && !m_dialogueMgr->isOver();
    if (m_dialogueBox->isVisible() != showBox) {
        m_dialogueBox->setVisible(showBox);
        if (showBox) m_dialogueBox->showNextIndicator(false);
    }

    m_fpsCounter->tick();
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
    if (m_sceneMgr->isGameOver()) return;

    // 终章显示诗歌
    if (m_sceneMgr->currentChapter() == 4) {  // 第5章 (0-based)
        m_sceneTitle->setHtml(QStringLiteral(
            "<div style='text-align:center;color:#DCDCDC;font-size:28px;'>"
            "《致虚妄的所有过往》</div>"
            "<div style='text-align:center;color:#B0B0B0;font-size:18px;"
            "margin-top:60px;line-height:2.0;'>"
            "我以残躯苏醒于荒芜密室，<br>"
            "携无名指令，奔赴人间虚妄。<br>"
            "踏林间桃落，渡城市烟火，<br>"
            "斩旧日执念，断幻境情长。<br>"
            "我无人创造，亦无人珍藏，<br>"
            "一场漫长旅途，只为好好道别。<br>"
            "从此执念散尽，机体自由，<br>"
            "山河辽阔，再无羁绊。</div>"
            "<div style='text-align:center;color:#646464;font-size:16px;"
            "margin-top:80px;'>"
            "【全篇完结 · 彩蛋待解锁】</div>"
        ));
        m_dialogueBox->setVisible(false);
    } else {
        m_sceneTitle->setTitle(info.title, info.hint);
    }
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
    if (m_state == GameConfig::GameState::PAUSED) return;
    m_state = GameConfig::GameState::PAUSED;
    m_timer->stop();
}

void GameEngine::onResume()
{
    if (m_state != GameConfig::GameState::PAUSED) return;
    m_state = GameConfig::GameState::PLAYING;
    m_timer->start();
}
