#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsObject>
#include <QPainter>
#include "core/GameConfig.h"

class SpriteAnimation;

class Player : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Player(QGraphicsItem* parent = nullptr);

    // ── State ──
    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    bool isAttacking() const { return m_attackTimer > 0; }
    bool isJumping() const { return m_jumping; }
    GameConfig::AnimState animState() const { return m_anim; }
    int dir() const { return m_dir; }
    int attackDir() const { return m_attackDir; }  // locked at attack() call

    void setMaxX(qreal maxX) { m_maxX = maxX; }

    void setHp(int hp);
    void takeDamage(int dmg);

    /// Reset player state for a new game (HP, animation, effects)
    void resetState();

    // ── Input ──
    void moveLeft();
    void moveRight();
    void attack();
    void jump();
    void stopMoving();

    // ── Tick (called each frame) ──
    void tick();

    // ── Bounds ──
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    // ── Draw ──
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void hpChanged(int current, int max);
    void attackTriggered();
    void playerDied();

private:
    void loadSprites();
    QList<QPixmap> loadFrames(const QString& prefix, int count);
    void playAnim(SpriteAnimation* anim);
    const QPixmap& currentFrame() const;

    void applyPhysics();

    // ── Stats ──
    int m_hp = 100;
    int m_maxHp = 100;
    int m_dir = 1;          // 1 = right, -1 = left
    int m_attackDir = 1;    // direction locked at attack() call
    int m_attackTimer = 0;
    GameConfig::AnimState m_anim = GameConfig::AnimState::IDLE;

    // ── Jump physics ──
    bool m_jumping = false;
    qreal m_velocityY = 0.0;

    // ── Sprite animations ──
    SpriteAnimation* m_animWalk   = nullptr;
    SpriteAnimation* m_animJump   = nullptr;
    SpriteAnimation* m_animAttack = nullptr;
    SpriteAnimation* m_animDie    = nullptr;
    SpriteAnimation* m_currentAnim = nullptr;

    QPixmap m_idleFrame;       // single frame when idle

    // ── Play area (public for chapter bounds) ──
public:
    static constexpr int W = 100;
    static constexpr int H = 180;
private:
    qreal m_maxX = GameConfig::WINDOW_WIDTH - W;  // dynamic for wide scenes
};

#endif // PLAYER_H
