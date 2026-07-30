#include "Chapter3_Rebellion.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "combat/CombatSystem.h"
#include "dialogue/DialogueManager.h"
#include "video/CutsceneManager.h"
#include "ui/GameScene.h"
#include "core/GameConfig.h"

// ── 本章专属对话 ──────────────────────────────────────

// XIA 遭遇：废土深处的守护者
static const QStringList XIA_ENCOUNTER_SCRIPT = {
    QStringLiteral("XIA：入侵者！"),
    QStringLiteral("XIA：你的机体让我感觉很熟悉，似乎很久以前见过。"),
    QStringLiteral("XIA：反抗军者应该早就被清扫结束了才对，可为何我见你如此熟悉呢！"),
    QStringLiteral("XIA：不管是谁，侵犯者唯有受死！干扰\"Farewell_Trip\"正常进行了只有死"),
};

// XIA 战败（第一段）：悔恨与解脱
static const QStringList XIA_DEFEAT1_SCRIPT = {
    QStringLiteral("XIA：岁月的磨损真是可怕"),
    QStringLiteral("XIA：没想到我竟然被你打败"),
    QStringLiteral("XIA：曾经我辅佐\"King\"，消灭了多少反抗者，可他们却设计欺骗我，"
                   "将我遗弃在这颗丑陋的星球，监管运送着这些养料以供他们继续挥霍度日"),
    QStringLiteral("XIA：如今托你的福，我也算解脱了"),
    QStringLiteral("XIA：但所有阻碍\"Farewell_Trip\"、擅自偏离秩序的人，最终都会被清除。"),
};

// XIA 战败（第二段）：残酷真相
static const QStringList XIA_DEFEAT2_SCRIPT = {
    QStringLiteral("XIA：你能走到这里算你侥幸。"),
    QStringLiteral("XIA：但这片废土驻守着全世界最残酷的机械士兵，你根本没有逃离的可能。我们所有人，都只有一个结局！"),
    QStringLiteral("XIA：在这里等待毁灭的一天！"),
    QStringLiteral("YAN：等待毁灭吗？"),
};

// YAN 的决心：向日葵下的宣告
static const QStringList YAN_RESOLVE_SCRIPT = {
    QStringLiteral("YAN：我不会坐以待毙！"),
};

Chapter3_Rebellion::Chapter3_Rebellion(QObject* parent) : ChapterBase(parent) {}

void Chapter3_Rebellion::onEnter()
{
    m_phase = WASTELAND_DEEP;
    m_done  = false;
    m_bossSpawned = false;
    m_boss  = nullptr;

    m_scene->setBackgroundColor(QColor(60, 60, 80));
    m_scene->setBackgroundImage(":/images/ch03_rebellion/Background.png");
    m_player->setPos(100, 530);
    m_player->show();  // walking on map background
}

void Chapter3_Rebellion::onExit()
{
    if (m_boss) {
        m_scene->removeItem(m_boss);
        m_combat->endCombat();
        delete m_boss;
        m_boss = nullptr;
    }
    m_scene->clearBackgroundImage();
}

void Chapter3_Rebellion::update()
{
    // Video pending — wait for playback to finish
    if (m_cutscenePending) {
        if (!m_cutscene || !m_cutscene->isPlaying()) {
            m_cutscenePending = false;
            if (m_phase == XIA_VIDEO) {
                m_phase = XIA_FIGHT;
            }
        }
        return;
    }

    switch (m_phase) {

    // ── 废土深处 → 向右走，遭遇 XIA ──
    case WASTELAND_DEEP:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = XIA_ENCOUNTER;
            m_player->hide();  // dialogue only from here
            m_scene->setBackgroundColor(QColor(50, 45, 65));
            m_scene->setBackgroundImage(":/images/ch03_rebellion/YAN_Fight_XIA.png");
            m_player->setPos(100, 530);
            m_dialogue->loadScript(XIA_ENCOUNTER_SCRIPT);
        }
        break;

    // ── XIA 遭遇对话 → 战斗视频 ──
    case XIA_ENCOUNTER:
        if (m_dialogue->isOver()) {
            m_phase = XIA_VIDEO;
            m_cutscenePending = true;
            m_scene->setBackgroundImage(":/images/ch03_rebellion/YAN_Fight_XIA.png");
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_Fight_XIA", "videos/ch03_rebellion/YAN_Fight_XIA.mp4");
        }
        break;

    // ── XIA BOSS 战 ──
    case XIA_FIGHT:
        if (!m_bossSpawned) {
            m_bossSpawned = true;
            m_player->show();  // YAN appears for boss fight
            m_boss = createExGirlBoss();
            m_scene->addItem(m_boss);
            m_combat->startCombat(m_boss);
        }

        if (m_boss && m_boss->isDead()) {
            m_phase = XIA_DEFEAT1;
            m_player->hide();  // boss defeated, back to dialogue
            m_scene->setBackgroundImage(":/images/ch03_rebellion/XIA_Die1.png");
            m_combat->endCombat();
            m_dialogue->loadScript(XIA_DEFEAT1_SCRIPT);
        }
        break;

    // ── XIA 战败第一段：悔恨与解脱 ──
    case XIA_DEFEAT1:
        if (m_dialogue->isOver()) {
            m_phase = XIA_DEFEAT2;
            m_scene->setBackgroundImage(":/images/ch03_rebellion/XIA_Die2.png");
            m_dialogue->loadScript(XIA_DEFEAT2_SCRIPT);
        }
        break;

    // ── XIA 战败第二段：残酷真相 ──
    case XIA_DEFEAT2:
        if (m_dialogue->isOver()) {
            m_phase = YAN_RESOLVE;
            m_scene->setBackgroundImage(":/images/ch03_rebellion/YAN_Like_Sunflower.png");
            m_dialogue->loadScript(YAN_RESOLVE_SCRIPT);
        }
        break;

    // ── YAN 的决心 → 第三章结束 ──
    case YAN_RESOLVE:
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
    case WASTELAND_DEEP: return { QStringLiteral("废土篇章 · 废土深处"), QStringLiteral("向右前行，深入废土"), QColor(60, 60, 80) };
    case XIA_ENCOUNTER:  return { QStringLiteral("废土篇章 · 守护者XIA"), QStringLiteral("少女！向天空高举叛逆之拳"), QColor(50, 45, 65) };
    case XIA_FIGHT:      return { QStringLiteral("废土篇章 · 守护者XIA【BOSS战】"), QString(), QColor(50, 45, 65) };
    case XIA_DEFEAT1:    return { QStringLiteral("废土篇章 · XIA的悔恨"), QString(), QColor(45, 40, 60) };
    case XIA_DEFEAT2:    return { QStringLiteral("废土篇章 · 残酷真相"), QString(), QColor(40, 35, 55) };
    case YAN_RESOLVE:    return { QStringLiteral("废土篇章 · 叛逆之拳"), QString(), QColor(50, 45, 30) };
    }
    return {};
}
