#include "Chapter4_Survivor.h"
#include "entity/Player.h"
#include "dialogue/DialogueManager.h"
#include "video/CutsceneManager.h"
#include "ui/GameScene.h"
#include "core/GameConfig.h"

// ═══════════════════════════════════════════════════════════════
//  对话脚本（严格匹配 Farewell_Trip.doc）
// ═══════════════════════════════════════════════════════════════

// 阶段1：遭遇 Angus —— YAN_Fight_Doc.png
static const QStringList ENCOUNTER_DIALOGUE = {
    QStringLiteral("Angus：哔哔哔！！！！！检测到外来入侵者！为机构清除一切障碍！"),
};

// 阶段2：Angus 认出 YAN —— Doc_Ack.png
static const QStringList RECOGNIZE_DIALOGUE = {
    QStringLiteral("Angus：—————————————"),
};

// 阶段3：认出燕 —— YAN_Die.jpg
static const QStringList FLASHBACK1_DIALOGUE = {
    QStringLiteral("Angus：————燕？！——————"),
};

// 阶段4：闪回+忏悔 —— Doc_Lose_Control0.png
static const QStringList FLASHBACK2_DIALOGUE = {
    QStringLiteral("陌生男人：抱歉，我们没有挽留住英雄"),
    QStringLiteral("陌生男人：离开这里，你就是一个被埋没的普通人。留在这里，你将拥有无尽的荣誉"),
    QStringLiteral("陌生男人：我永远都不会再回到这里了，永远不会再为你，为你们所用！"),
    QStringLiteral("Angus：啊！！！！！！！！！！"),
    QStringLiteral("YAN：博士？！"),
    QStringLiteral("Angus：我本该怀抱愧疚而死去，却苟延残喘至这副模样"),
    QStringLiteral("Angus：我。。。。。。。。对不起太多人了"),
    QStringLiteral("Angus：趁我还清醒，赶紧动手吧！"),
    QStringLiteral("Angus：我不想再伤害任何人了。。。。。。"),
    QStringLiteral("Angus：从这里进去，就是列车的登车点。"),
    QStringLiteral("Angus：离开这里，答应我，幸福地活下去。"),
    QStringLiteral("Angus：动手吧！YAN！"),
};

// 阶段5：Angus 失控杀戮 —— Doc_Lose_Control.png
static const QStringList GOODBYE_DIALOGUE = {
    QStringLiteral("Angus：！！！！！！！！！！！杀戮！杀戮！"),
    QStringLiteral("YAN：博士？！"),
    QStringLiteral("YAN：博士。。。。。。。。。。"),
};

// 阶段6：YAN 哀悼 —— YAN_Defeat_Doc.png
static const QStringList MOURN_DIALOGUE = {
    QStringLiteral("YAN：博士。。。。。。。"),
};

// 阶段6.5：告别博士 —— Goodbye_Doc.png
static const QStringList FAREWELL_DOC_DIALOGUE = {
    QStringLiteral("YAN：安息吧，博士"),
};

Chapter4_Survivor::Chapter4_Survivor(QObject* parent) : ChapterBase(parent) {}

void Chapter4_Survivor::onEnter()
{
    m_phase = WALK_IN;
    m_done  = false;
    m_cutscenePending = false;

    // YAN 先走进地图
    m_scene->setBackgroundColor(QColor(50, 40, 35));
    m_scene->setBackgroundImage(":/images/ch04_survivor/Background.png");
    m_player->setPos(100, 530);
    m_player->show();
}

void Chapter4_Survivor::onExit()
{
    m_scene->clearBackgroundImage();
}

void Chapter4_Survivor::update()
{
    if (m_done) return;

    // ── 视频等待：播完后等点击再进下一阶段 ──
    if (m_cutscenePending) {
        if (!m_cutscene || !m_cutscene->isPlaying()) {
            m_cutscenePending = false;
            if (m_cutscene) m_cutscene->cleanup();
            m_videoWait = true;
            m_dialogue->loadScript({QStringLiteral(" ")});
        }
        return;
    }

    // 视频播完后等待点击
    if (m_videoWait) {
        if (m_dialogue->isOver()) {
            m_videoWait = false;
            switch (m_phase) {
            case ENCOUNTER:
                m_phase = RECOGNIZE;
                m_dialogue->loadScript(RECOGNIZE_DIALOGUE);
                break;
            case GOODBYE:
                m_phase = MOURN;
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

    // ── 阶段0：YAN 走入地图 → 遭遇 Angus ──
    case WALK_IN:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = ENCOUNTER;
            m_player->hide();
            m_scene->setBackgroundColor(QColor(50, 40, 35));
            m_scene->setBackgroundImage(":/images/ch04_survivor/YAN_Fight_Doc.png");
            m_player->setPos(100, 530);
            m_dialogue->loadScript(ENCOUNTER_DIALOGUE);
        }
        break;

    // ── 阶段1：遭遇 Angus → 对话结束直接播视频 ──
    case ENCOUNTER:
        if (m_dialogue->isOver()) {
            m_cutscenePending = true;
            m_scene->setBackgroundImage(":/images/ch04_survivor/Doc_Ack.png");
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_ACK_Doc", "videos/ch04_survivor/YAN_ACK_Doc.mp4");
        }
        break;

    // ── 阶段2：Angus 认出 YAN ──
    case RECOGNIZE:
        if (m_dialogue->isOver()) {
            m_phase = FLASHBACK1;
            m_scene->setBackgroundColor(QColor(25, 20, 20));
            m_scene->setBackgroundImage(":/images/ch04_survivor/YAN_Die.jpg");
            m_dialogue->loadScript(FLASHBACK1_DIALOGUE);
        }
        break;

    // ── 阶段3：闪回+忏悔 —— Doc_Lose_Control0.png ──
    case FLASHBACK1:
        if (m_dialogue->isOver()) {
            m_phase = FLASHBACK2;
            m_scene->setBackgroundColor(QColor(20, 15, 20));
            m_scene->setBackgroundImage(":/images/ch04_survivor/Doc_Lose_Control0.png");
            m_dialogue->loadScript(FLASHBACK2_DIALOGUE);
        }
        break;

    // ── 阶段4：失控杀戮 —— Doc_Lose_Control.png ──
    case FLASHBACK2:
        if (m_dialogue->isOver()) {
            m_phase = GOODBYE;
            m_scene->setBackgroundColor(QColor(30, 10, 10));
            m_scene->setBackgroundImage(":/images/ch04_survivor/Doc_Lose_Control.png");
            m_dialogue->loadScript(GOODBYE_DIALOGUE);
        }
        break;

    // ── 阶段5：Angus 失控杀戮 → 播视频 ──
    case GOODBYE:
        if (m_dialogue->isOver()) {
            m_cutscenePending = true;
            m_scene->setBackgroundImage(":/images/ch04_survivor/YAN_Defeat_Doc.png");
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_Ack", "videos/ch04_survivor/YAN_Ack.mp4");
        }
        break;

    // ── 阶段6：YAN 哀悼博士 → 告别博士 ──
    case MOURN:
        if (m_dialogue->isOver()) {
            m_phase = FAREWELL_DOC;
            m_scene->setBackgroundColor(QColor(15, 18, 25));
            m_scene->setBackgroundImage(":/images/ch04_survivor/Goodbye_Doc.png");
            m_dialogue->loadScript(FAREWELL_DOC_DIALOGUE);
        }
        break;

    // ── 阶段7：告别博士 → 启程视频 ──
    case FAREWELL_DOC:
        if (m_dialogue->isOver()) {
            m_phase = DEPARTURE;
            m_cutscenePending = true;
            m_scene->clearBackgroundImage();  // 视频下铺黑屏，播完不闪旧图
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_Train", "videos/ch04_survivor/YAN_Train.mp4");
        }
        break;

    // ── 阶段8：启程（视频播完后 chapterFinished） ──
    case DEPARTURE:
        break;
    }
}

ChapterInfo Chapter4_Survivor::currentInfo() const
{
    switch (m_phase) {
    case WALK_IN:    return { QStringLiteral("幻境篇章 · 残垣"), QStringLiteral("向右行走"), QColor(50, 40, 35) };
    case ENCOUNTER:  return { QStringLiteral("幻境篇章 · 遭遇Angus"), QString(), QColor(50, 40, 35) };
    case RECOGNIZE:   return { QStringLiteral("幻境篇章 · 故人相认"), QString(), QColor(35, 30, 25) };
    case FLASHBACK1:  return { QStringLiteral("幻境篇章 · 旧世回忆"), QString(), QColor(25, 20, 20) };
    case FLASHBACK2:  return { QStringLiteral("幻境篇章 · 博士的忏悔"), QString(), QColor(20, 15, 20) };
    case GOODBYE:    return { QStringLiteral("幻境篇章 · 失控杀戮"), QString(), QColor(30, 10, 10) };
    case MOURN:        return { QStringLiteral("幻境篇章 · 哀悼"), QString(), QColor(20, 20, 25) };
    case FAREWELL_DOC: return { QStringLiteral("幻境篇章 · 告别博士"), QString(), QColor(15, 18, 25) };
    case DEPARTURE:    return { QStringLiteral("幻境篇章 · 启程"), QString(), QColor(15, 18, 25) };
    }
    return {};
}

#include "moc_Chapter4_Survivor.cpp"
