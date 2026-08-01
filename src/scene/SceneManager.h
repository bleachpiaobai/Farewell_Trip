#pragma once
#include <QObject>
#include <QVector>
#include "core/GameConfig.h"

class GameScene;
class Player;
class CombatSystem;
class DialogueManager;
class TransitionEffect;
class CutsceneManager;
class ChapterBase;
class ChapterTitleCard;
struct ChapterInfo;

// ── 章节管理器：只负责调度，不包含任何剧情数据 ────────

class SceneManager : public QObject
{
    Q_OBJECT
public:
    SceneManager(GameScene* scene, Player* player,
                 CombatSystem* combat, DialogueManager* dialogue,
                 TransitionEffect* transition,
                 CutsceneManager* cutscene = nullptr,
                 QObject* parent = nullptr);

    void start();               // 从第一章开始
    void onTick();              // 每帧委托给当前章节

    int  currentChapter() const { return m_index; }
    int  totalChapters()  const { return m_chapters.size(); }
    bool isGameOver()     const;

    /// Inject the title card (created by GameEngine)
    void setTitleCard(ChapterTitleCard* card) { m_titleCard = card; }

    /// Dismiss title card (called by GameEngine on space/click)
    void dismissTitleCard();

    /// Whether the title card is currently showing
    bool isTitleCardActive() const { return m_titleCardActive; }

signals:
    void chapterChanged(int index);
    void hudUpdate(const ChapterInfo& info);
    void gameCompleted();  // 所有章节结束 → 返回菜单

private slots:
    void onChapterFinished();

private:
    void switchToChapter(int index);
    void showTitleCard(int index);
    void enterChapter();

    GameScene*        m_scene;
    Player*           m_player;
    CombatSystem*     m_combat;
    DialogueManager*  m_dialogue;
    TransitionEffect* m_transition;
    CutsceneManager*  m_cutscene = nullptr;

    ChapterTitleCard* m_titleCard = nullptr;
    QVector<ChapterBase*> m_chapters;
    int m_index = -1;

    // ── Title card state ──
    bool m_titleCardActive = false;
    int  m_pendingChapter = -1;
};
