#include "GameView.h"
#include "core/GameConfig.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QOpenGLWidget>
#include <QSurfaceFormat>

GameView::GameView(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    // ── OpenGL viewport for GPU-accelerated rendering ──
    // This prevents video lag when the window is scaled up (fullscreen).
    auto* glWidget = new QOpenGLWidget();
    QSurfaceFormat fmt;
    fmt.setSwapInterval(1);           // VSync on — smooth video
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    glWidget->setFormat(fmt);
    // CRITICAL: QOpenGLWidget defaults to Qt::NoFocus, which prevents it from
    // accepting focus forwarded by QGraphicsView::focusInEvent. Without focus
    // on the viewport, key events never reach GameView::keyPressEvent.
    glWidget->setFocusPolicy(Qt::StrongFocus);
    setViewport(glWidget);

    // Set logical scene size; the view will scale to fit
    setSceneRect(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

    // Camera following (no-op when scene == viewport)
    updateViewTransform();
    setBackgroundBrush(Qt::black);

    // Let the view resize freely — content scales via fitInView
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
    updateViewTransform();
}

void GameView::setCamera(qreal playerX, qreal sceneWidth)
{
    m_cameraX = playerX;
    m_sceneWidth = sceneWidth;
    updateViewTransform();
}

void GameView::updateViewTransform()
{
    // Camera: center on player with ~40% offset from left, clamped to scene bounds
    qreal maxCamLeft = qMax(0.0, m_sceneWidth - GameConfig::WINDOW_WIDTH);
    qreal camLeft = m_cameraX - GameConfig::WINDOW_WIDTH * 0.4;
    camLeft = qBound(0.0, camLeft, maxCamLeft);

    QRectF visibleRect(camLeft, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    fitInView(visibleRect, Qt::KeepAspectRatio);
}
