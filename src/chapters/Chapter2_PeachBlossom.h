#pragma once
#include "chapters/ChapterBase.h"

class Enemy;

// ╔══════════════════════════════════════════════════════════╗
// ║  第二章  少女！决战桃子怪                                  ║
// ║  荒漠 → 桃林遭遇 → BOSS战 → 震惊 → 哭泣 → 叹息 →          ║
// ║  战争历史 → 百年感慨 → 列车指引 → 告别视频 → 结束           ║
// ╚══════════════════════════════════════════════════════════╝

class Chapter2_PeachBlossom : public ChapterBase {
    Q_OBJECT
public:
    explicit Chapter2_PeachBlossom(QObject* parent = nullptr);

    void onEnter() override;
    void onExit() override;
    void update() override;
    bool isComplete() const override { return m_done; }
    ChapterInfo currentInfo() const override;

private:
    enum Phase { WASTELAND, PEACH_ENCOUNTER, PEACH_FIGHT,
                 PEACH_SHOCK, PEACH_CRY, PEACH_SIGH,
                 PEACH_TEACHER, PEACH_OLD, PEACH_TRAIN,
                 PEACH_VIDEO, PEACH_GOODBYE };

    Phase  m_phase = WASTELAND;
    Enemy* m_boss  = nullptr;
    bool   m_done  = false;
    bool   m_bossSpawned = false;
    bool   m_cutscenePending = false;
};
