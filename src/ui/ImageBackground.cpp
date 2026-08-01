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
    return QRectF(0, 0, m_sceneW, m_sceneH);
}

void ImageBackground::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (m_pixmap.isNull()) return;

    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    QRectF target(0, 0, m_sceneW, m_sceneH);
    painter->drawPixmap(target, m_pixmap, m_pixmap.rect());
}

void ImageBackground::setSceneSize(qreal w, qreal h)
{
    m_sceneW = w;
    m_sceneH = h;
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
    m_pixmap = pixmap.scaled(static_cast<int>(m_sceneW), static_cast<int>(m_sceneH),
                             Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    // Center-crop to exact scene size
    if (m_pixmap.width() > m_sceneW || m_pixmap.height() > m_sceneH) {
        int x = static_cast<int>((m_pixmap.width() - m_sceneW) / 2);
        int y = static_cast<int>((m_pixmap.height() - m_sceneH) / 2);
        m_pixmap = m_pixmap.copy(x, y, static_cast<int>(m_sceneW), static_cast<int>(m_sceneH));
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
