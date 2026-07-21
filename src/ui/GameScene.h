#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>

class ImageBackground;

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject* parent = nullptr);

    /// Set a solid-color background by filling the scene rect.
    void setBackgroundColor(const QColor& color);

    /// Show a full-screen background image (scaled to fit).
    void setBackgroundImage(const QString& qrcPath);

    /// Remove background image, reverting to solid color.
    void clearBackgroundImage();

    /// Access the image background for fade control.
    ImageBackground* imageBg() const { return m_imageBg; }

    /// Convenience: clear all items and reset background.
    void clearScene();

private:
    QGraphicsRectItem* m_background = nullptr;
    ImageBackground*   m_imageBg   = nullptr;
};

#endif // GAMESCENE_H
