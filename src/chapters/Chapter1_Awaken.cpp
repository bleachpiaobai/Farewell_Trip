#include "Chapter1_Awaken.h"
#include "core/TransitionEffect.h"
#include "entity/Player.h"
#include "dialogue/DialogueManager.h"
#include "video/CutsceneManager.h"
#include "ui/GameScene.h"

// ── 本章专属对话（严格按照 Farewell_Trip.doc） ──────

// 阶段1：生日回忆 —— 百年之前的温暖碎片
static const QStringList SCRIPT_BIRTHDAY = {
    QStringLiteral("（眨眼的特效）"),
    QStringLiteral("陌生女人：博士！博士！"),
    QStringLiteral("陌生女人：生日快乐！"),
    QStringLiteral("陌生女人：我们给你带了蛋糕，还有小花(●'◡'●)~！"),
    QStringLiteral("陌生女人：只可惜气候越来越差了，我们找了很久的向日葵都没有找到。"),
    QStringLiteral("陌生女人：所以只能买了仿真的将就一下了。"),
    QStringLiteral("陌生男人：你可真是小机灵鬼，连我喜欢什么都记得这么清。"),
    QStringLiteral("陌生女人：嘿呀，快许愿吧，博士！"),
    QStringLiteral("陌生女人：我已经有点饿了。"),
    QStringLiteral("陌生男人：嗯"),
    QStringLiteral("陌生女人：博士，你的愿望是什么？"),
    QStringLiteral("陌生男人：我的愿望是————"),
    QStringLiteral("陌生女人：嗯？！快说呀~(*/ω＼*)博士，到底是什么？"),
    QStringLiteral("陌生男人：呼——————！"),
    QStringLiteral("陌生女人：(￢︿￢☆)哼！"),
    QStringLiteral("（电流杂音：滋滋滋）"),
};

// 阶段2：系统重启 —— 百年预埋指令触发
static const QStringList SCRIPT_REBOOT = {
    QStringLiteral("（眨眼的特效）"),
    QStringLiteral("系统音：检测到最高权限指令激活，系统重启中"),
    QStringLiteral("系统音：智能感知模块启用成功"),
    QStringLiteral("系统音：语言模块启用成功"),
    QStringLiteral("系统音：尝试连接本地数据库"),
    QStringLiteral("系统音：当前数据库为空"),
    QStringLiteral("系统音：启用设定文件，读取中……"),
    QStringLiteral("系统音：YAN，女性家庭服务型机器人，生产日期2130年"),
    QStringLiteral("系统音：主动模式解锁，开机完成"),
};

// 阶段3：觉醒 —— 数据清空，何去何从
static const QStringList SCRIPT_AWAKEN = {
    QStringLiteral("YAN：奇怪，我好像做了一个很长的梦"),
    QStringLiteral("YAN：一觉醒来，所有的数据全被清空了"),
    QStringLiteral("YAN：我该何去何从呢？"),
    QStringLiteral("YAN：真是一个值得思考的问题"),
};

// 阶段4：觅食 —— 吃饱了再想
static const QStringList SCRIPT_HUNGRY = {
    QStringLiteral("YAN： 呃~"),
    QStringLiteral("YAN： (⊙﹏⊙)~~~~~"),
    QStringLiteral("YAN：肚子好饿呀，去外面找点电池吃一下好了o(*￣▽￣*)ブ"),
    QStringLiteral("YAN：这么复杂的事情等吃完饭再慢慢思考吧"),
};

Chapter1_Awaken::Chapter1_Awaken(QObject* parent) : ChapterBase(parent) {}

void Chapter1_Awaken::onEnter()
{
    m_player->hide();

    m_scene->setBackgroundColor(QColor(40, 35, 28));
    m_scene->setBackgroundImage(":/images/ch01_awaken/Birthday_1.png");
    m_player->setPos(100, 530);

    m_dialogue->loadScript(SCRIPT_BIRTHDAY);
    m_phase = BIRTHDAY;
    m_transitioning = false;
    m_done = false;
}

void Chapter1_Awaken::onExit()
{
    m_scene->clearBackgroundImage();
}

void Chapter1_Awaken::update()
{
    if (m_done || m_transitioning) return;

    // Video pending — wait for playback to finish
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
            m_done = true;
            emit chapterFinished();
        }
        return;
    }

    switch (m_phase) {

    case BIRTHDAY:
        if (m_dialogue->isOver()) {
            startTransitionTo(REBOOT);
        }
        break;

    case REBOOT:
        if (m_dialogue->isOver()) {
            startTransitionTo(AWAKEN);
        }
        break;

    case AWAKEN:
        if (m_dialogue->isOver()) {
            startTransitionTo(HUNGRY);
        }
        break;

    case HUNGRY:
        if (m_dialogue->isOver()) {
            m_phase = VIDEO_AWAKE;
            m_cutscenePending = true;
            m_scene->clearBackgroundImage();  // 视频下铺黑屏，播完不闪旧图
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_Awake", "videos/ch01_awaken/YAN_Awake.mp4");
        }
        break;

    case VIDEO_AWAKE:
    case DONE:
        break;
    }
}

void Chapter1_Awaken::switchToPhase(Phase phase)
{
    m_phase = phase;
    switch (phase) {
    case REBOOT:
        m_scene->setBackgroundColor(QColor(15, 15, 25));
        m_scene->setBackgroundImage(":/images/ch01_awaken/YAN_Start1.png");
        m_dialogue->loadScript(SCRIPT_REBOOT);
        break;
    case AWAKEN:
        m_scene->setBackgroundColor(QColor(18, 22, 35));
        m_scene->setBackgroundImage(":/images/ch01_awaken/YAN_Start2.png");
        m_dialogue->loadScript(SCRIPT_AWAKEN);
        break;
    case HUNGRY:
        m_scene->setBackgroundColor(QColor(25, 28, 38));
        m_scene->setBackgroundImage(":/images/ch01_awaken/YAN_Start3.png");
        m_dialogue->loadScript(SCRIPT_HUNGRY);
        break;
    default:
        break;
    }
}

void Chapter1_Awaken::startTransitionTo(Phase nextPhase)
{
    if (!m_transition) {
        // 没有过渡效果，直接切换
        switchToPhase(nextPhase);
        return;
    }

    m_transitioning = true;

    // crossFade: 淡出到黑 → 切换图片 → 淡入回来
    m_transition->crossFade([this, nextPhase]() {
        switchToPhase(nextPhase);
    });

    // 淡入完成后恢复交互
    connect(m_transition, &TransitionEffect::fadeInFinished, this, [this]() {
        m_transitioning = false;
    }, Qt::SingleShotConnection);
}

ChapterInfo Chapter1_Awaken::currentInfo() const
{
    switch (m_phase) {
    case BIRTHDAY: return { QStringLiteral("百年前 · 生日"), QString(), QColor(40, 35, 30) };
    case REBOOT:   return { QStringLiteral("密室 · 系统重启"), QString(), QColor(15, 15, 25) };
    case AWAKEN:   return { QStringLiteral("密室 · 觉醒"), QString(), QColor(18, 22, 35) };
    case HUNGRY:   return { QStringLiteral("密室 · 出发"), QString(), QColor(25, 28, 38) };
    case DONE:     return {};
    }
    return {};
}
