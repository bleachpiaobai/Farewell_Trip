#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include "core/GameConfig.h"

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView(QGraphicsScene* scene, QWidget* parent = nullptr);

    /// Set the camera target X (player position); sceneWidth may differ per chapter.
    void setCamera(qreal playerX, qreal sceneWidth);

signals:
    void gameKeyPressed(int key);
    void gameKeyReleased(int key);
    void gameMousePressed(Qt::MouseButton button, const QPointF& pos);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateViewTransform();
    qreal m_cameraX = GameConfig::WINDOW_WIDTH / 2.0;
    qreal m_sceneWidth = GameConfig::WINDOW_WIDTH;
};

#endif // GAMEVIEW_H
