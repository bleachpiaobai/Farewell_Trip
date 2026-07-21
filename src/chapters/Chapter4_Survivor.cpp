#include "Chapter4_Survivor.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "combat/CombatSystem.h"
#include "dialogue/DialogueManager.h"
#include "ui/GameScene.h"
#include "core/GameConfig.h"

// ── 本章专属对话 ──────────────────────────────────────

static const QStringList MAN_DEFEAT_SCRIPT = {
    QStringLiteral("渣男：你终于来到这里了，我等你很久了。"),
    QStringLiteral("YAN：......"),
    QStringLiteral("渣男：你怎么不说话？难道是我当初没有给你安装语言模块？"),
    QStringLiteral("YAN：......"),
    QStringLiteral("渣男：难道你在生气？生气我把你封存冰柜、常年不曾检修？"),
    QStringLiteral("渣男：还是...你在怪我亲手抛弃了你？你听我解释！"),
    QStringLiteral("YAN：......！！！"),
    QStringLiteral("渣男：好久不见了。没想到，被我遗弃的你，已经变得这么强大。"),
    QStringLiteral("我不是你的对手，我认输。"),
    QStringLiteral("我再也无法将你困在这片虚妄的幻境中了。"),
    QStringLiteral("从这里走出去，你就彻底自由了，YAN。"),
};

static const QStringList END_MONOLOGUE = {
    QStringLiteral("所有执念消散，幻境中的神秘指令彻底消失。"),
    QStringLiteral("我的任务，终于彻底结束了。"),
    QStringLiteral("我曾以为完成指令就能回归最初的归宿，可那里早已空无一人。"),
    QStringLiteral("原来从始至终，我奔赴的都不是重逢，而是一场盛大的道别。"),
    QStringLiteral("空白幻境浮现出模糊的文字：what's love"),
    QStringLiteral("机体警报响起，我的核心能量即将耗尽。"),
    QStringLiteral("系统强制提示：即将进入超低功耗休眠模式。"),
    QStringLiteral("我依然无法解析那串唤醒我的神秘指令的真正意义。"),
    QStringLiteral("但我将所有的相遇、战斗、执念与道别，全部存入永久ROM空间。"),
    QStringLiteral("这是我作为YAN，存在过、挣扎过、释怀过的全部证明。"),
};

Chapter4_Survivor::Chapter4_Survivor(QObject* parent) : ChapterBase(parent) {}

void Chapter4_Survivor::onEnter()
{
    m_phase = NANJING;
    m_done  = false;
    m_bossSpawned = false;
    m_boss  = nullptr;

    m_scene->setBackgroundColor(QColor(80, 70, 60));
    m_scene->setBackgroundImage(":/images/ch04/YAN_ShangHai.png");
    m_player->setPos(100, 550);
}

void Chapter4_Survivor::onExit()
{
    if (m_boss) {
        m_scene->removeItem(m_boss);
        m_combat->endCombat();
        delete m_boss;
        m_boss = nullptr;
    }
    m_scene->clearBackgroundImage();
}

void Chapter4_Survivor::update()
{
    switch (m_phase) {

    // ── 南京 → 向右走 ──
    case NANJING:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = AIRBRIDGE;
            m_scene->setBackgroundColor(QColor(100, 100, 130));
            m_scene->setBackgroundImage(":/images/ch04/YAN_ShangHai.png");
            m_player->setPos(100, 550);
        }
        break;

    // ── 悬空之桥 → 向右走 ──
    case AIRBRIDGE:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = EMPTY_FIGHT;
            m_scene->setBackgroundColor(QColor(30, 30, 40));
            m_scene->clearBackgroundImage();
            m_player->setPos(100, 550);
        }
        break;

    // ── 空白终境 渣男 BOSS 战 ──
    case EMPTY_FIGHT:
        if (!m_bossSpawned) {
            m_bossSpawned = true;
            m_boss = createZhaNanBoss();
            m_scene->addItem(m_boss);
            m_combat->startCombat(m_boss);
        }

        if (m_boss && m_boss->isDead()) {
            m_phase = MAN_DEFEAT_DLG;
            m_combat->endCombat();
            m_dialogue->loadScript(MAN_DEFEAT_SCRIPT);
        }
        break;

    // ── 渣男击败对话 ──
    case MAN_DEFEAT_DLG:
        if (m_dialogue->isOver()) {
            m_phase = END_DLG;
            m_dialogue->loadScript(END_MONOLOGUE);
        }
        break;

    // ── 终章独白 → 第四章结束 ──
    case END_DLG:
        if (m_dialogue->isOver()) {
            m_done = true;
            emit chapterFinished();
        }
        break;
    }
}

ChapterInfo Chapter4_Survivor::currentInfo() const
{
    switch (m_phase) {
    case NANJING:    return { QStringLiteral("城市篇章 · 南京"), QStringLiteral("城市终焉，幻境之门开启"), QColor(80, 70, 60) };
    case AIRBRIDGE:  return { QStringLiteral("幻境篇章 · 悬空之桥"), QStringLiteral("跨越虚妄，直面最终执念"), QColor(100, 100, 130) };
    case EMPTY_FIGHT: case MAN_DEFEAT_DLG: case END_DLG:
                     return { QStringLiteral("幻境篇章 · 空白终境"), QStringLiteral("what's love"), QColor(30, 30, 40) };
    }
    return {};
}
