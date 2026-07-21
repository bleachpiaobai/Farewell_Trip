#pragma once
#include <QObject>
#include <QTimer>
#include <QMetaObject>
#include "core/GameConfig.h"

class GameView;
class GameScene;
class MainWindow;
class InputManager;
class EventBus;
class ResourceManager;
class SceneManager;
class DialogueManager;
class CombatSystem;
class CutsceneManager;
class TransitionEffect;
class FPSCounter;
class Player;
class Enemy;
class SceneTitleLabel;
class DialogueBox;
class PlayerHPBar;
class BossHPBar;
struct ChapterInfo;

class GameEngine : public QObject
{
    Q_OBJECT
public:
    explicit GameEngine(MainWindow* window, QObject* parent = nullptr);
    ~GameEngine();

    void start();
    void stop();
    GameConfig::GameState state() const { return m_state; }

    // 子系统访问器
    GameView*    gameView()    const { return m_gameView; }
    GameScene*   gameScene()   const { return m_gameScene; }
    EventBus*    eventBus()    const { return m_eventBus; }
    InputManager* input()      const { return m_input; }
    SceneManager* scenes()     const { return m_sceneMgr; }
    DialogueManager* dialogue() const { return m_dialogueMgr; }
    CombatSystem*  combat()    const { return m_combatSys; }

public slots:
    void onNewGame();
    void onPause();
    void onResume();

private slots:
    void onTick();
    void onKeyPressed(int key);
    void onKeyReleased(int key);
    void onHudUpdate(const ChapterInfo& info);
    void onMenuStartGame();
    void onBossSpawned(Enemy* boss);
    void onCombatEnded();

private:
    void wireConnections();
    void processInput();

    // ── 子系统 ──
    MainWindow*      m_window      = nullptr;
    GameView*        m_gameView    = nullptr;
    GameScene*       m_gameScene   = nullptr;
    EventBus*        m_eventBus    = nullptr;
    InputManager*    m_input       = nullptr;
    ResourceManager* m_resources   = nullptr;
    SceneManager*    m_sceneMgr    = nullptr;
    DialogueManager* m_dialogueMgr = nullptr;
    CombatSystem*    m_combatSys   = nullptr;
    CutsceneManager* m_cutsceneMgr = nullptr;
    TransitionEffect* m_transition = nullptr;
    FPSCounter*      m_fpsCounter  = nullptr;

    // ── 实体 / UI ──
    Player*          m_player      = nullptr;
    SceneTitleLabel* m_sceneTitle  = nullptr;
    DialogueBox*     m_dialogueBox = nullptr;
    PlayerHPBar*     m_playerHpBar = nullptr;
    BossHPBar*       m_bossHpBar   = nullptr;

    // ── Boss HP 信号连接（随 Boss 变化） ──
    QMetaObject::Connection m_bossHpConnection;

    QTimer* m_timer = nullptr;
    GameConfig::GameState m_state = GameConfig::GameState::MAIN_MENU;
    bool m_running = false;
};
