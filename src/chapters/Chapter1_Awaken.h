#pragma once
#include "chapters/ChapterBase.h"

// ╔══════════════════════════════════════════════════════════╗
// ║  第一章  少女！从沉睡中苏醒                                ║
// ║  YAN 在废弃仓库密室中苏醒，接收神秘指令，踏上旅途            ║
// ║  三张图片 + 对话，带淡入淡出过渡                            ║
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
    enum Phase { IMAGE_1, IMAGE_2, IMAGE_3, DONE };
    void switchToPhase(Phase phase);
    void startTransitionTo(Phase nextPhase);

    Phase m_phase = IMAGE_1;
    bool  m_done  = false;
    bool  m_transitioning = false;
};
