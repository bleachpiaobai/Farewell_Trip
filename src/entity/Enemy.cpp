#include "Enemy.h"
#include "animation/SpriteAnimation.h"
#include "utils/MathUtils.h"
#include <QtMath>
#include <QDebug>
#include <QTransform>

// ── Helper: load frames from QRC prefix ──
static QList<QPixmap> loadFrames(const QString& prefix, int count, int targetW, int targetH)
{
    QList<QPixmap> frames;
    for (int i = 1; i <= count; i++) {
        QString path = QString("%1/%2.png").arg(prefix).arg(i);
        QPixmap pix(path);
        if (!pix.isNull()) {
            pix = pix.scaledToHeight(targetH, Qt::SmoothTransformation);
            frames.append(pix);
        } else {
            qWarning() << "Enemy: failed to load frame" << path;
        }
    }
    return frames;
}

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
            pix = pix.scaledToHeight(H, Qt::SmoothTransformation);
            m_frameW = qMax(m_frameW, static_cast<qreal>(pix.width()));
            m_frameH = qMax(m_frameH, static_cast<qreal>(pix.height()));
            frames.append(pix);
        }
    }

    if (frames.isEmpty()) return;

    m_spriteFrame = frames.first();  // fallback

    m_spriteAnim = new SpriteAnimation(this);
    m_spriteAnim->setFrames(frames);
    m_spriteAnim->setFrameDuration(msPerFrame);
    connect(m_spriteAnim, &SpriteAnimation::frameChanged, this, [this](int) { update(); });
    connect(m_spriteAnim, &SpriteAnimation::frameChanged, this, &Enemy::onJumpFrame);
    m_spriteAnim->start();  // always animating (idle animation)

    // 保存基准位置供跳跃使用
    m_baseX = x();
    m_baseY = y();
}

void Enemy::setWalkSprites(const QString& qrcPrefix, int frameCount, int msPerFrame)
{
    auto frames = loadFrames(qrcPrefix, frameCount, W, H);
    for (const auto& f : frames) {
        m_frameW = qMax(m_frameW, static_cast<qreal>(f.width()));
        m_frameH = qMax(m_frameH, static_cast<qreal>(f.height()));
    }
    m_animWalk = new SpriteAnimation(this);
    m_animWalk->setFrames(frames);
    m_animWalk->setFrameDuration(msPerFrame);
    connect(m_animWalk, &SpriteAnimation::frameChanged, this, [this](int) { update(); });
    // 不再连接 onJumpFrame — walk anim 不需要跳跃偏移

    // 用第一帧作为 idle
    if (!frames.isEmpty()) {
        m_idleFrame = frames.first();
    }

    // Start looping walk as default
    playWalkAnim();
}

void Enemy::setAttackSprites(const QString& qrcPrefix, int frameCount, int msPerFrame)
{
    auto frames = loadFrames(qrcPrefix, frameCount, W, H);
    for (const auto& f : frames) {
        m_frameW = qMax(m_frameW, static_cast<qreal>(f.width()));
        m_frameH = qMax(m_frameH, static_cast<qreal>(f.height()));
    }
    m_animAttack = new SpriteAnimation(this);
    m_animAttack->setFrames(frames);
    m_animAttack->setFrameDuration(msPerFrame);
    connect(m_animAttack, &SpriteAnimation::frameChanged, this, [this](int) { update(); });
}

void Enemy::playWalkAnim()
{
    if (!m_animWalk || m_currentAnim == m_animWalk) return;
    if (m_currentAnim) m_currentAnim->stop();
    m_currentAnim = m_animWalk;
    m_animWalk->restart();
    update();
}

void Enemy::playAttackAnim()
{
    if (!m_animAttack) return;
    if (m_currentAnim) m_currentAnim->stop();
    m_currentAnim = m_animAttack;
    m_animAttack->restart();
    update();
}

void Enemy::faceToward(qreal targetX)
{
    qreal dx = targetX - x();
    int newDir = (dx > 0) ? 1 : -1;
    if (newDir != m_dir) {
        prepareGeometryChange();
        m_dir = newDir;
    }
}

void Enemy::moveToward(qreal targetX, qreal speed)
{
    qreal dx = targetX - x();
    int newDir = (dx > 0) ? 1 : -1;
    if (newDir != m_dir) {
        prepareGeometryChange();
        m_dir = newDir;
    }
    if (qAbs(dx) < 5.0) return;

    qreal move = qBound(-speed, dx, speed);
    setPos(x() + move, y());
    m_baseX = x();
}

// ── Jump behavior ─────────────────────────────────────────

void Enemy::enableJumping(qreal jumpHeight, qreal jumpWidth)
{
    m_jumpingEnabled = true;
    m_jumpHeight = jumpHeight;
    m_jumpWidth  = jumpWidth;
    m_baseX = x();
    m_baseY = y();
}

void Enemy::onJumpFrame(int frameIndex)
{
    if (!m_jumpingEnabled || m_dead) return;

    int totalFrames = m_spriteAnim ? m_spriteAnim->frameCount() : 6;
    if (totalFrames <= 1) return;

    // ── 跳跃轨迹：正弦波模拟抛物线跳跃 ──
    //  Y轴：sin(0→π) 从地面→最高点→地面（一个完整跳跃弧）
    //  X轴：sin(0→2π) 左右微微摇摆
    qreal phase = static_cast<qreal>(frameIndex) / totalFrames;
    qreal yOffset = -m_jumpHeight * qSin(phase * M_PI);
    qreal xOffset = m_jumpWidth * qSin(phase * 2.0 * M_PI);

    setPos(m_baseX + xOffset, m_baseY + yOffset);
}

QRectF Enemy::boundingRect() const
{
    qreal cw = (m_collisionW > 0) ? m_collisionW : m_frameW;
    qreal ch = (m_collisionH > 0) ? m_collisionH : m_frameH;
    // 右朝向时 sprite 画在 (0, 0)；左朝向时 flip 后画在 (W - m_frameW, 0)
    if (m_dir == 1)
        return QRectF(0, 0, cw, ch);
    else
        return QRectF(W - cw, 0, cw, ch);
}

QPainterPath Enemy::shape() const
{
    QPainterPath path;
    QRectF rect = boundingRect();
    // 椭圆碰撞体，略微内缩让手感自然
    qreal mx = rect.width()  * 0.05;
    qreal my = rect.height() * 0.05;
    path.addEllipse(rect.adjusted(mx, my, -mx, -my));
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
    if (!m_dead) {
        playWalkAnim();
    }
    if (m_spriteAnim) m_spriteAnim->restart();
}

// ── Drawing ──────────────────────────────────────────────

void Enemy::drawBoss(QPainter* p)
{
    if (m_dead) return;

    p->save();
    p->setRenderHint(QPainter::SmoothPixmapTransform);

    // ── Resolve the frame to draw ──
    QPixmap frame;

    if (m_currentAnim && m_currentAnim->isRunning()) {
        frame = m_currentAnim->currentFrame();
    } else if (!m_idleFrame.isNull()) {
        frame = m_idleFrame;
    } else if (m_spriteAnim && m_spriteAnim->isRunning()) {
        frame = m_spriteAnim->currentFrame();
    } else if (!m_spriteFrame.isNull()) {
        frame = m_spriteFrame;
    }

    if (!frame.isNull()) {
        // Flip horizontally when facing left — use painter transform
        // so the flip axis stays at the character's body center (X=W)
        if (m_dir == -1) {
            p->translate(W, 0);
            p->scale(-1, 1);
        }
        p->drawPixmap(0, 0, frame);
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
