#include "Chapter2_PeachBlossom.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "combat/CombatSystem.h"
#include "dialogue/DialogueManager.h"
#include "ui/GameScene.h"
#include "core/GameConfig.h"

// ── 本章专属对话 ──────────────────────────────────────

static const QStringList PEACH_DEFEAT_SCRIPT = {
    "梨花怪：你赢了...",
    "可你不过是被主人抛弃的失败品而已。",
    "如果我拥有主人最新研发的PETG材料毁灭装甲，我绝对不会输给你。",
    "只是主人最近不知所踪，我的进化计划，被彻底搁置了...",
};

Chapter2_PeachBlossom::Chapter2_PeachBlossom(QObject* parent) : ChapterBase(parent) {}

void Chapter2_PeachBlossom::onEnter()
{
    m_phase = FOREST;
    m_done  = false;
    m_bossSpawned = false;
    m_boss  = nullptr;

    m_scene->setBackgroundColor(QColor(30, 80, 40));
    m_player->setPos(100, 550);
}

void Chapter2_PeachBlossom::onExit()
{
    // 清理 BOSS
    if (m_boss) {
        m_scene->removeItem(m_boss);
        m_combat->endCombat();
        delete m_boss;
        m_boss = nullptr;
    }
}

void Chapter2_PeachBlossom::update()
{
    switch (m_phase) {

    // ── 阶段1：森林 → 向右走到桃林 ──
    case FOREST:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = PEACH_FIGHT;
            m_scene->setBackgroundColor(QColor(255, 220, 225));
            m_player->setPos(100, 550);
        }
        break;

    // ── 阶段2：桃林 BOSS 战 ──
    case PEACH_FIGHT:
        if (!m_bossSpawned) {
            m_bossSpawned = true;
            m_boss = createPeachBoss();
            m_scene->addItem(m_boss);
            m_combat->startCombat(m_boss);
        }

        if (m_boss && m_boss->isDead()) {
            m_phase = PEACH_DEFEAT_DLG;
            m_combat->endCombat();
            m_dialogue->loadScript(PEACH_DEFEAT_SCRIPT);
        }
        break;

    // ── 阶段3：击败对话 → 第二章结束 ──
    case PEACH_DEFEAT_DLG:
        if (m_dialogue->isOver()) {
            m_done = true;
            emit chapterFinished();
        }
        break;
    }
}

ChapterInfo Chapter2_PeachBlossom::currentInfo() const
{
    switch (m_phase) {
    case FOREST:       return { "自然篇章 · 静谧森林", "向右行走，前往桃林", QColor(30, 80, 40) };
    case PEACH_FIGHT:  return { "自然篇章 · 十里桃林", "", QColor(255, 220, 225) };
    case PEACH_DEFEAT_DLG: return { "自然篇章 · 十里桃林", "", QColor(255, 220, 225) };
    }
    return {};
}
