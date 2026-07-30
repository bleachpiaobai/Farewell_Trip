#pragma once
#include "chapters/ChapterBase.h"

class Enemy;

// ╔══════════════════════════════════════════════════════════╗
// ║  第三章  少女！向天空高举叛逆之拳                          ║
// ║  废土深处 → 遭遇XIA → 战斗视频 → BOSS战 → 悔恨 → 决心     ║
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
    enum Phase { WASTELAND_DEEP, XIA_ENCOUNTER, XIA_VIDEO, XIA_FIGHT,
                 XIA_DEFEAT1, XIA_DEFEAT2, YAN_RESOLVE };

    Phase  m_phase = WASTELAND_DEEP;
    Enemy* m_boss  = nullptr;
    bool   m_done  = false;
    bool   m_bossSpawned = false;
    bool   m_cutscenePending = false;
};
