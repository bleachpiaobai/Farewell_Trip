#include "Chapter2_PeachBlossom.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "combat/CombatSystem.h"
#include "dialogue/DialogueManager.h"
#include "video/CutsceneManager.h"
#include "ui/GameScene.h"
#include "core/GameConfig.h"

// ── 本章专属对话（严格按照 Farewell_Trip.doc） ──────

// 阶段2：YAN出现在桃林 —— YAN_Appear_Peach.png
static const QStringList YAN_APPEAR_SCRIPT = {
    QStringLiteral("Peach：喂，哪里来的坏家——伙！"),
    QStringLiteral("YAN：请问——"),
};

// 阶段3：Peach 宣战 —— YAN_Fight_Peach.png
static const QStringList PEACH_FIGHT_DECLARE_SCRIPT = {
    QStringLiteral("Peach：我是不会给你们这些强盗任何东西的！看我召唤蜜桃战——宝！"),
    QStringLiteral("Peach：呃呃——都过去这么久了，博士的答应我的机甲怎么还没有做出来。。。。"),
    QStringLiteral("Peach：不管了，看招！"),
};

// 阶段5：Peach 被击败 —— YAN_Defeat_Peach.png
static const QStringList PEACH_DEFEAT_SCRIPT = {
    QStringLiteral("Peach：呃呃——我倒，不要吃我——啊！"),
    QStringLiteral("Peach：等一下，你看着好眼熟……"),
};

// 阶段6：Peach 认出 YAN —— Peach_Shock.png
static const QStringList PEACH_SHOCK_SCRIPT = {
    QStringLiteral("Peach：你是YAN？"),
    QStringLiteral("YAN：是的(￣▽￣)\""),
    QStringLiteral("Peach：你怎么不早——说！"),
    QStringLiteral("YAN：你也没问我啊(￣▽￣)\""),
    QStringLiteral("Peach：算了算了，博士在哪呢？她有答应给我做机甲的事情到现在还没有做呢，哼！"),
    QStringLiteral("YAN：博士是谁(￣▽￣)\"？？？"),
    QStringLiteral("Peach：博士是创造你的家伙啊！"),
    QStringLiteral("Peach：我是用于榨桃子汁的娱乐型机器人Peach"),
    QStringLiteral("Peach：我们都是博士创造的机器人"),
};

// 阶段7：Peach 哭泣 —— Peach_Cry.png
static const QStringList PEACH_CRY_SCRIPT = {
    QStringLiteral("Peach：我一直以为，当年你们抛下我，去往月球了。"),
    QStringLiteral("Peach：博士什么都没有跟你说过吗？"),
    QStringLiteral("YAN：我什么都不记得了。"),
    QStringLiteral("Peach：博士肯定是表面一套背地一套，把我们都抛弃了，自己一个人跑去月球了"),
    QStringLiteral("Peach：呜呜——呜。"),
    QStringLiteral("YAN：目前来讲，我对你所说的一无所知(￣▽￣)\""),
};

// 阶段8：Peach 叹息 —— Peach_Sigh.png
static const QStringList PEACH_SIGH_SCRIPT = {
    QStringLiteral("Peach：也是，不见的这么多年里，发生了太多的事了"),
    QStringLiteral("Peach：普通人早就死的死，逃的逃了"),
    QStringLiteral("Peach：健忘而已，很正常，能活着已经很幸运啦！"),
    QStringLiteral("YAN：话说我不是机器人吗，也会健忘吗(￣▽￣)\"？"),
    QStringLiteral("Peach：那我就不知道了"),
    QStringLiteral("Peach：谁知道博士有没有给你加这种无聊的设定，美其名曰让机器人像人一样活着"),
    QStringLiteral("Peach：话说回来，你真的一点都不记得了吗，看来我要好好给你恶补一下历史了！"),
    QStringLiteral("Peach：唉，这事就说来话长了"),
};

// 阶段9：Peach 老师模式 —— Peach_Teacher.png
static const QStringList PEACH_TEACHER_SCRIPT = {
    QStringLiteral("Peach：竖起小耳朵听好——了哇"),
    QStringLiteral("YAN：！！！！嗯嗯（￣︶￣）↗"),
    QStringLiteral("Peach：2125年地球的能源储备濒临枯竭"),
    QStringLiteral("Peach：随着资源逐渐耗尽，人类社会开始分裂成两种截然不同的思想阵营"),
    QStringLiteral("Peach：激进派认为，和平已经无法解决危机，唯有通过战争掠夺资源，让强者生存、弱者淘汰，才能延续人类文明。"),
    QStringLiteral("Peach：而保守派则坚持和平与秩序，主张通过资源共享和平均分配，让所有人共同度过这场危机"),
    QStringLiteral("Peach：开始是双方之间的怀疑与争论，随后是局部的试探与冲突，随后是冰冷的沉默与制裁，最后爆发成战争"),
    QStringLiteral("Peach：人们不断的互相伤害，让这颗曾经蔚蓝的星球不再适合居住"),
    QStringLiteral("Peach：你、我、博士，也就是我们。也都无法逃终止战争，也无法逃离战争。在民间分成了两股势力，"
                   "以\"King\"机构为首的中央和平军与民间组建而来的反抗军两股势力，久战不下。"),
    QStringLiteral("Peach：我和你们也就是在这的某一天失去了联系，最后中央和平军派出了机器士兵残酷镇压，"
                   "这场世界级别规模的大战，才终于结束"),
    QStringLiteral("Peach：这场战争透支了这个星球的未来，严重的自然灾害,加上基础设施的破坏，"
                   "使得获胜者还有没有品尝到胜利的果实，人类便着急启动了最后的希望——B计划\"Farewell_Trip\""),
    QStringLiteral("Peach：而所谓的告别旅行，正是告别地球，去往月球！开辟新的住所"),
};

// 阶段10：Peach 百年感慨 —— Peach_old.png
static const QStringList PEACH_OLD_SCRIPT = {
    QStringLiteral("Peach：转眼间竟已经过去百年之久"),
    QStringLiteral("Peach：而我也已经在这里榨了一百年的桃子汁，垂垂老——矣！"),
    QStringLiteral("YAN：!!!!!!!!(○´･д･)ﾉ"),
    QStringLiteral("Peach：且不说发射站那边现在是何面貌"),
    QStringLiteral("Peach：我对月球没有任何向往，我只想留在这颗曾经蔚蓝、孕育我的星球。"),
};

// 阶段11：图片轮播 — 发射中心 Shoot_Center.png
static const QStringList PEACH_TRAIN_SHOOT1_SCRIPT = {
    QStringLiteral("Peach：而你如果要离开，那来的正是时候。"),
};

// 阶段12：图片轮播 — 列车1 Train_1.png
static const QStringList PEACH_TRAIN_TRAIN1_SCRIPT = {
    QStringLiteral("Peach：每半年，巨型的无人列车就会驶过地球。"),
};

// 阶段13：图片轮播 — 列车2 Train_2.png
static const QStringList PEACH_TRAIN_TRAIN2_SCRIPT = {
    QStringLiteral("Peach：呼啸穿梭在废土之上，搜刮些这个星球最后的可能性。"),
};

// 阶段14：图片轮播 — 发射中心 Shoot_Center.png
static const QStringList PEACH_TRAIN_SHOOT2_SCRIPT = {
    QStringLiteral("Peach：你可以借此机会，等列车到来，就能搭乘配套载人火箭前往去上海的发射中心。"),
    QStringLiteral("Peach：不过老尼已经许久不再去过，如今是何面貌，一概不知了——了哇！"),
    QStringLiteral("YAN：!!!!!!!!(＃°Д°)"),
};

// 阶段16：告别对话 —— Peach_Goodbye.png
static const QStringList PEACH_GOODBYE_SCRIPT = {
    QStringLiteral("Peach：那就再见了——了哇！"),
    QStringLiteral("YAN：嗯嗯！b（￣▽￣）d"),
};

Chapter2_PeachBlossom::Chapter2_PeachBlossom(QObject* parent) : ChapterBase(parent) {}

void Chapter2_PeachBlossom::onEnter()
{
    m_phase = WASTELAND;
    m_done  = false;
    m_bossSpawned = false;
    m_boss  = nullptr;

    m_scene->setBackgroundColor(QColor(180, 160, 120));
    m_scene->setBackgroundImage(":/images/ch02_peach/Background.png");
    m_player->setPos(100, 530);
    m_player->show();  // walking on map background
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
    m_scene->clearBackgroundImage();
}

void Chapter2_PeachBlossom::update()
{
    // Video pending — wait for playback to finish
    if (m_cutscenePending) {
        if (!m_cutscene || !m_cutscene->isPlaying()) {
            m_cutscenePending = false;
            if (m_cutscene) m_cutscene->cleanup();
            if (m_phase == PEACH_VIDEO) {
                m_videoWait = true;
                m_dialogue->loadScript({QStringLiteral(" ")});
            }
        }
        return;
    }

    // 视频播完后等待点击再进入对话
    if (m_videoWait) {
        if (m_dialogue->isOver()) {
            m_videoWait = false;
            if (m_phase == PEACH_VIDEO) {
                m_phase = PEACH_GOODBYE;
                m_dialogue->loadScript(PEACH_GOODBYE_SCRIPT);
            }
        }
        return;
    }

    switch (m_phase) {

    // ── 阶段1：荒漠废土 → YAN向右走向桃林 ──
    case WASTELAND:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = YAN_APPEAR;
            m_player->hide();  // dialogue only from here
            m_scene->setBackgroundColor(QColor(255, 220, 225));
            m_scene->setBackgroundImage(":/images/ch02_peach/YAN_Appear_Peach.png");
            m_player->setPos(100, 530);
            m_dialogue->loadScript(YAN_APPEAR_SCRIPT);
        }
        break;

    // ── 阶段2：YAN出现在桃林，Peach遭遇 ──
    case YAN_APPEAR:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_FIGHT_DECLARE;
            m_scene->setBackgroundImage(":/images/ch02_peach/YAN_Fight_Peach.png");
            m_dialogue->loadScript(PEACH_FIGHT_DECLARE_SCRIPT);
        }
        break;

    // ── 阶段3：Peach宣战 ──
    case PEACH_FIGHT_DECLARE:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_FIGHT;
        }
        break;

    // ── 阶段4：桃林 BOSS 战（背景切换为地图 Background） ──
    case PEACH_FIGHT:
        if (!m_bossSpawned) {
            m_bossSpawned = true;
            m_player->show();  // YAN appears for boss fight
            m_scene->setBackgroundImage(":/images/ch02_peach/Background.png");
            m_boss = createPeachBoss();
            m_scene->addItem(m_boss);
            m_combat->startCombat(m_boss);
        }

        if (m_boss && m_boss->isDead()) {
            m_phase = PEACH_DEFEAT;
            m_player->hide();  // boss defeated, back to dialogue
            m_scene->setBackgroundImage(":/images/ch02_peach/YAN_Defeat_Peach.png");
            m_combat->endCombat();
            m_dialogue->loadScript(PEACH_DEFEAT_SCRIPT);
        }
        break;

    // ── 阶段5：Peach 被击败 ──
    case PEACH_DEFEAT:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_SHOCK;
            m_scene->setBackgroundImage(":/images/ch02_peach/Peach_Shock.png");
            m_dialogue->loadScript(PEACH_SHOCK_SCRIPT);
        }
        break;

    // ── 阶段6：Peach 震惊认出 YAN ──
    case PEACH_SHOCK:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_CRY;
            m_scene->setBackgroundImage(":/images/ch02_peach/Peach_Cry.png");
            m_dialogue->loadScript(PEACH_CRY_SCRIPT);
        }
        break;

    // ── 阶段7：Peach 哭泣 ──
    case PEACH_CRY:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_SIGH;
            m_scene->setBackgroundImage(":/images/ch02_peach/Peach_Sigh.png");
            m_dialogue->loadScript(PEACH_SIGH_SCRIPT);
        }
        break;

    // ── 阶段8：Peach 叹息 ──
    case PEACH_SIGH:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_TEACHER;
            m_scene->setBackgroundImage(":/images/ch02_peach/Peach_Teacher.png");
            m_dialogue->loadScript(PEACH_TEACHER_SCRIPT);
        }
        break;

    // ── 阶段9：Peach 讲述战争历史 ──
    case PEACH_TEACHER:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_OLD;
            m_scene->setBackgroundImage(":/images/ch02_peach/Peach_old.png");
            m_dialogue->loadScript(PEACH_OLD_SCRIPT);
        }
        break;

    // ── 阶段10：Peach 百年感慨 ──
    case PEACH_OLD:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_TRAIN_SHOOT1;
            m_scene->setBackgroundImage(":/images/ch02_peach/Shoot_Center.png");
            m_dialogue->loadScript(PEACH_TRAIN_SHOOT1_SCRIPT);
        }
        break;

    // ── 阶段11：图片轮播 — 发射中心 ──
    case PEACH_TRAIN_SHOOT1:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_TRAIN_TRAIN1;
            m_scene->setBackgroundImage(":/images/ch02_peach/Train_1.png");
            m_dialogue->loadScript(PEACH_TRAIN_TRAIN1_SCRIPT);
        }
        break;

    // ── 阶段12：图片轮播 — 列车1 ──
    case PEACH_TRAIN_TRAIN1:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_TRAIN_TRAIN2;
            m_scene->setBackgroundImage(":/images/ch02_peach/Train_2.png");
            m_dialogue->loadScript(PEACH_TRAIN_TRAIN2_SCRIPT);
        }
        break;

    // ── 阶段13：图片轮播 — 列车2 ──
    case PEACH_TRAIN_TRAIN2:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_TRAIN_SHOOT2;
            m_scene->setBackgroundImage(":/images/ch02_peach/Shoot_Center.png");
            m_dialogue->loadScript(PEACH_TRAIN_SHOOT2_SCRIPT);
        }
        break;

    // ── 阶段14：图片轮播 — 发射中心（续） ──
    case PEACH_TRAIN_SHOOT2:
        if (m_dialogue->isOver()) {
            m_phase = PEACH_VIDEO;
            m_cutscenePending = true;
            m_scene->setBackgroundImage(":/images/ch02_peach/Peach_Goodbye.png");
            if (m_cutscene)
                m_cutscene->playCutscene("Peach_Goodbye", "videos/ch02_peach/Peach_Goodbye.mp4");
        }
        break;

    // ── 阶段15：告别视频 (PEACH_VIDEO handled above) ──

    // ── 阶段16：告别对话 → 第二章结束 ──
    case PEACH_GOODBYE:
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
    case WASTELAND:            return { QStringLiteral("废土篇章 · 荒漠"), QStringLiteral("向右行走，前往桃林"), QColor(180, 160, 120) };
    case YAN_APPEAR:           return { QStringLiteral("废土篇章 · 十里桃林"), QStringLiteral("YAN遭遇Peach"), QColor(255, 220, 225) };
    case PEACH_FIGHT_DECLARE:  return { QStringLiteral("废土篇章 · 十里桃林"), QStringLiteral("少女！决战桃子怪"), QColor(255, 220, 225) };
    case PEACH_FIGHT:          return { QStringLiteral("废土篇章 · 十里桃林【BOSS战】"), QString(), QColor(255, 220, 225) };
    case PEACH_DEFEAT:         return { QStringLiteral("废土篇章 · 击败Peach"), QString(), QColor(255, 220, 225) };
    case PEACH_SHOCK:          return { QStringLiteral("废土篇章 · 故人相认"), QString(), QColor(255, 220, 225) };
    case PEACH_CRY:            return { QStringLiteral("废土篇章 · 被遗弃的真相"), QString(), QColor(255, 200, 210) };
    case PEACH_SIGH:           return { QStringLiteral("废土篇章 · 时光荏苒"), QString(), QColor(255, 210, 200) };
    case PEACH_TEACHER:        return { QStringLiteral("废土篇章 · 战争历史"), QString(), QColor(255, 200, 190) };
    case PEACH_OLD:            return { QStringLiteral("废土篇章 · 百年孤寂"), QString(), QColor(240, 200, 180) };
    case PEACH_TRAIN_SHOOT1:   return { QStringLiteral("废土篇章 · 星际列车"), QString(), QColor(220, 200, 170) };
    case PEACH_TRAIN_TRAIN1:   return { QStringLiteral("废土篇章 · 星际列车"), QString(), QColor(220, 200, 170) };
    case PEACH_TRAIN_TRAIN2:   return { QStringLiteral("废土篇章 · 星际列车"), QString(), QColor(220, 200, 170) };
    case PEACH_TRAIN_SHOOT2:   return { QStringLiteral("废土篇章 · 星际列车"), QString(), QColor(220, 200, 170) };
    case PEACH_GOODBYE:        return { QStringLiteral("废土篇章 · 告别Peach"), QString(), QColor(200, 180, 160) };
    }
    return {};
}
