#pragma once
#include "chapters/ChapterBase.h"

class Enemy;

// ╔══════════════════════════════════════════════════════════╗
// ║  第三章  少女！向天空高举叛逆之拳                          ║
// ║  纽约 → 上海 → 上海政法学院 → 击败前女友执念                ║
// ╚══════════════════════════════════════════════════════════╝

class Chapter3_Rebellion : public ChapterBase {
    Q_OBJECT
public:
    explicit Chapter3_Rebellion(QObject* parent = nullptr);

    void onEnter() override;
    void onExit() override;
    void update() override;
    bool isComplete() const override { return m_done; }
    ChapterInfo currentInfo() const override;

private:
    enum Phase { NEWYORK, SHANGHAI, SUFA_FIGHT, SUFA_DEFEAT_DLG };

    Phase  m_phase = NEWYORK;
    Enemy* m_boss  = nullptr;
    bool   m_done  = false;
    bool   m_bossSpawned = false;
};
