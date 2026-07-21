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
};

#endif // IMAGEBACKGROUND_H
