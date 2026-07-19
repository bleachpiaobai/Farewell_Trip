#include "Chapter5_Farewell.h"
#include "entity/Player.h"
#include "dialogue/DialogueManager.h"
#include "ui/GameScene.h"

// ── 终章诗歌 ──────────────────────────────────────────

Chapter5_Farewell::Chapter5_Farewell(QObject* parent) : ChapterBase(parent) {}

void Chapter5_Farewell::onEnter()
{
    m_scene->setBackgroundColor(Qt::black);
    m_started = false;

    // 隐藏玩家（终章不需要操控）
    m_player->hide();
}

void Chapter5_Farewell::onExit()
{
    m_player->show();
}

void Chapter5_Farewell::update()
{
    Q_UNUSED(m_started);
    // 终章是静态诗歌画面，无需帧更新逻辑
    // 玩家可以在这里停留，或者按 ESC 退出
}

ChapterInfo Chapter5_Farewell::currentInfo() const
{
    return { "终章 · 最后的诗", "", Qt::black };
}
