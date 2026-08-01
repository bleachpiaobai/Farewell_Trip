#pragma once
#include "chapters/ChapterBase.h"

class Enemy;

// ╔══════════════════════════════════════════════════════════╗
// ║  第三章  少女！向天空高举叛逆之拳                          ║
// ║  废土→XIA_Stand→视频→Boss战→Die1→Die2→Finger→Fist→葵    ║
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
                 XIA_DEFEAT1, XIA_DEFEAT2, XIA_FINGER, XIA_FIST, YAN_RESOLVE };

    Phase  m_phase = WASTELAND_DEEP;
    Enemy* m_boss  = nullptr;
    bool   m_done  = false;
    bool   m_bossSpawned = false;
    bool   m_cutscenePending = false;
    bool   m_videoWait = false;
    bool   m_resolveStarted = false;
};
