#pragma once
#include "chapters/ChapterBase.h"
#include <QStringList>
#include <QList>

// ╔══════════════════════════════════════════════════════════════╗
// ║  第五章  少女！银河告别旅行                                  ║
// ║  开场视频 → 记忆蒙太奇 → 地球告别循环 → 结尾视频 → 结束       ║
// ╚══════════════════════════════════════════════════════════════╝

struct SlideData {
    QString imagePath;
    QString dialogue;     // 空字符串 = 自动跳过
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
    void showMemorySlide(int index);
    void advanceSlide();

    enum Phase { OPENING, SLIDESHOW, EARTH_FAREWELL, ENDING_VIDEO };
    Phase m_phase = OPENING;
    int m_slideIndex = 0;
    int m_earthCycleCount = 0;  // track how many times we've cycled
    bool m_cutscenePending = false;
    bool m_autoSkip = false;
    bool m_done = false;
};
