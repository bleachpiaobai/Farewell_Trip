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
#include "ui/HUD.h"
#include <QDebug>

// ── 章节名称（黑幕标题） ────────────────────────────

static const char* CHAPTER_NAMES[] = {
    "第一章\n少女！从沉睡中苏醒",
    "第二章\n少女！决战桃子怪",
    "第三章\n少女！向天空高举叛逆之拳",
    "第四章\n少女！最后的幸存者",
    "第五章\n少女！银河告别旅行",
};

SceneManager::SceneManager(GameScene* scene, Player* player,
                           CombatSystem* combat, DialogueManager* dialogue,
                           TransitionEffect* transition,
                           CutsceneManager* cutscene,
                           QObject* parent)
    : QObject(parent)
    , m_scene(scene)
    , m_player(player)
    , m_combat(combat)
    , m_dialogue(dialogue)
    , m_transition(transition)
    , m_cutscene(cutscene)
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
        ch->setContext(m_scene, m_player, m_combat, m_dialogue, m_transition, m_cutscene);
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

    // 重置过渡效果
    if (m_transition)
        m_transition->reset();

    // ── 显示黑幕标题 ──
    if (m_titleCard && index < static_cast<int>(sizeof(CHAPTER_NAMES) / sizeof(CHAPTER_NAMES[0]))) {
        showTitleCard(index);
    } else {
        // Fallback: no title card available, enter directly
        enterChapter();
    }
}

void SceneManager::showTitleCard(int index)
{
    m_titleCardActive = true;
    m_pendingChapter = index;

    m_titleCard->show(QString::fromUtf8(CHAPTER_NAMES[index]));

    qDebug() << "[TitleCard] 显示第" << (index + 1) << "章标题";
}

void SceneManager::dismissTitleCard()
{
    if (!m_titleCardActive) return;

    m_titleCardActive = false;
    m_titleCard->hide();

    qDebug() << "[TitleCard] 标题关闭，进入章节";

    enterChapter();
}

void SceneManager::enterChapter()
{
    if (m_pendingChapter < 0 || m_pendingChapter >= m_chapters.size()) return;

    int index = m_pendingChapter;
    m_pendingChapter = -1;

    ChapterBase* ch = m_chapters[index];
    qDebug() << "[Chapter] 进入第" << (index + 1) << "章：" << ch->currentInfo().title;
    ch->onEnter();

    emit chapterChanged(index);
    emit hudUpdate(ch->currentInfo());
}

void SceneManager::onTick()
{
    // ── 黑幕标题状态下不更新章节逻辑 ──
    if (m_titleCardActive) return;

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
    } else {
        emit gameCompleted();
    }
}
