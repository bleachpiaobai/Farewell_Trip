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

    void setHp(int hp);
    void takeDamage(int dmg);

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

    // ── Play area ──
    static constexpr int W = 60;
    static constexpr int H = 110;
};

#endif // PLAYER_H
