#pragma once
#include "chapters/ChapterBase.h"

// ╔══════════════════════════════════════════════════════════════╗
// ║  第四章  少女！最后的幸存者                                  ║
// ║  行走→遭遇Angus→视频→认出→闪回→忏悔→失控杀戮→视频→哀悼→启程 ║
// ║  严格按照 Farewell_Trip.doc                                  ║
// ╚══════════════════════════════════════════════════════════════╝

class Chapter4_Survivor : public ChapterBase {
    Q_OBJECT
public:
    explicit Chapter4_Survivor(QObject* parent = nullptr);

    void onEnter() override;
    void onExit() override;
    void update() override;
    bool isComplete() const override { return m_done; }
    ChapterInfo currentInfo() const override;

private:
    enum Phase { WALK_IN, ENCOUNTER, RECOGNIZE, FLASHBACK1, FLASHBACK2, GOODBYE, MOURN, FAREWELL_DOC, DEPARTURE };

    Phase m_phase = WALK_IN;
    bool  m_done  = false;
    bool  m_cutscenePending = false;
    bool  m_videoWait = false;
};
