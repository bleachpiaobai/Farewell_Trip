#include "Chapter4_Survivor.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "combat/CombatSystem.h"
#include "dialogue/DialogueManager.h"
#include "video/CutsceneManager.h"
#include "ui/GameScene.h"
#include "core/GameConfig.h"

// ═══════════════════════════════════════════════════════════════
//  对话脚本（严格匹配 Farewell_Trip.doc）
// ═══════════════════════════════════════════════════════════════

static const QStringList ENCOUNTER_DIALOGUE = {
    QStringLiteral("Angus：哔哔哔！！！！！检测到外来入侵者！为机构清除一切障碍！"),
};

static const QStringList RECOGNIZE_DIALOGUE = {
    QStringLiteral("Angus：—————————————"),
};

static const QStringList FLASHBACK_DIALOGUE = {
    QStringLiteral("Angus：————燕？！——————"),
    QStringLiteral("陌生男人：抱歉，我们没有挽留住英雄"),
    QStringLiteral("陌生男人：离开这里，你就是一个被埋没的普通人。留在这里，你将拥有无尽的荣誉"),
    QStringLiteral("陌生男人：我永远都不会再回到这里了，永远不会再为你，为你们所用！"),
};

static const QStringList CONFESSION_DIALOGUE = {
    QStringLiteral("Angus：啊！！！！！！！！！！"),
    QStringLiteral("Angus：对不起！燕！让你看到了这么丑陋的我！"),
    QStringLiteral("YAN：博士？！"),
    QStringLiteral("Angus：我本该随你而去，却苟延残喘至这副模样"),
    QStringLiteral("Angus：我罪孽深重"),
    QStringLiteral("Angus：从这里进去，就是星际列车的登车点。"),
    QStringLiteral("Angus：离开这里，答应我，幸福地活下去。"),
    QStringLiteral("Angus：自毁程序————启——"),
};

static const QStringList GOODBYE_DIALOGUE = {
    QStringLiteral("Angus：！！！！！！！！！！！杀戮！杀戮！"),
    QStringLiteral("YAN：博士？！"),
};

static const QStringList MOURN_DIALOGUE = {
    QStringLiteral("YAN：博士。。。。。。。"),
};

Chapter4_Survivor::Chapter4_Survivor(QObject* parent) : ChapterBase(parent) {}

void Chapter4_Survivor::onEnter()
{
    m_phase = ENCOUNTER;
    m_done  = false;
    m_cutscenePending = false;
    m_boss  = nullptr;

    m_scene->setBackgroundColor(QColor(50, 40, 35));
    m_scene->setBackgroundImage(":/images/ch04_survivor/YAN_Fight_Doc.png");
    m_player->setPos(100, 530);
    m_player->show();  // YAN visible for boss fight
    m_dialogue->loadScript(ENCOUNTER_DIALOGUE);
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
    if (m_done) return;

    // ── 视频等待：播完后跳转下一阶段 ──
    if (m_cutscenePending) {
        if (!m_cutscene || !m_cutscene->isPlaying()) {
            m_cutscenePending = false;
            switch (m_phase) {
            case ENCOUNTER:
                m_phase = RECOGNIZE;
                m_scene->setBackgroundColor(QColor(35, 30, 25));
                m_scene->setBackgroundImage(":/images/ch04_survivor/Doc_Ack.png");
                m_dialogue->loadScript(RECOGNIZE_DIALOGUE);
                break;
            case GOODBYE:
                m_phase = MOURN;
                m_scene->setBackgroundColor(QColor(20, 20, 25));
                m_scene->setBackgroundImage(":/images/ch04_survivor/YAN_Defeat_Doc.png");
                m_dialogue->loadScript(MOURN_DIALOGUE);
                break;
            case DEPARTURE:
                m_done = true;
                emit chapterFinished();
                break;
            default: break;
            }
        }
        return;
    }

    switch (m_phase) {

    // ── 遭遇 Angus → BOSS 战 → 视频 ──
    case ENCOUNTER:
        if (m_dialogue->isOver() && !m_boss) {
            m_boss = createAngusBoss();
            m_scene->addItem(m_boss);
            m_combat->startCombat(m_boss);
        }
        if (m_boss && m_boss->isDead()) {
            m_combat->endCombat();
            m_scene->removeItem(m_boss);
            delete m_boss;
            m_boss = nullptr;
            m_player->hide();  // boss defeated, dialogue only from here
            m_cutscenePending = true;
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_ACK_Doc", "videos/ch04_survivor/YAN_ACK_Doc.mp4");
        }
        break;

    // ── Angus 认出 YAN ──
    case RECOGNIZE:
        if (m_dialogue->isOver()) {
            m_phase = FLASHBACK;
            m_scene->setBackgroundColor(QColor(25, 20, 20));
            m_scene->setBackgroundImage(":/images/ch04_survivor/YAN_Die.jpg");
            m_dialogue->loadScript(FLASHBACK_DIALOGUE);
        }
        break;

    // ── 闪回：燕的牺牲 ──
    case FLASHBACK:
        if (m_dialogue->isOver()) {
            m_phase = CONFESSION;
            m_scene->setBackgroundColor(QColor(20, 15, 20));
            m_scene->setBackgroundImage(":/images/ch04_survivor/Doc_Lose_Control.png");
            m_dialogue->loadScript(CONFESSION_DIALOGUE);
        }
        break;

    // ── Angus 失控忏悔 → 自毁 ──
    case CONFESSION:
        if (m_dialogue->isOver()) {
            m_phase = GOODBYE;
            m_scene->setBackgroundColor(QColor(30, 10, 10));
            m_scene->setBackgroundImage(":/images/ch04_survivor/Doc_Lose_Control.png");
            m_dialogue->loadScript(GOODBYE_DIALOGUE);
        }
        break;

    // ── Angus 告别 → 视频 ──
    case GOODBYE:
        if (m_dialogue->isOver()) {
            m_cutscenePending = true;
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_Ack", "videos/ch04_survivor/YAN_Ack.mp4");
        }
        break;

    // ── YAN 哀悼博士 → 启程视频 ──
    case MOURN:
        if (m_dialogue->isOver()) {
            m_phase = DEPARTURE;
            m_scene->setBackgroundColor(QColor(15, 18, 25));
            m_scene->setBackgroundImage(":/images/ch04_survivor/Goodbye_Doc.png");
            m_cutscenePending = true;
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_Train", "videos/ch04_survivor/YAN_Train.mp4");
        }
        break;

    // ── 启程（视频播完后 chapterFinished） ──
    case DEPARTURE:
        break;
    }
}

ChapterInfo Chapter4_Survivor::currentInfo() const
{
    switch (m_phase) {
    case ENCOUNTER:   return { QStringLiteral("幻境篇章 · 决战Angus【BOSS战】"), QString(), QColor(50, 40, 35) };
    case RECOGNIZE:   return { QStringLiteral("幻境篇章 · 故人相认"), QString(), QColor(35, 30, 25) };
    case FLASHBACK:   return { QStringLiteral("幻境篇章 · 旧世回忆"), QString(), QColor(25, 20, 20) };
    case CONFESSION:  return { QStringLiteral("幻境篇章 · 博士的忏悔"), QString(), QColor(20, 15, 20) };
    case GOODBYE:     return { QStringLiteral("幻境篇章 · 最后的告别"), QString(), QColor(30, 10, 10) };
    case MOURN:       return { QStringLiteral("幻境篇章 · 哀悼"), QString(), QColor(20, 20, 25) };
    case DEPARTURE:   return { QStringLiteral("幻境篇章 · 启程"), QString(), QColor(15, 18, 25) };
    }
    return {};
}

#include "moc_Chapter4_Survivor.cpp"
