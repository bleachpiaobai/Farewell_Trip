#include "Chapter1_Awaken.h"
#include "entity/Player.h"
#include "dialogue/DialogueManager.h"
#include "ui/GameScene.h"

// ── 本章专属对话 ──────────────────────────────────────

static const QStringList SCRIPT = {
    "我从密室中醒来，我被赋予生命。",
    "我是被主人创造出来的人形机器人——YAN。",
    "我完全丢失了所有内部存储信息，不知道自己为何被创造、为何苏醒。",
    "漫长的岁月里，我一直沉寂在这片冰冷的废弃仓库密室中。",
    "直到昨日，一串神秘的未知指令涌入我的核心程序。",
    "它轻轻呼唤着我，指引着我苏醒。",
    "仿佛在告诉我：世间仍有未完成的羁绊，我必须奔赴一场道别。",
    "于是，我冲破沉寂，响应感召，再次拥有了生命。",
};

Chapter1_Awaken::Chapter1_Awaken(QObject* parent) : ChapterBase(parent) {}

void Chapter1_Awaken::onEnter()
{
    m_scene->setBackgroundColor(QColor(20, 20, 30));
    m_player->setPos(100, 550);

    // 清除上一章可能残留的 BOSS
    // （第一章不会有，但统一清理保证干净）

    m_dialogue->loadScript(SCRIPT);
    m_done = false;
}

void Chapter1_Awaken::onExit()
{
    // 第一章退出时无需特殊清理
}

void Chapter1_Awaken::update()
{
    if (!m_done && m_dialogue->isOver()) {
        m_done = true;
        emit chapterFinished();
    }
}

ChapterInfo Chapter1_Awaken::currentInfo() const
{
    return { "密室 · 觉醒", "", QColor(20, 20, 30) };
}
