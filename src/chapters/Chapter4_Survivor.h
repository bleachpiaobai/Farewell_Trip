#pragma once
#include "chapters/ChapterBase.h"

class Enemy;

// ╔══════════════════════════════════════════════════════════╗
// ║  第四章  少女！最后的幸存者                                ║
// ║  南京 → 悬空之桥 → 空白终境 → 击败渣男执念                  ║
// ╚══════════════════════════════════════════════════════════╝

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
    enum Phase { NANJING, AIRBRIDGE, EMPTY_FIGHT, MAN_DEFEAT_DLG, END_DLG };

    Phase  m_phase = NANJING;
    Enemy* m_boss  = nullptr;
    bool   m_done  = false;
    bool   m_bossSpawned = false;
};
