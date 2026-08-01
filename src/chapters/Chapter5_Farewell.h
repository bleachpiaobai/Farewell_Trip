#pragma once
#include "chapters/ChapterBase.h"
#include <QStringList>
#include <QList>

// ╔══════════════════════════════════════════════════════════════╗
// ║  第五章  少女！告别地球旅行                                  ║
// ║  Farewell0→对话→Data视频→对话→Awake视频→对话→行走→幻灯片→   ║
// ║  Farewell视频→地球告别→ending0→菜单                          ║
// ╚══════════════════════════════════════════════════════════════╝

struct SlideData {
    QString imagePath;
    QString dialogue;
};

class Chapter5_Farewell : public ChapterBase {
    Q_OBJECT
public:
    explicit Chapter5_Farewell(QObject* parent = nullptr);

    void onEnter() override;
    void onExit() override;
    void update() override;
    bool isComplete() const override { return m_done; }
    ChapterInfo currentInfo() const override;

private:
    void showSlide(const SlideData& slide);

    enum Phase { OPENING, SYS_CALL, DATA_VIDEO, SYS_DATA,
                 AWAKE_VIDEO, YAN_AWAKE, WALK2, WALK1,
                 SLIDESHOW, MEMORY_VIDEO, SLIDESHOW2,
                 FAREWELL_VIDEO, FAREWELL_SLIDE, ENDING_VIDEO };
    Phase m_phase = OPENING;
    int m_slideIndex = 0;
    bool m_cutscenePending = false;
    bool m_videoWait = false;
    bool m_done = false;
};
