#include "GameScene.h"
#include "ImageBackground.h"
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

    // Background image layer (above solid color, below entities)
    m_imageBg = new ImageBackground();
    m_imageBg->setZValue(-500);
    addItem(m_imageBg);
}

void GameScene::setBackgroundColor(const QColor& color)
{
    m_background->setBrush(color);
}

void GameScene::setBackgroundImage(const QString& qrcPath)
{
    if (m_imageBg)
        m_imageBg->setImage(qrcPath);
}

void GameScene::clearBackgroundImage()
{
    if (m_imageBg)
        m_imageBg->clear();
}

void GameScene::clearScene()
{
    // Remove everything except the background rect and image bg
    QList<QGraphicsItem*> items = this->items();
    for (auto* item : items) {
        if (item != m_background && item != m_imageBg)
            removeItem(item);
    }
}
