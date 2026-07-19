#pragma once
#include "chapters/ChapterBase.h"

// ╔══════════════════════════════════════════════════════════╗
// ║  第一章  少女！从沉睡中苏醒                                ║
// ║  YAN 在废弃仓库密室中苏醒，接收神秘指令，踏上旅途            ║
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
    bool m_done = false;
};
