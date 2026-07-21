#include "Chapter5_Farewell.h"
#include "entity/Player.h"
#include "dialogue/DialogueManager.h"
#include "ui/GameScene.h"

// ── 终章诗歌 ──────────────────────────────────────────

Chapter5_Farewell::Chapter5_Farewell(QObject* parent)
    : ChapterBase(parent)
{
    m_slideshowTimer = new QTimer(this);
    m_slideshowTimer->setInterval(5000); // 5 seconds per slide
    connect(m_slideshowTimer, &QTimer::timeout, this, &Chapter5_Farewell::onSlideshowTick);

    // 终章幻灯片素材列表
    m_slides = {
        ":/images/ch05/YAN_Farewell.jpg",
        ":/images/ch05/YAN_Memory.png",
        ":/images/ch05/YAN_Eating.png",
        ":/images/ch05/YAN_Chirsmas.png",
        ":/images/ch05/YAN_Graduate.jpg",
        ":/images/ch05/YAN_Janpan.png",
        ":/images/ch05/YAN_NewYork.png",
        ":/images/ch05/YAN_ShangHai.png",
        ":/images/ch05/YAN_Sleep.png",
        ":/images/ch05/YAN_Train.jpg",
    };
}

void Chapter5_Farewell::onEnter()
{
    m_scene->setBackgroundColor(Qt::black);
    m_slideIndex = 0;
    m_started = false;

    // 显示第一张告别图片
    if (!m_slides.isEmpty()) {
        m_scene->setBackgroundImage(m_slides[0]);
    }

    // 隐藏玩家（终章不需要操控）
    m_player->hide();

    // 启动幻灯片轮播
    m_slideshowTimer->start();
}

void Chapter5_Farewell::onExit()
{
    m_slideshowTimer->stop();
    m_player->show();
    m_scene->clearBackgroundImage();
}

void Chapter5_Farewell::update()
{
    Q_UNUSED(m_started);
    // 终章是静态诗歌画面，轮播由定时器驱动
}

void Chapter5_Farewell::onSlideshowTick()
{
    if (m_slides.isEmpty()) return;
    m_slideIndex = (m_slideIndex + 1) % m_slides.size();
    m_scene->setBackgroundImage(m_slides[m_slideIndex]);
}

ChapterInfo Chapter5_Farewell::currentInfo() const
{
    return { QStringLiteral("终章 · 最后的诗"), QString(), Qt::black };
}
