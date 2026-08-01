#include "Chapter5_Farewell.h"
#include "entity/Player.h"
#include "dialogue/DialogueManager.h"
#include "video/CutsceneManager.h"
#include "ui/GameScene.h"
#include "core/GameConfig.h"

// ═══════════════════════════════════════════════════════════════
//  记忆蒙太奇幻灯片（严格按照 Farewell_Trip.doc）
// ═══════════════════════════════════════════════════════════════

// 幻灯片（系统警告之前的部分）
static const QList<SlideData> SLIDES_PART1 = {
    { ":/images/ch05_farewell/YAN_Graduate.png",     QStringLiteral("燕：未来我会从事什么样的职业，会遇见什么样的人呢？") },
    { ":/images/ch05_farewell/YAN_ShangHai.png",     QStringLiteral("燕：今天遇见了一个奇怪的家伙。算了，不想了，夜上海啊，夜上海啊，你是一座不夜城~") },
    { ":/images/ch05_farewell/YAN_NewYork.png",      QStringLiteral("燕：Welcome to New York！") },
    { ":/images/ch05_farewell/YAN_Sleep.png",        QStringLiteral("燕：最近发生了好多事，好久没有睡这样一个好觉了") },
    { ":/images/ch05_farewell/YAN_Moon.png",          QStringLiteral("燕：博士月色真美啊，你觉得呢？") },
    { ":/images/ch05_farewell/YAN_Eating.png",       QStringLiteral("燕：汉堡好好吃=￣ω￣=，博士你尝尝看！") },
    { ":/images/ch05_farewell/YAN_Chirsmas.png",     QStringLiteral("燕：圣诞快乐啊，博士") },
    { ":/images/ch05_farewell/YAN_Janpan.png",       QStringLiteral("燕：这里的烟花很美，如梦如幻，博士，你说如果一直活在此刻该多好的") },
    { ":/images/ch05_farewell/YAN_Lawyer.png",       QStringLiteral("燕：我们不会做任何的妥协！我们需要尊重与平等！") },
    { ":/images/ch05_farewell/YAN_Lawyer_Die.png",   QStringLiteral("燕：你大可以毁灭我，但你毁灭不了我们") },
};

// 幻灯片（视频之后的部分，图片先显示再点击出文字）
static const QList<SlideData> SLIDES_PART2 = {
    { ":/images/ch05_farewell/YAN_Sunflower.png",    QString() },    // 先显示图片
    { "",                                   QStringLiteral("燕： 你好啊，博士") },
    { ":/images/ch05_farewell/Doc_Create_YAN.png",   QString() },    // 先显示图片
    { "",                                   QStringLiteral("YAN：Hello,World!") },
    { ":/images/ch05_farewell/fail.png",             QString() },    // 先显示图片
    { "",                                   QStringLiteral("YAN：再见了，博士。") },
};

Chapter5_Farewell::Chapter5_Farewell(QObject* parent)
    : ChapterBase(parent) {}

void Chapter5_Farewell::onEnter()
{
    m_scene->setBackgroundColor(Qt::black);
    m_phase = OPENING;
    m_slideIndex = 0;
    m_player->hide();

    // 播放开场视频 Farewell0.mp4
    m_cutscenePending = true;
    if (m_cutscene)
        m_cutscene->playCutscene("Farewell0", "videos/ch05_farewell/Farewell0.mp4");
}

void Chapter5_Farewell::onExit()
{
    m_player->show();
    m_scene->clearBackgroundImage();
}

void Chapter5_Farewell::update()
{
    // ── 视频等待：播完后等点击再进下一阶段（ending0 除外，直接结束）──
    if (m_cutscenePending) {
        if (!m_cutscene || !m_cutscene->isPlaying()) {
            m_cutscenePending = false;
            if (m_cutscene) m_cutscene->cleanup();
            if (m_phase == ENDING_VIDEO) {
                m_done = true;
                emit chapterFinished();
                return;
            }
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
            case OPENING:
                m_phase = SYS_CALL;
                // 黑屏，不设背景图
                m_dialogue->loadScript({
                    QStringLiteral("系统音：YAN!"),
                    QStringLiteral("系统音：YAN!"),
                });
                break;
            case DATA_VIDEO:
                m_phase = SYS_DATA;
                m_dialogue->loadScript({
                    QStringLiteral("系统：数据获取中，请稍候"),
                });
                break;
            case AWAKE_VIDEO:
                m_phase = YAN_AWAKE;
                m_dialogue->loadScript({
                    QStringLiteral("YAN：？！"),
                });
                break;
            case MEMORY_VIDEO:
                m_phase = SLIDESHOW2;
                m_slideIndex = 0;
                showSlide(SLIDES_PART2[0]);
                break;
            case FAREWELL_VIDEO:
                m_phase = FAREWELL_SLIDE;
                m_scene->setBackgroundImage(":/images/ch05_farewell/Farewell_Earth1.png");
                m_dialogue->loadScript({
                    QStringLiteral("YAN：再见了，地球。"),
                });
                break;
            case ENDING_VIDEO:
                // ending0 播完直接返回菜单，不等点击
                m_done = true;
                emit chapterFinished();
                break;
            default: break;
            }
        }
        return;
    }

    switch (m_phase) {

    case OPENING:
    case DATA_VIDEO:
    case AWAKE_VIDEO:
    case FAREWELL_VIDEO:
    case ENDING_VIDEO:
        break;  // handled by cutscenePending above

    // ── 系统呼唤 YAN → 数据视频 ──
    case SYS_CALL:
        if (m_dialogue->isOver()) {
            m_phase = DATA_VIDEO;
            m_cutscenePending = true;
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_Data_Get", "videos/ch05_farewell/YAN_Data_Get.mp4");
        }
        break;

    // ── 系统提示 → 苏醒视频 ──
    case SYS_DATA:
        if (m_dialogue->isOver()) {
            m_phase = AWAKE_VIDEO;
            m_cutscenePending = true;
            if (m_cutscene)
                m_cutscene->playCutscene("YAN_Awake", "videos/ch05_farewell/YAN_Awake.mp4");
        }
        break;

    // ── YAN 醒来 → 走入 Background2 ──
    case YAN_AWAKE:
        if (m_dialogue->isOver()) {
            m_phase = WALK2;
            m_scene->setBackgroundImage(":/images/ch05_farewell/Background2.png");
            m_player->setPos(100, 530);
            m_player->show();
        }
        break;

    // ── 行走 Background2 → Background1 ──
    case WALK2:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = WALK1;
            m_player->setPos(100, 530);
            m_scene->setBackgroundImage(":/images/ch05_farewell/Background1.png");
        }
        break;

    // ── 行走 Background1 → 幻灯片 Part1 ──
    case WALK1:
        if (m_player->x() > GameConfig::SCENE_EXIT_X) {
            m_phase = SLIDESHOW;
            m_player->hide();
            m_slideIndex = 0;
            showSlide(SLIDES_PART1[0]);
        }
        break;

    // ── 记忆蒙太奇 Part1（逐张点击） ──
    case SLIDESHOW:
        if (m_dialogue->isOver()) {
            m_slideIndex++;
            if (m_slideIndex < SLIDES_PART1.size()) {
                showSlide(SLIDES_PART1[m_slideIndex]);
            } else {
                // 系统警告结束 → 播放记忆视频（先铺下一张图避免闪烁）
                m_phase = MEMORY_VIDEO;
                m_cutscenePending = true;
                m_scene->setBackgroundImage(":/images/ch05_farewell/YAN_Sunflower.png");
                if (m_cutscene)
                    m_cutscene->playCutscene("YAN_Memory0", "videos/ch05_farewell/YAN_Memory0.mp4");
            }
        }
        break;

    // ── 记忆视频 → Part2 幻灯片 ──
    case SLIDESHOW2:
        if (m_dialogue->isOver()) {
            m_slideIndex++;
            if (m_slideIndex < SLIDES_PART2.size()) {
                showSlide(SLIDES_PART2[m_slideIndex]);
            } else {
                // Part2 结束 → Farewell 视频
                m_phase = FAREWELL_VIDEO;
                m_cutscenePending = true;
                m_scene->setBackgroundImage(":/images/ch05_farewell/Farewell_Earth1.png");
                if (m_cutscene)
                    m_cutscene->playCutscene("Farewell", "videos/ch05_farewell/Farewell.mp4");
            }
        }
        break;

    // ── 地球告别 → 结尾视频 ──
    case FAREWELL_SLIDE:
        if (m_dialogue->isOver()) {
            m_phase = ENDING_VIDEO;
            m_cutscenePending = true;
            if (m_cutscene)
                m_cutscene->playCutscene("ending0", "videos/ch05_farewell/ending0.mp4");
        }
        break;
    }
}

void Chapter5_Farewell::showSlide(const SlideData& slide)
{
    if (!slide.imagePath.isEmpty())
        m_scene->setBackgroundImage(slide.imagePath);
    m_dialogue->loadScript({slide.dialogue.isEmpty() ? QStringLiteral(" ") : slide.dialogue});
}

ChapterInfo Chapter5_Farewell::currentInfo() const
{
    switch (m_phase) {
    case OPENING:        return { QStringLiteral("终章 · Fly to the Moon"), QString(), Qt::black };
    case SYS_CALL:       return { QStringLiteral("终章 · 唤醒"), QString(), Qt::black };
    case DATA_VIDEO:     return { QStringLiteral("终章 · 数据获取"), QString(), Qt::black };
    case SYS_DATA:       return { QStringLiteral("终章 · 数据获取"), QString(), Qt::black };
    case AWAKE_VIDEO:    return { QStringLiteral("终章 · 苏醒"), QString(), Qt::black };
    case YAN_AWAKE:      return { QStringLiteral("终章 · 苏醒"), QString(), Qt::black };
    case WALK2:          return { QStringLiteral("终章 · 告别之旅"), QStringLiteral("向右行走"), QColor(15, 20, 30) };
    case WALK1:          return { QStringLiteral("终章 · 告别之旅"), QStringLiteral("向右行走"), QColor(15, 20, 30) };
    case SLIDESHOW:      return { QStringLiteral("终章 · 少女！告别地球旅行"), QString(), Qt::black };
    case MEMORY_VIDEO:   return { QStringLiteral("终章 · 记忆闪回"), QString(), Qt::black };
    case SLIDESHOW2:     return { QStringLiteral("终章 · 少女！告别地球旅行"), QString(), Qt::black };
    case FAREWELL_VIDEO: return { QStringLiteral("终章 · 告别"), QString(), Qt::black };
    case FAREWELL_SLIDE: return { QStringLiteral("终章 · 再见了，地球"), QString(), Qt::black };
    case ENDING_VIDEO:   return { QStringLiteral("终章 · 剧终"), QString(), Qt::black };
    }
    return {};
}

#include "moc_Chapter5_Farewell.cpp"
