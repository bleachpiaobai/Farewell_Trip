#pragma once
#include <QObject>
#include <QColor>
#include <QString>

class GameScene;
class Player;
class CombatSystem;
class DialogueManager;
class TransitionEffect;
class CutsceneManager;
class Enemy;

// ── 每章统一的接口 ──────────────────────────────

struct ChapterInfo {
    QString title;     // 场景标题
    QString hint;      // 操作提示（可为空）
    QColor  bgColor;   // 背景色
};

class ChapterBase : public QObject {
    Q_OBJECT
public:
    explicit ChapterBase(QObject* parent = nullptr) : QObject(parent) {}
    ~ChapterBase() override = default;

    // 注入依赖
    void setContext(GameScene* scene, Player* player,
                    CombatSystem* combat, DialogueManager* dialogue,
                    TransitionEffect* transition = nullptr,
                    CutsceneManager* cutscene = nullptr) {
        m_scene      = scene;
        m_player     = player;
        m_combat     = combat;
        m_dialogue   = dialogue;
        m_transition = transition;
        m_cutscene   = cutscene;
    }

    // ── 生命周期 ──
    virtual void onEnter() = 0;        // 进入本章
    virtual void onExit() = 0;         // 离开本章
    virtual void update() = 0;         // 每帧逻辑
    virtual bool isComplete() const = 0;
    virtual ChapterInfo currentInfo() const = 0;  // HUD 显示

signals:
    void chapterFinished();            // 本章结束，进入下一章

protected:
    GameScene*        m_scene      = nullptr;
    Player*           m_player     = nullptr;
    CombatSystem*     m_combat     = nullptr;
    DialogueManager*  m_dialogue   = nullptr;
    TransitionEffect* m_transition = nullptr;
    CutsceneManager*  m_cutscene   = nullptr;
};
