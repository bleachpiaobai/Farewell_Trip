#pragma once
#include "chapters/ChapterBase.h"

class Enemy;

// ╔══════════════════════════════════════════════════════════════╗
// ║  第四章  少女！最后的幸存者                                  ║
// ║  遭遇Angus → BOSS战 → 认出 → 闪回 → 忏悔 → 告别 → 哀悼 → 启程║
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
    enum Phase { ENCOUNTER, RECOGNIZE, FLASHBACK, CONFESSION, GOODBYE, MOURN, DEPARTURE };

    Phase  m_phase = ENCOUNTER;
    Enemy* m_boss  = nullptr;
    bool   m_done  = false;
    bool   m_cutscenePending = false;
};
