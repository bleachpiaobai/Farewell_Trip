#pragma once
#include "chapters/ChapterBase.h"

// ╔══════════════════════════════════════════════════════════╗
// ║  第一章  少女！从沉睡中苏醒                                ║
// ║  生日回忆 → 系统重启 → 觉醒 → 出发觅食 → 苏醒视频          ║
// ║  四张图片 + 对话 + 视频，带淡入淡出过渡                     ║
// ╚══════════════════════════════════════════════════════════╝

class Chapter1_Awaken : public ChapterBase {
    Q_OBJECT
public:
    explicit Chapter1_Awaken(QObject* parent = nullptr);

    void onEnter() override;
    void onExit() override;
    void update() override;
    bool isComplete() const override { return m_done; }
    ChapterInfo currentInfo() const override;

private:
    enum Phase { BIRTHDAY, REBOOT, AWAKEN, HUNGRY, VIDEO_AWAKE, DONE };
    void switchToPhase(Phase phase);
    void startTransitionTo(Phase nextPhase);

    Phase m_phase = BIRTHDAY;
    bool  m_done  = false;
    bool  m_transitioning = false;
    bool  m_cutscenePending = false;
    bool  m_videoWait = false;
};
