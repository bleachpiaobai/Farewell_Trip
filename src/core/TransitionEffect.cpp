#include "TransitionEffect.h"
#include "ui/GameScene.h"

TransitionEffect::TransitionEffect(GameScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
    , m_timer(new QTimer(this))
{
    m_timer->setTimerType(Qt::PreciseTimer);
    m_timer->setInterval(GameConfig::FRAME_INTERVAL_MS);
    connect(m_timer, &QTimer::timeout, this, &TransitionEffect::onTick);
}

void TransitionEffect::fadeOut(const QColor& toColor, int durationMs)
{
    m_fadingOut = true;
    m_targetColor = toColor;
    m_durationMs = durationMs;
    m_elapsed = 0;
    m_progress = 0.0;
    m_active = true;
    m_timer->start();
}

void TransitionEffect::fadeIn(int durationMs)
{
    m_fadingOut = false;
    m_durationMs = durationMs;
    m_elapsed = 0;
    m_progress = 1.0;
    m_active = true;
    m_timer->start();
}

void TransitionEffect::onTick()
{
    if (!m_active) return;

    m_elapsed += GameConfig::FRAME_INTERVAL_MS;
    m_progress = static_cast<qreal>(m_elapsed) / m_durationMs;

    if (m_fadingOut)
        m_progress = qMin(m_progress, 1.0);
    else
        m_progress = qMax(1.0 - m_progress, 0.0);

    // Apply a dark overlay on top of the scene
    // (simple approach: add a semi-transparent rect at highest Z)
    // In a full implementation, this would use a dedicated overlay item.

    if (m_elapsed >= m_durationMs) {
        m_active = false;
        m_timer->stop();
        if (m_fadingOut)
            emit fadeOutFinished();
        else
            emit fadeInFinished();
    }
}
