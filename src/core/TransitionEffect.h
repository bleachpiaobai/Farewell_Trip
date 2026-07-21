#ifndef TRANSITIONEFFECT_H
#define TRANSITIONEFFECT_H

#include <QObject>
#include <QColor>
#include "GameConfig.h"

class QGraphicsRectItem;
class GameScene;

// ── Full-screen fade transition effect ─────────────────

class TransitionEffect : public QObject
{
    Q_OBJECT
public:
    explicit TransitionEffect(GameScene* scene, QObject* parent = nullptr);
    ~TransitionEffect();

    /// Fade scene to black (or custom color)
    void fadeOut(const QColor& toColor = Qt::black,
                 int durationMs = GameConfig::TRANSITION_FADE_MS);

    /// Fade black overlay away to reveal scene
    void fadeIn(int durationMs = GameConfig::TRANSITION_FADE_MS);

    /// Convenience: fade out → callback → fade in
    /// The callback is invoked when fully dark (midpoint).
    void crossFade(std::function<void()> midCallback,
                   int durationMs = GameConfig::TRANSITION_FADE_MS);

    bool isActive() const { return m_active; }

    /// Immediately hide overlay (emergency reset)
    void reset();

public slots:
    void onTick();  // Called each frame by GameEngine

signals:
    void fadeOutFinished();
    void fadeInFinished();

private:
    GameScene* m_scene;
    QGraphicsRectItem* m_overlay = nullptr;

    bool m_active = false;
    int  m_phase = 0;     // 0=idle, 1=fadeOut, 2=waiting, 3=fadeIn
    QColor m_targetColor;
    int m_durationMs = 0;
    int m_elapsed = 0;

    std::function<void()> m_midCallback;
};

#endif // TRANSITIONEFFECT_H
