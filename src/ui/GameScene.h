#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject* parent = nullptr);

    /// Set a solid-color background by filling the scene rect.
    void setBackgroundColor(const QColor& color);

    /// Convenience: clear all items and reset background.
    void clearScene();

private:
    QGraphicsRectItem* m_background = nullptr;
};

#endif // GAMESCENE_H
