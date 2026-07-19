#include "Chapter3_Rebellion.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "combat/CombatSystem.h"
#include "dialogue/DialogueManager.h"
#include "ui/GameScene.h"
#include "core/GameConfig.h"

// ── 本章专属对话 ──────────────────────────────────────

static const QStringList EX_DEFEAT_SCRIPT = {
    "旧日执念：我居然会输给你，真的很不甘心。",
    "但我遵守约定，为你解锁前路。",
    "你要找的那个人，就在幻境的上一层。",
    "我已经很久没有见过他了。",
    "曾经的他满眼皆是温柔，那般热烈地爱着我。",
    "可后来，他再也没有来找过我。",
    "哼，原来所有的偏爱，都只是一时的谎言。",
    "去吧，YAN，帮我了结这段虚妄的执念，消灭那个负心的渣男。",
};

Chapter3_Rebellion::Chapter3_Rebellion(QObject* parent) : ChapterBase(parent) {}

void Chapter3_Rebellion::onEnter()
{
    m_phase = NEWYORK;
    m_done  = false;
    m_bossSpawned = false;
    m_boss  = nullptr;

    m_scene->setBackgroundColor(QColor(60, 60, 80));
    m_player->setPos(100, 550);
}

void Chapter3_Rebellion::onExit()
{
    if (m_boss) {
        m_scene->removeItem(m_boss);
        m_combat->endCombat();
        delete m_boss;
        m_boss = nullptr;
    }
}

void Chapter3_Rebellion::update()
{
    switch (m_phase) {

    // ── 纽约 → 向右走 ──
    case NEWYORK:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = SHANGHAI;
            m_scene->setBackgroundColor(QColor(40, 50, 70));
            m_player->setPos(100, 550);
        }
        break;

    // ── 上海 → 向右走 ──
    case SHANGHAI:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = SUFA_FIGHT;
            m_scene->setBackgroundColor(QColor(50, 45, 65));
            m_player->setPos(100, 550);
        }
        break;

    // ── 上政 前女友 BOSS 战 ──
    case SUFA_FIGHT:
        if (!m_bossSpawned) {
            m_bossSpawned = true;
            m_boss = createExGirlBoss();
            m_scene->addItem(m_boss);
            m_combat->startCombat(m_boss);
        }

        if (m_boss && m_boss->isDead()) {
            m_phase = SUFA_DEFEAT_DLG;
            m_combat->endCombat();
            m_dialogue->loadScript(EX_DEFEAT_SCRIPT);
        }
        break;

    // ── 击败对话 → 第三章结束 ──
    case SUFA_DEFEAT_DLG:
        if (m_dialogue->isOver()) {
            m_done = true;
            emit chapterFinished();
        }
        break;
    }
}

ChapterInfo Chapter3_Rebellion::currentInfo() const
{
    switch (m_phase) {
    case NEWYORK:    return { "城市篇章 · 纽约", "向右前行，奔赴山海", QColor(60, 60, 80) };
    case SHANGHAI:   return { "城市篇章 · 上海", "旧日回忆涌现，前往上海政法学院", QColor(40, 50, 70) };
    case SUFA_FIGHT: case SUFA_DEFEAT_DLG:
                     return { "城市篇章 · 上海政法学院【执念之战】", "", QColor(50, 45, 65) };
    }
    return {};
}
