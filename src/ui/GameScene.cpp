#include "GameScene.h"
#include "core/GameConfig.h"

#include <QGraphicsRectItem>
#include <QPen>

GameScene::GameScene(QObject* parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

    // Default black background
    m_background = addRect(sceneRect(), Qt::NoPen, Qt::black);
    m_background->setZValue(-1000);
}

void GameScene::setBackgroundColor(const QColor& color)
{
    m_background->setBrush(color);
}

void GameScene::clearScene()
{
    // Remove everything except the background rect
    QList<QGraphicsItem*> items = this->items();
    for (auto* item : items) {
        if (item != m_background)
            removeItem(item);
    }
}
