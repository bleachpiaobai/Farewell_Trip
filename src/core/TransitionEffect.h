#ifndef TRANSITIONEFFECT_H
#define TRANSITIONEFFECT_H

#include <QObject>
#include <QTimer>
#include <QColor>
#include "GameConfig.h"

class GameScene;

class TransitionEffect : public QObject
{
    Q_OBJECT
public:
    explicit TransitionEffect(GameScene* scene, QObject* parent = nullptr);

    void fadeOut(const QColor& toColor = Qt::black, int durationMs = GameConfig::TRANSITION_DURATION_MS);
    void fadeIn(int durationMs = GameConfig::TRANSITION_DURATION_MS);
    bool isActive() const { return m_active; }

public slots:
    void onTick();

signals:
    void fadeOutFinished();
    void fadeInFinished();

private:
    GameScene* m_scene;
    QTimer* m_timer;
    bool m_active = false;
    bool m_fadingOut = true;
    QColor m_targetColor;
    int m_durationMs;
    int m_elapsed = 0;
    qreal m_progress = 0.0;
};

#endif // TRANSITIONEFFECT_H
