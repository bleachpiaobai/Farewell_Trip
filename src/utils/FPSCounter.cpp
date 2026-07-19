#include "FPSCounter.h"

#include <QFont>

FPSCounter::FPSCounter(QGraphicsItem* parent)
    : QGraphicsTextItem(parent)
{
    setDefaultTextColor(QColor(0, 255, 0));
    setFont(QFont("Consolas", 14));
    setZValue(1000);
    setPlainText("FPS: --");
    m_timer.start();
}

void FPSCounter::tick()
{
    m_frameCount++;
    qint64 elapsed = m_timer.elapsed();
    if (elapsed >= 1000) {
        m_lastFPS = m_frameCount * 1000 / static_cast<int>(elapsed);
        m_frameCount = 0;
        m_timer.restart();
        setPlainText(QString("FPS: %1").arg(m_lastFPS));
    }
}
