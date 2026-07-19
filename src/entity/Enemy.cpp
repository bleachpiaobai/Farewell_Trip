#include "Enemy.h"
#include "utils/MathUtils.h"

Enemy::Enemy(const QString& name, int maxHp, const QColor& bodyColor,
             QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_name(name)
    , m_maxHp(maxHp)
    , m_bodyColor(bodyColor)
{
    m_hp = m_maxHp;
    setZValue(9);
}

QRectF Enemy::boundingRect() const
{
    // Include name label above and HP bar
    return QRectF(-5, -50, W + 10, H + 55);
}

QPainterPath Enemy::shape() const
{
    QPainterPath path;
    path.addEllipse(QRectF(0, 0, W, H));
    return path;
}

void Enemy::setHp(int hp)
{
    m_hp = MathUtils::clamp(hp, 0, m_maxHp);
    emit hpChanged(m_name, m_hp, m_maxHp);
    update();  // trigger repaint so HP bar reflects the change
    if (m_hp <= 0 && !m_dead) {
        m_dead = true;
        emit defeated(m_name);
    }
}

void Enemy::takeDamage(int dmg)
{
    setHp(m_hp - dmg);
}

void Enemy::kill()
{
    setHp(0);
}

void Enemy::reset()
{
    m_hp = m_maxHp;
    m_dead = false;
}

// ── Drawing ──────────────────────────────────────────────

void Enemy::drawBoss(QPainter* p)
{
    if (m_dead) return;

    p->save();
    p->setPen(Qt::NoPen);
    p->setRenderHint(QPainter::Antialiasing);

    // Body
    p->setBrush(m_bodyColor);
    p->drawEllipse(QRectF(5, 5, W - 10, H - 10));

    // Lighter inner glow
    p->setBrush(m_bodyColor.lighter(140));
    p->drawEllipse(QRectF(20, 20, W - 40, H - 40));

    // Eyes
    p->setBrush(Qt::black);
    p->drawEllipse(QRectF(25, 25, 8, 8));
    p->drawEllipse(QRectF(47, 25, 8, 8));

    // Eye highlights
    p->setBrush(Qt::white);
    p->drawEllipse(QRectF(27, 27, 3, 3));
    p->drawEllipse(QRectF(49, 27, 3, 3));

    p->restore();
}

void Enemy::drawHPBar(QPainter* p)
{
    if (m_dead) return;

    p->save();

    // Name
    p->setPen(Qt::white);
    QFont f("Microsoft YaHei", 10);
    f.setBold(true);
    p->setFont(f);
    p->drawText(QRectF(0, -45, W, 16), Qt::AlignCenter, m_name);

    // HP bar background
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(40, 40, 40));
    p->drawRect(QRectF(0, -28, W, 8));

    // HP fill
    qreal ratio = static_cast<qreal>(m_hp) / m_maxHp;
    QColor hpColor = (ratio > 0.5) ? QColor(200, 60, 60)
                     : QColor(220, 20, 20);
    p->setBrush(hpColor);
    p->drawRect(QRectF(0, -28, W * ratio, 8));

    // Border
    p->setPen(QPen(QColor(150, 150, 150), 1));
    p->setBrush(Qt::NoBrush);
    p->drawRect(QRectF(0, -28, W, 8));

    p->restore();
}

void Enemy::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    drawBoss(painter);
    drawHPBar(painter);
}
