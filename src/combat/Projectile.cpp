#include "Projectile.h"
#include "core/GameConfig.h"
#include <QtMath>

Projectile::Projectile(const QColor& color, qreal radius, int damage,
                       qreal vx, qreal vy, qreal gravity, int lifetime,
                       QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_color(color)
    , m_radius(radius)
    , m_damage(damage)
    , m_vx(vx)
    , m_vy(vy)
    , m_gravity(gravity)
    , m_lifetime(lifetime)
{
    setZValue(15);
}

QRectF Projectile::boundingRect() const
{
    // 碰撞判定仅用实体半径，不含发光余量
    return QRectF(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
}

QPainterPath Projectile::shape() const
{
    QPainterPath path;
    path.addEllipse(QPointF(0, 0), m_radius, m_radius);
    return path;
}

void Projectile::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    qreal drawR = m_radius + GLOW;

    // 拖尾
    for (int i = 0; i < m_trail.size(); i++) {
        qreal alpha = 0.12 * (i + 1) / m_trail.size();
        qreal r = m_radius * 0.5 * (i + 1) / m_trail.size();
        QPointF localPos = mapFromScene(m_trail[i]);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(m_color.red(), m_color.green(), m_color.blue(),
                                 static_cast<int>(alpha * 255)));
        painter->drawEllipse(localPos, r, r);
    }

    // 外发光
    QColor glowColor(m_color.red(), m_color.green(), m_color.blue(), 70);
    painter->setPen(Qt::NoPen);
    painter->setBrush(glowColor);
    painter->drawEllipse(QPointF(0, 0), drawR, drawR);

    // 主体
    painter->setBrush(m_color);
    painter->drawEllipse(QPointF(0, 0), m_radius, m_radius);

    // 高光
    QColor highlight(255, 255, 255, 130);
    painter->setBrush(highlight);
    painter->drawEllipse(QPointF(-m_radius * 0.3, -m_radius * 0.3),
                         m_radius * 0.35, m_radius * 0.35);
}

void Projectile::tick()
{
    if (m_expired) return;

    m_trail.prepend(scenePos());
    while (m_trail.size() > MAX_TRAIL)
        m_trail.removeLast();

    setPos(x() + m_vx, y() + m_vy);

    if (m_gravity > 0.0)
        m_vy += m_gravity;

    if (m_lifetime > 0) {
        m_lifetime--;
        if (m_lifetime <= 0) {
            m_expired = true;
            return;
        }
    }

    // 弹跳模式：碰到屏幕边缘反弹，不消失
    if (m_bouncing) {
        if (x() < 50.0 || x() > GameConfig::WINDOW_WIDTH - 50.0) {
            m_vx = -m_vx;
            setPos(x() < 50.0 ? 50.0 : GameConfig::WINDOW_WIDTH - 50.0, y());
        }
    } else {
        if (x() < -100 || x() > GameConfig::WINDOW_WIDTH + 100 ||
            y() < -200 || y() > GameConfig::WINDOW_HEIGHT + 100) {
            m_expired = true;
        }
    }
}
