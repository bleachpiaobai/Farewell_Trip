#pragma once
#include <QObject>
#include <QVector>
#include "core/GameConfig.h"

class GameScene;
class Player;
class CombatSystem;
class DialogueManager;
class ChapterBase;
struct ChapterInfo;

// ── 章节管理器：只负责调度，不包含任何剧情数据 ────────

class SceneManager : public QObject
{
    Q_OBJECT
public:
    SceneManager(GameScene* scene, Player* player,
                 CombatSystem* combat, DialogueManager* dialogue,
                 QObject* parent = nullptr);

    void start();               // 从第一章开始
    void onTick();              // 每帧委托给当前章节

    int  currentChapter() const { return m_index; }
    int  totalChapters()  const { return m_chapters.size(); }
    bool isGameOver()     const;

signals:
    void chapterChanged(int index);
    void hudUpdate(const ChapterInfo& info);

private slots:
    void onChapterFinished();

private:
    void switchToChapter(int index);

    GameScene*       m_scene;
    Player*          m_player;
    CombatSystem*    m_combat;
    DialogueManager* m_dialogue;

    QVector<ChapterBase*> m_chapters;
    int m_index = -1;
};
