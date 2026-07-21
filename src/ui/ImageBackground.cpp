#include "ImageBackground.h"
#include "core/GameConfig.h"
#include <QPainter>
#include <QDebug>

ImageBackground::ImageBackground(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    setZValue(-500);  // above solid color rect (-1000), below entities
    setOpacity(1.0);

    m_fadeAnim = new QPropertyAnimation(this, "opacity", this);
    connect(m_fadeAnim, &QPropertyAnimation::finished, this, &ImageBackground::fadeFinished);
}

QRectF ImageBackground::boundingRect() const
{
    return QRectF(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
}

void ImageBackground::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (m_pixmap.isNull()) return;

    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    QRectF target(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    painter->drawPixmap(target, m_pixmap, m_pixmap.rect());
}

void ImageBackground::setImage(const QString& qrcPath)
{
    QPixmap pix(qrcPath);
    if (pix.isNull()) {
        qWarning() << "ImageBackground: failed to load" << qrcPath;
        return;
    }
    setImage(pix);
}

void ImageBackground::setImage(const QPixmap& pixmap)
{
    m_pixmap = pixmap.scaled(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT,
                             Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    // Center-crop to exact scene size
    if (m_pixmap.width() > GameConfig::WINDOW_WIDTH ||
        m_pixmap.height() > GameConfig::WINDOW_HEIGHT) {
        int x = (m_pixmap.width() - GameConfig::WINDOW_WIDTH) / 2;
        int y = (m_pixmap.height() - GameConfig::WINDOW_HEIGHT) / 2;
        m_pixmap = m_pixmap.copy(x, y, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    }
    update();
}

void ImageBackground::clear()
{
    m_pixmap = QPixmap();
    update();
}

void ImageBackground::fadeTo(qreal targetOpacity, int durationMs)
{
    m_fadeAnim->stop();
    m_fadeAnim->setDuration(durationMs);
    m_fadeAnim->setStartValue(opacity());
    m_fadeAnim->setEndValue(targetOpacity);
    m_fadeAnim->setEasingCurve(QEasingCurve::InOutCubic);
    m_fadeAnim->start();
}
