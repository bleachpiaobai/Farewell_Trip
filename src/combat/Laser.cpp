#include "Laser.h"

Laser::Laser(qreal x, qreal yTop, qreal height, qreal width,
             const QColor& color,
             int telegraphFrames, int activeFrames, int damage,
             QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_color(color)
    , m_laserWidth(width)
    , m_laserHeight(height)
    , m_damage(damage)
    , m_telegraphFrames(telegraphFrames)
    , m_activeFrames(activeFrames)
{
    setPos(x, yTop);
    setZValue(16);  // above everything
}

QRectF Laser::boundingRect() const
{
    return QRectF(0, 0, m_laserWidth, m_laserHeight);
}

QPainterPath Laser::shape() const
{
    QPainterPath path;
    // 激活时碰撞体积稍窄（给玩家一点容错）
    qreal margin = (m_phase == ACTIVE) ? 4.0 : 0.0;
    path.addRect(margin, 0, m_laserWidth - margin * 2, m_laserHeight);
    return path;
}

void Laser::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QColor drawColor = m_color;
    qreal alpha;

    switch (m_phase) {
    case TELEGRAPH: {
        // 预警：闪烁半透明
        alpha = 0.25 + 0.15 * ((m_timer / 5) % 2);  // blink
        drawColor.setAlphaF(alpha);
        painter->setPen(Qt::NoPen);
        painter->setBrush(drawColor);
        painter->drawRect(QRectF(0, 0, m_laserWidth, m_laserHeight));

        // 边缘发光线
        QColor edgeColor(m_color.red(), m_color.green(), m_color.blue(), 60);
        painter->setPen(QPen(edgeColor, 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawLine(QPointF(m_laserWidth / 2, 0), QPointF(m_laserWidth / 2, m_laserHeight));
        break;
    }
    case ACTIVE: {
        // 激活：亮色核心 + 外发光
        // 外发光
        QColor glowColor(m_color.red(), m_color.green(), m_color.blue(), 100);
        painter->setPen(Qt::NoPen);
        painter->setBrush(glowColor);
        painter->drawRect(QRectF(-4, 0, m_laserWidth + 8, m_laserHeight));

        // 主体
        drawColor.setAlphaF(0.85);
        painter->setBrush(drawColor);
        painter->drawRect(QRectF(0, 0, m_laserWidth, m_laserHeight));

        // 白色核心线
        QColor core(220, 230, 255, 200);
        painter->setPen(QPen(core, 3));
        painter->drawLine(QPointF(m_laserWidth / 2, 4), QPointF(m_laserWidth / 2, m_laserHeight - 4));
        break;
    }
    case DONE:
        break;
    }
}

void Laser::tick()
{
    if (m_expired) return;

    m_timer++;

    switch (m_phase) {
    case TELEGRAPH:
        if (m_timer >= m_telegraphFrames) {
            m_phase = ACTIVE;
            m_timer = 0;
        }
        update();  // blink effect
        break;
    case ACTIVE:
        if (m_timer >= m_activeFrames) {
            m_phase = DONE;
            m_expired = true;
        }
        break;
    case DONE:
        m_expired = true;
        break;
    }
}
