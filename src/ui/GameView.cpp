#include "GameView.h"
#include "core/GameConfig.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>

GameView::GameView(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    // Fixed logical size
    setFixedSize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    setSceneRect(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

    // Fit the scene into the view
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    setBackgroundBrush(Qt::black);

    // Enable key tracking
    setFocusPolicy(Qt::StrongFocus);
}

void GameView::keyPressEvent(QKeyEvent* event)
{
    emit gameKeyPressed(event->key());
    // Don't call base — we handle everything ourselves
}

void GameView::keyReleaseEvent(QKeyEvent* event)
{
    emit gameKeyReleased(event->key());
}

void GameView::mousePressEvent(QMouseEvent* event)
{
    emit gameMousePressed(event->button(), event->pos());
}

void GameView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}
