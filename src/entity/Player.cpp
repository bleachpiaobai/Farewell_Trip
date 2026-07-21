#include "Player.h"
#include "animation/SpriteAnimation.h"
#include "utils/MathUtils.h"
#include <QDebug>
#include <QTransform>

Player::Player(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    setPos(100, GameConfig::PLAYER_GROUND_Y);
    setZValue(10);

    loadSprites();
}

// ── Sprite loading ────────────────────────────────────────

QList<QPixmap> Player::loadFrames(const QString& prefix, int count)
{
    QList<QPixmap> frames;
    for (int i = 1; i <= count; i++) {
        QString path = QString("%1/%2.png").arg(prefix).arg(i);
        QPixmap pix(path);
        if (!pix.isNull()) {
            // Scale to player size, keeping aspect ratio
            pix = pix.scaled(W, H, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            frames.append(pix);
        } else {
            qWarning() << "Player: failed to load frame" << path;
        }
    }
    return frames;
}

void Player::loadSprites()
{
    // ── Walk animation (10 frames) ──
    m_animWalk = new SpriteAnimation(this);
    m_animWalk->setFrames(loadFrames(":/images/yan_action/walk", 10));
    m_animWalk->setFrameDuration(80);  // ~12.5 fps
    connect(m_animWalk, &SpriteAnimation::frameChanged, this, [this](int) { update(); });

    // ── Jump animation (7 frames) ──
    m_animJump = new SpriteAnimation(this);
    m_animJump->setFrames(loadFrames(":/images/yan_action/jump", 7));
    m_animJump->setFrameDuration(70);
    connect(m_animJump, &SpriteAnimation::frameChanged, this, [this](int) { update(); });

    // ── Attack animation (7 frames, "ack" folder) ──
    m_animAttack = new SpriteAnimation(this);
    m_animAttack->setFrames(loadFrames(":/images/yan_action/ack", 7));
    m_animAttack->setFrameDuration(60);  // ~16 fps — snappy
    connect(m_animAttack, &SpriteAnimation::frameChanged, this, [this](int) { update(); });

    // ── Die animation (5 frames) ──
    m_animDie = new SpriteAnimation(this);
    m_animDie->setFrames(loadFrames(":/images/yan_action/die", 5));
    m_animDie->setFrameDuration(120);  // slower, dramatic
    connect(m_animDie, &SpriteAnimation::frameChanged, this, [this](int) { update(); });

    // ── Idle: use first walk frame ──
    QPixmap idle(":/images/yan_action/walk/1.png");
    if (!idle.isNull()) {
        m_idleFrame = idle.scaled(W, H, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}

void Player::playAnim(SpriteAnimation* anim)
{
    if (m_currentAnim == anim) {
        if (!anim->isRunning()) {
            anim->restart();
            update();
        }
        return;
    }

    if (m_currentAnim) {
        m_currentAnim->stop();
    }

    m_currentAnim = anim;
    if (anim) {
        anim->restart();
    }
    update();
}

const QPixmap& Player::currentFrame() const
{
    if (m_currentAnim && m_currentAnim->isRunning()) {
        return m_currentAnim->currentFrame();
    }
    return m_idleFrame;
}

// ── Bounds ────────────────────────────────────────────────

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

// ── HP ────────────────────────────────────────────────────

void Player::setHp(int hp)
{
    m_hp = MathUtils::clamp(hp, 0, m_maxHp);
    emit hpChanged(m_hp, m_maxHp);
    update();
}

void Player::takeDamage(int dmg)
{
    setHp(m_hp - dmg);
}

// ── Input ─────────────────────────────────────────────────

void Player::moveLeft()
{
    setPos(x() - GameConfig::PLAYER_MOVE_SPEED, y());
    m_dir = -1;
    if (!m_jumping) {
        m_anim = GameConfig::AnimState::WALK_LEFT;
        playAnim(m_animWalk);
    }
}

void Player::moveRight()
{
    setPos(x() + GameConfig::PLAYER_MOVE_SPEED, y());
    m_dir = 1;
    if (!m_jumping) {
        m_anim = GameConfig::AnimState::WALK_RIGHT;
        playAnim(m_animWalk);
    }
}

void Player::attack()
{
    if (m_attackTimer > 0) {
        return;
    }
    m_attackTimer = GameConfig::ATTACK_COOLDOWN;
    m_anim = GameConfig::AnimState::ATTACK;
    playAnim(m_animAttack);
    emit attackTriggered();
}

void Player::jump()
{
    if (m_jumping) return;
    m_jumping = true;
    m_velocityY = GameConfig::JUMP_VELOCITY;
    m_anim = GameConfig::AnimState::IDLE;
    playAnim(m_animJump);
}

void Player::stopMoving()
{
    if (!m_jumping &&
        (m_anim == GameConfig::AnimState::WALK_LEFT ||
         m_anim == GameConfig::AnimState::WALK_RIGHT)) {
        m_anim = GameConfig::AnimState::IDLE;
        if (m_currentAnim) {
            m_currentAnim->stop();
            m_currentAnim = nullptr;
        }
        update();
    }
}

// ── Tick ──────────────────────────────────────────────────

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
            if (m_currentAnim) {
                m_currentAnim->stop();
                m_currentAnim = nullptr;
            }
            update();
        }
        setPos(x(), newY);
    }

    // ── Attack cooldown ──
    if (m_attackTimer > 0) {
        m_attackTimer--;
        if (m_attackTimer == 0 && m_anim == GameConfig::AnimState::ATTACK) {
            m_anim = GameConfig::AnimState::IDLE;
            if (m_currentAnim) {
                m_currentAnim->stop();
                m_currentAnim = nullptr;
            }
            update();
        }
    }
}

// ── Drawing ────────────────────────────────────────────────

void Player::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    // ── Draw sprite frame ──
    QPixmap frame = currentFrame();
    if (!frame.isNull()) {
        if (m_dir == -1) {
            // Flip horizontally for left-facing
            frame = frame.transformed(QTransform().scale(-1, 1), Qt::SmoothTransformation);
        }
        painter->drawPixmap(0, 0, frame);
    } else {
        // Fallback: draw a simple colored rect
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(GameConfig::PlaceholderColor::Player));
        painter->drawRect(QRectF(0, 0, W, H));
    }

}
