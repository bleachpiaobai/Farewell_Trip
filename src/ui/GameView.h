#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView(QGraphicsScene* scene, QWidget* parent = nullptr);

signals:
    void gameKeyPressed(int key);
    void gameKeyReleased(int key);
    void gameMousePressed(Qt::MouseButton button, const QPointF& pos);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};

#endif // GAMEVIEW_H
