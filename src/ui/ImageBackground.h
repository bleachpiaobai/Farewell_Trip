#ifndef IMAGEBACKGROUND_H
#define IMAGEBACKGROUND_H

#include <QGraphicsObject>
#include <QPropertyAnimation>

// ── Full-screen background image with fade support ─────

class ImageBackground : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ImageBackground(QGraphicsItem* parent = nullptr);

    void setImage(const QString& qrcPath);
    void setImage(const QPixmap& pixmap);
    void clear();

    /// Set the logical scene size (for wide scrolling backgrounds)
    void setSceneSize(qreal w, qreal h);

    /// Animate opacity to target over durationMs
    void fadeTo(qreal targetOpacity, int durationMs);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void fadeFinished();

private:
    QPixmap m_pixmap;
    QPropertyAnimation* m_fadeAnim = nullptr;
    qreal m_sceneW = 1280;
    qreal m_sceneH = 720;
};

#endif // IMAGEBACKGROUND_H
