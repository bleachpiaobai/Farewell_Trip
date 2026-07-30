#include "Chapter5_Farewell.h"
#include "entity/Player.h"
#include "dialogue/DialogueManager.h"
#include "video/CutsceneManager.h"
#include "ui/GameScene.h"

// ═══════════════════════════════════════════════════════════════
//  记忆蒙太奇幻灯片（严格匹配 Farewell_Trip.doc）
//  空 dialogue = 自动跳过该页
// ═══════════════════════════════════════════════════════════════

static const QList<SlideData> SLIDES = {
    // 过场背景（无文字，自动跳过）
    { ":/images/ch05_farewell/Background1.png",      "" },
    { ":/images/ch05_farewell/Background2.png",      "" },
    // 记忆碎片
    { ":/images/ch05_farewell/YAN_Graduate.png",     QStringLiteral("燕：未来我会从事什么样的职业，会遇见什么样的人呢？") },
    { ":/images/ch05_farewell/YAN_ShangHai.png",     QStringLiteral("燕：今天遇见了一个奇怪的家伙。算了，不想了，夜上海啊，夜上海啊，你是一座不夜城~") },
    { ":/images/ch05_farewell/YAN_NewYork.png",      QStringLiteral("燕：Welcome to New York！") },
    { ":/images/ch05_farewell/YAN_Sleep.png",        QStringLiteral("燕：最近发生了好多事，好久没有睡这样一个好觉了") },
    { ":/images/ch05_farewell/YAN_Janpan.png",       QStringLiteral("燕：博士月色真美啊，你觉得呢？") },
    { ":/images/ch05_farewell/YAN_Eating.png",       QStringLiteral("燕：汉堡好好吃=￣ω￣=，博士你尝尝看！") },
    { ":/images/ch05_farewell/YAN_Chirsmas.png",     QStringLiteral("燕：圣诞快乐啊，博士") },
    { ":/images/ch05_farewell/YAN_Janpan.png",       QStringLiteral("燕：这里的烟花很美，如梦如幻，博士，你说如果一直活在此刻该多好的") },
    { ":/images/ch04_survivor/YAN_Lawyer.png",       QStringLiteral("燕：我们不会做任何的妥协！我们需要尊重与平等！") },
    { ":/images/ch04_survivor/YAN_Lawyer_Die.png",   QStringLiteral("燕：你大可以毁灭我，但你毁灭不了我们") },
    { "",                                   QStringLiteral("系统：(警告！！！！！)内存出错") },
    { ":/images/ch05_farewell/YAN_Sunflower.png",    QStringLiteral("燕： 你好啊，博士") },
    { ":/images/ch05_farewell/Doc_Create_YAN.png",   QStringLiteral("YAN：Hello,World!") },
    { ":/images/ch05_farewell/fail.png",             QStringLiteral("YAN：再见了，博士。") },
};

// 地球告别（循环点击切换）
static const QStringList EARTH_FAREWELL_SLIDES = {
    ":/images/ch05_farewell/Farewell_Earth1.png",
    ":/images/ch05_farewell/Farewell_Earth2.png",
    ":/images/ch05_farewell/Farewell_Earth3.png",
};

Chapter5_Farewell::Chapter5_Farewell(QObject* parent)
    : ChapterBase(parent) {}

void Chapter5_Farewell::onEnter()
{
    m_scene->setBackgroundColor(QColor(15, 20, 30));
    m_phase = OPENING;
    m_slideIndex = 0;
    m_autoSkip = false;

    m_player->hide();

    // 播放开场告别视频
    m_cutscenePending = true;
    if (m_cutscene)
        m_cutscene->playCutscene("Farewell", "videos/ch05_farewell/Farewell.mp4");
}

void Chapter5_Farewell::onExit()
{
    m_player->show();
    m_scene->clearBackgroundImage();
}

void Chapter5_Farewell::update()
{
    // Video pending — wait for playback to finish
    if (m_cutscenePending) {
        if (!m_cutscene || !m_cutscene->isPlaying()) {
            m_cutscenePending = false;
            if (m_phase == OPENING) {
                m_phase = SLIDESHOW;
                m_slideIndex = 0;
                showMemorySlide(0);
            } else if (m_phase == ENDING_VIDEO) {
                m_done = true;
                emit chapterFinished();
            }
        }
        return;
    }

    switch (m_phase) {

    // ── 开场视频 → 幻灯片 ──
    case OPENING:
        break;  // handled by cutscenePending above

    // ── 记忆蒙太奇（点击切换） ──
    case SLIDESHOW:
        if (m_autoSkip) {
            m_autoSkip = false;
            advanceSlide();
            break;
        }
        if (m_dialogue->isOver()) {
            advanceSlide();
        }
        break;

    // ── 地球告别（循环点击三张图，然后播放结局视频） ──
    case EARTH_FAREWELL:
        if (m_dialogue->isOver()) {
            m_earthCycleCount++;
            // After all 3 farewell images shown, play ending video
            if (m_earthCycleCount >= EARTH_FAREWELL_SLIDES.size()) {
                m_phase = ENDING_VIDEO;
                m_cutscenePending = true;
                if (m_cutscene)
                    m_cutscene->playCutscene("ending", "videos/ch05_farewell/ending.mp4");
                return;
            }
            m_slideIndex = (m_slideIndex + 1) % EARTH_FAREWELL_SLIDES.size();
            m_scene->setBackgroundImage(EARTH_FAREWELL_SLIDES[m_slideIndex]);
            if (m_slideIndex == 0)
                m_dialogue->loadScript({QStringLiteral("YAN：再见了，地球。")});
            else
                m_dialogue->loadScript({QStringLiteral("")});
        }
        break;

    case ENDING_VIDEO:
        break;  // handled by cutscenePending above
    }
}

void Chapter5_Farewell::showMemorySlide(int index)
{
    const auto& slide = SLIDES[index];
    if (!slide.imagePath.isEmpty())
        m_scene->setBackgroundImage(slide.imagePath);

    if (slide.dialogue.isEmpty()) {
        m_autoSkip = true;         // 空文本 → 下一帧自动跳
    } else {
        m_dialogue->loadScript({slide.dialogue});
    }
}

void Chapter5_Farewell::advanceSlide()
{
    m_slideIndex++;
    if (m_slideIndex < SLIDES.size()) {
        showMemorySlide(m_slideIndex);
    } else {
        // 幻灯片播完 → 地球告别循环
        m_phase = EARTH_FAREWELL;
        m_slideIndex = 0;
        m_scene->setBackgroundImage(EARTH_FAREWELL_SLIDES[0]);
        m_dialogue->loadScript({QStringLiteral("YAN：再见了，地球。")});
    }
}

ChapterInfo Chapter5_Farewell::currentInfo() const
{
    switch (m_phase) {
    case OPENING:        return { QStringLiteral("终章 · Fly to the Moon"), QString(), QColor(15, 20, 30) };
    case SLIDESHOW:      return { QStringLiteral("终章 · 少女！告别地球旅行"), QString(), Qt::black };
    case EARTH_FAREWELL: return { QStringLiteral("终章 · Fly to the Moon"), QString(), Qt::black };
    case ENDING_VIDEO:   return { QStringLiteral("终章 · 剧终"), QString(), Qt::black };
    }
    return {};
}

#include "moc_Chapter5_Farewell.cpp"
