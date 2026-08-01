#pragma once
#include "chapters/ChapterBase.h"

class Enemy;

// ╔══════════════════════════════════════════════════════════╗
// ║  第二章  少女！决战桃子怪                                  ║
// ║  荒漠行走 → YAN出现 → 宣战 → BOSS战 → 击败 → 震惊 →       ║
// ║  哭泣 → 叹息 → 老师模式 → 百年感慨 →                       ║
// ║  图片轮播(发射中心→列车1→列车2→发射中心) → 告别视频 → 结束  ║
// ║  严格按照 Farewell_Trip.doc                               ║
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
    enum Phase { WASTELAND, YAN_APPEAR, PEACH_FIGHT_DECLARE, PEACH_FIGHT,
                 PEACH_DEFEAT, PEACH_SHOCK, PEACH_CRY, PEACH_SIGH,
                 PEACH_TEACHER, PEACH_OLD,
                 PEACH_TRAIN_SHOOT1, PEACH_TRAIN_TRAIN1,
                 PEACH_TRAIN_TRAIN2, PEACH_TRAIN_SHOOT2,
                 PEACH_VIDEO, PEACH_GOODBYE };

    Phase  m_phase = WASTELAND;
    Enemy* m_boss  = nullptr;
    bool   m_done  = false;
    bool   m_bossSpawned = false;
    bool   m_cutscenePending = false;
    bool   m_videoWait = false;
};
