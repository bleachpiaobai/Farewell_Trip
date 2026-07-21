#include "Chapter1_Awaken.h"
#include "core/TransitionEffect.h"
#include "entity/Player.h"
#include "dialogue/DialogueManager.h"
#include "ui/GameScene.h"

// ── 本章专属对话（分为三组，对应三张图片） ──────────────

static const QStringList SCRIPT_1 = {
    QStringLiteral("我从密室中醒来，我被赋予生命。"),
    QStringLiteral("我是被主人创造出来的人形机器人——YAN。"),
    QStringLiteral("我完全丢失了所有内部存储信息，不知道自己为何被创造、为何苏醒。"),
};

static const QStringList SCRIPT_2 = {
    QStringLiteral("漫长的岁月里，我一直沉寂在这片冰冷的废弃仓库密室中。"),
    QStringLiteral("直到昨日，一串神秘的未知指令涌入我的核心程序。"),
    QStringLiteral("它轻轻呼唤着我，指引着我苏醒。"),
};

static const QStringList SCRIPT_3 = {
    QStringLiteral("仿佛在告诉我：世间仍有未完成的羁绊，我必须奔赴一场道别。"),
    QStringLiteral("于是，我冲破沉寂，响应感召，再次拥有了生命。"),
};

Chapter1_Awaken::Chapter1_Awaken(QObject* parent) : ChapterBase(parent) {}

void Chapter1_Awaken::onEnter()
{
    m_scene->setBackgroundColor(QColor(20, 20, 30));
    m_scene->setBackgroundImage(":/images/ch01/YAN_Start1.png");
    m_player->setPos(100, 550);

    m_dialogue->loadScript(SCRIPT_1);
    m_phase = IMAGE_1;
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

    switch (m_phase) {

    case IMAGE_1:
        if (m_dialogue->isOver()) {
            startTransitionTo(IMAGE_2);
        }
        break;

    case IMAGE_2:
        if (m_dialogue->isOver()) {
            startTransitionTo(IMAGE_3);
        }
        break;

    case IMAGE_3:
        if (m_dialogue->isOver()) {
            m_done = true;
            emit chapterFinished();
        }
        break;

    case DONE:
        break;
    }
}

void Chapter1_Awaken::switchToPhase(Phase phase)
{
    m_phase = phase;
    switch (phase) {
    case IMAGE_2:
        m_scene->setBackgroundImage(":/images/ch01/YAN_Start2.png");
        m_dialogue->loadScript(SCRIPT_2);
        break;
    case IMAGE_3:
        m_scene->setBackgroundImage(":/images/ch01/YAN_Start3.png");
        m_dialogue->loadScript(SCRIPT_3);
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
    return { QStringLiteral("密室 · 觉醒"), QString(), QColor(20, 20, 30) };
}
