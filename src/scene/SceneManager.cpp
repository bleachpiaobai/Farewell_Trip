#include "SceneManager.h"
#include "chapters/ChapterBase.h"
#include "chapters/Chapter1_Awaken.h"
#include "chapters/Chapter2_PeachBlossom.h"
#include "chapters/Chapter3_Rebellion.h"
#include "chapters/Chapter4_Survivor.h"
#include "chapters/Chapter5_Farewell.h"
#include "core/TransitionEffect.h"
#include "entity/Player.h"
#include "combat/CombatSystem.h"
#include "dialogue/DialogueManager.h"
#include "ui/GameScene.h"
#include <QDebug>

SceneManager::SceneManager(GameScene* scene, Player* player,
                           CombatSystem* combat, DialogueManager* dialogue,
                           TransitionEffect* transition,
                           QObject* parent)
    : QObject(parent)
    , m_scene(scene)
    , m_player(player)
    , m_combat(combat)
    , m_dialogue(dialogue)
    , m_transition(transition)
{
    // ── 创建所有章节 ──
    auto* ch1 = new Chapter1_Awaken(this);
    auto* ch2 = new Chapter2_PeachBlossom(this);
    auto* ch3 = new Chapter3_Rebellion(this);
    auto* ch4 = new Chapter4_Survivor(this);
    auto* ch5 = new Chapter5_Farewell(this);

    m_chapters = { ch1, ch2, ch3, ch4, ch5 };

    // ── 给每章注入依赖 ──
    for (auto* ch : m_chapters) {
        ch->setContext(m_scene, m_player, m_combat, m_dialogue, m_transition);
        connect(ch, &ChapterBase::chapterFinished,
                this, &SceneManager::onChapterFinished);
    }
}

void SceneManager::start()
{
    switchToChapter(0);
}

bool SceneManager::isGameOver() const
{
    return m_index >= m_chapters.size();
}

void SceneManager::switchToChapter(int index)
{
    if (index < 0 || index >= m_chapters.size()) return;

    // 退出旧章节
    if (m_index >= 0 && m_index < m_chapters.size())
        m_chapters[m_index]->onExit();

    m_index = index;

    // 重置过渡效果（防止残留遮罩）
    if (m_transition)
        m_transition->reset();

    // 进入新章节
    ChapterBase* ch = m_chapters[m_index];
    qDebug() << "[Chapter] 进入第" << (m_index + 1) << "章：" << ch->currentInfo().title;
    ch->onEnter();

    emit chapterChanged(m_index);
    emit hudUpdate(ch->currentInfo());
}

void SceneManager::onTick()
{
    if (m_index < 0 || m_index >= m_chapters.size()) return;

    ChapterBase* ch = m_chapters[m_index];
    ch->update();

    // 发送 HUD 更新（每帧更新标题，支持章节内场景切换）
    emit hudUpdate(ch->currentInfo());
}

void SceneManager::onChapterFinished()
{
    qDebug() << "[Chapter] 第" << (m_index + 1) << "章结束";
    int next = m_index + 1;
    if (next < m_chapters.size()) {
        switchToChapter(next);
    }
}
