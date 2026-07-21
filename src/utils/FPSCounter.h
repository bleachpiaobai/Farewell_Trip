#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include <QGraphicsTextItem>
#include <QElapsedTimer>

class FPSCounter : public QGraphicsTextItem
{
public:
    explicit FPSCounter(QGraphicsItem* parent = nullptr);

    void tick();   // call once per frame

private:
    QElapsedTimer m_timer;
    int m_frameCount = 0;
    int m_lastFPS = 0;
};

#endif // FPSCOUNTER_H
