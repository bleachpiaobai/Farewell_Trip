#pragma once
#include "chapters/ChapterBase.h"

class Enemy;

// ╔══════════════════════════════════════════════════════════╗
// ║  第二章  少女！决战梨花怪                                  ║
// ║  穿越静谧森林 → 十里桃林 → 击败桃子怪                       ║
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
    enum Phase { FOREST, PEACH_FIGHT, PEACH_DEFEAT_DLG };

    Phase  m_phase = FOREST;
    Enemy* m_boss  = nullptr;
    bool   m_done  = false;
    bool   m_bossSpawned = false;
};
