#include "TransitionEffect.h"
#include "ui/GameScene.h"
#include <QGraphicsRectItem>
#include <QDebug>

TransitionEffect::TransitionEffect(GameScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
{
    // Create full-screen black overlay at highest Z
    m_overlay = new QGraphicsRectItem();
    m_overlay->setRect(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    m_overlay->setBrush(Qt::black);
    m_overlay->setZValue(10000);
    m_overlay->setOpacity(0.0);
    m_overlay->setVisible(false);
    m_scene->addItem(m_overlay);
}

TransitionEffect::~TransitionEffect()
{
    // m_overlay is owned by the scene, but we remove it
    if (m_overlay && m_overlay->scene()) {
        m_overlay->scene()->removeItem(m_overlay);
    }
    delete m_overlay;
}

void TransitionEffect::fadeOut(const QColor& toColor, int durationMs)
{
    m_phase = 1;
    m_targetColor = toColor;
    m_durationMs = durationMs;
    m_elapsed = 0;
    m_active = true;

    m_overlay->setBrush(toColor);
    m_overlay->setOpacity(0.0);
    m_overlay->setVisible(true);
}

void TransitionEffect::fadeIn(int durationMs)
{
    m_phase = 3;
    m_durationMs = durationMs;
    m_elapsed = 0;
    m_active = true;

    m_overlay->setOpacity(1.0);
    m_overlay->setVisible(true);
}

void TransitionEffect::crossFade(std::function<void()> midCallback, int durationMs)
{
    m_midCallback = std::move(midCallback);
    fadeOut(Qt::black, durationMs);
}

void TransitionEffect::reset()
{
    m_active = false;
    m_phase = 0;
    m_elapsed = 0;
    m_midCallback = nullptr;
    if (m_overlay) {
        m_overlay->setOpacity(0.0);
        m_overlay->setVisible(false);
    }
}

void TransitionEffect::onTick()
{
    if (!m_active) return;

    m_elapsed += GameConfig::FRAME_INTERVAL_MS;
    qreal progress = static_cast<qreal>(m_elapsed) / m_durationMs;

    switch (m_phase) {

    case 1: // fadeOut: 0 → 1
        progress = qMin(progress, 1.0);
        m_overlay->setOpacity(progress);

        if (m_elapsed >= m_durationMs) {
            // Fully dark — invoke midpoint callback if crossFade
            if (m_midCallback) {
                m_midCallback();
                m_midCallback = nullptr;
                // Start fade in
                m_phase = 3;
                m_elapsed = 0;
                m_overlay->setOpacity(1.0);
            } else {
                m_active = false;
                emit fadeOutFinished();
            }
        }
        break;

    case 3: // fadeIn: 1 → 0
        m_overlay->setOpacity(qMax(1.0 - progress, 0.0));

        if (m_elapsed >= m_durationMs) {
            m_active = false;
            m_overlay->setOpacity(0.0);
            m_overlay->setVisible(false);
            emit fadeInFinished();
        }
        break;
    }
}
