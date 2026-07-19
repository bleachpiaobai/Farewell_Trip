#include "Player.h"
#include "utils/MathUtils.h"
#include <QDebug>

Player::Player(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    setPos(100, GameConfig::PLAYER_GROUND_Y);
    setZValue(10);
}

QRectF Player::boundingRect() const
{
    return QRectF(-5, -25, W + 10, H + 25);
}

QPainterPath Player::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, W, H);
    return path;
}

void Player::setHp(int hp)
{
    m_hp = MathUtils::clamp(hp, 0, m_maxHp);
    emit hpChanged(m_hp, m_maxHp);
    update();  // trigger repaint so HP bar reflects the change
}

void Player::takeDamage(int dmg)
{
    setHp(m_hp - dmg);
}

void Player::moveLeft()
{
    setPos(x() - GameConfig::PLAYER_MOVE_SPEED, y());
    m_dir = -1;
    if (!m_jumping)
        m_anim = GameConfig::AnimState::WALK_LEFT;
}

void Player::moveRight()
{
    setPos(x() + GameConfig::PLAYER_MOVE_SPEED, y());
    m_dir = 1;
    if (!m_jumping)
        m_anim = GameConfig::AnimState::WALK_RIGHT;
}

void Player::attack()
{
    if (m_attackTimer > 0) {
        qDebug() << "[Player] Attack BLOCKED — cooldown:" << m_attackTimer;
        return;
    }
    m_attackTimer = GameConfig::ATTACK_COOLDOWN;
    m_anim = GameConfig::AnimState::ATTACK;
    qDebug() << "[Player] Attack! pos:" << x() << y() << "dir:" << m_dir;
    emit attackTriggered();
}

void Player::jump()
{
    if (m_jumping) return; // no double-jump
    m_jumping = true;
    m_velocityY = GameConfig::JUMP_VELOCITY;
    m_anim = GameConfig::AnimState::IDLE; // neutral pose in air
}

void Player::stopMoving()
{
    if (!m_jumping && (m_anim == GameConfig::AnimState::WALK_LEFT ||
                       m_anim == GameConfig::AnimState::WALK_RIGHT))
        m_anim = GameConfig::AnimState::IDLE;
}

void Player::tick()
{
    // ── Horizontal bounds ──
    qreal cx = MathUtils::clamp(x(), 0.0,
                                 static_cast<qreal>(GameConfig::WINDOW_WIDTH - W));
    setPos(cx, y());

    // ── Jump physics ──
    if (m_jumping) {
        m_velocityY += GameConfig::GRAVITY;
        qreal newY = y() + m_velocityY;

        if (newY >= GameConfig::PLAYER_GROUND_Y) {
            // Land
            newY = GameConfig::PLAYER_GROUND_Y;
            m_jumping = false;
            m_velocityY = 0.0;
            m_anim = GameConfig::AnimState::IDLE;
        }
        setPos(x(), newY);
    }

    // ── Attack cooldown ──
    if (m_attackTimer > 0) {
        m_attackTimer--;
        if (m_attackTimer == 0 && m_anim == GameConfig::AnimState::ATTACK)
            m_anim = GameConfig::AnimState::IDLE;
    }

    // ── Animation frame ──
    m_animFrame = (m_animFrame + 1) % 60;
}

// ── Drawing ──────────────────────────────────────────────

void Player::drawRobot(QPainter* p)
{
    p->save();
    // Flip if facing left
    if (m_dir == -1) {
        p->translate(W, 0);
        p->scale(-1, 1);
    }

    // Attack pose: lean forward
    qreal lean = (m_anim == GameConfig::AnimState::ATTACK && m_attackTimer > 6) ? 8.0 : 0.0;

    // Jump: legs tuck up
    qreal legTop = 80.0;
    qreal legLen = 30.0;
    if (m_jumping) {
        legTop = 70.0;
        legLen = 15.0;
    }

    // Body color
    QColor bodyColor = QColor(GameConfig::PlaceholderColor::Player);
    if (m_anim == GameConfig::AnimState::ATTACK)
        bodyColor = bodyColor.lighter(130);

    p->setPen(Qt::NoPen);
    p->setBrush(bodyColor);

    // Head (ellipse)
    p->drawEllipse(QRectF(15 + lean, 0, 30, 30));

    // Eyes
    p->setBrush(Qt::white);
    p->drawEllipse(QRectF(22 + lean, 8, 6, 6));
    p->drawEllipse(QRectF(32 + lean, 8, 6, 6));
    p->setBrush(Qt::black);
    p->drawEllipse(QRectF(24 + lean, 10, 3, 3));
    p->drawEllipse(QRectF(34 + lean, 10, 3, 3));

    // Body
    p->setBrush(bodyColor);
    p->drawRect(QRectF(10 + lean, 30, 40, 50));

    // Core glow
    p->setBrush(QColor(100, 220, 255, 180));
    p->drawEllipse(QRectF(22 + lean, 42, 16, 16));

    // Legs
    qreal legOffset = 0;
    if (m_anim == GameConfig::AnimState::WALK_LEFT || m_anim == GameConfig::AnimState::WALK_RIGHT)
        legOffset = (m_animFrame % 20 < 10) ? 3.0 : -3.0;
    p->setBrush(bodyColor.darker(120));
    p->drawRect(QRectF(15 + lean + legOffset, legTop, 10, legLen));
    p->drawRect(QRectF(35 + lean - legOffset, legTop, 10, legLen));

    // Arms
    p->setBrush(bodyColor);
    if (m_anim == GameConfig::AnimState::ATTACK && m_attackTimer > 6) {
        // Punch forward
        p->drawRect(QRectF(50, 35, 22, 8));
        p->setBrush(QColor(255, 200, 50, 200));
        p->drawEllipse(QRectF(68, 31, 10, 16));
    } else {
        p->drawRect(QRectF(0, 35, 10, 8));
        p->drawRect(QRectF(50, 35, 10, 8));
    }

    p->restore();
}

void Player::drawHPBar(QPainter* p)
{
    // Background
    p->setPen(Qt::NoPen);
    p->setBrush(Qt::black);
    p->drawRect(QRectF(0, -20, W, 8));

    // HP fill
    qreal ratio = static_cast<qreal>(m_hp) / m_maxHp;
    QColor hpColor = (ratio > 0.5) ? QColor(0, 200, 0)
                   : (ratio > 0.25) ? QColor(220, 180, 0)
                   : QColor(220, 0, 0);
    p->setBrush(hpColor);
    p->drawRect(QRectF(0, -20, W * ratio, 8));

    // Border
    p->setPen(QPen(Qt::white, 1));
    p->setBrush(Qt::NoBrush);
    p->drawRect(QRectF(0, -20, W, 8));

    // HP text
    p->setPen(Qt::white);
    QFont f("Consolas", 7);
    p->setFont(f);
    p->drawText(QRectF(0, -20, W, 8), Qt::AlignCenter,
                QString("%1/%2").arg(m_hp).arg(m_maxHp));
}

void Player::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    drawRobot(painter);
    drawHPBar(painter);
}
