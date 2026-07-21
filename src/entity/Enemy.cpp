#include "Enemy.h"
#include "animation/SpriteAnimation.h"
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

void Enemy::setSpriteSheet(const QString& qrcPrefix, int frameCount, int msPerFrame)
{
    QList<QPixmap> frames;
    for (int i = 1; i <= frameCount; i++) {
        QString path = QString("%1/%2.png").arg(qrcPrefix).arg(i);
        QPixmap pix(path);
        if (!pix.isNull()) {
            pix = pix.scaled(W, H, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            frames.append(pix);
        }
    }

    if (frames.isEmpty()) return;

    m_spriteFrame = frames.first();  // fallback

    m_spriteAnim = new SpriteAnimation(this);
    m_spriteAnim->setFrames(frames);
    m_spriteAnim->setFrameDuration(msPerFrame);
    connect(m_spriteAnim, &SpriteAnimation::frameChanged, this, [this](int) { update(); });
    m_spriteAnim->start();  // always animating (idle animation)
}

QRectF Enemy::boundingRect() const
{
    return QRectF(-5, -5, W + 10, H + 10);
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
    update();
    if (m_hp <= 0 && !m_dead) {
        m_dead = true;
        if (m_spriteAnim) m_spriteAnim->stop();
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
    if (m_spriteAnim) m_spriteAnim->restart();
}

// ── Drawing ──────────────────────────────────────────────

void Enemy::drawBoss(QPainter* p)
{
    if (m_dead) return;

    p->save();
    p->setRenderHint(QPainter::SmoothPixmapTransform);

    // ── Sprite-based drawing ──
    if (m_spriteAnim && m_spriteAnim->isRunning()) {
        QPixmap frame = m_spriteAnim->currentFrame();
        if (!frame.isNull()) {
            p->drawPixmap(0, 0, frame);
            p->restore();
            return;
        }
    }

    // ── Single sprite frame fallback ──
    if (!m_spriteFrame.isNull()) {
        p->drawPixmap(0, 0, m_spriteFrame);
        p->restore();
        return;
    }

    // ── Procedural fallback (original) ──
    p->setPen(Qt::NoPen);

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


void Enemy::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    drawBoss(painter);
}
