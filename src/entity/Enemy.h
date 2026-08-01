#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsObject>
#include <QPainter>
#include "core/GameConfig.h"

class SpriteAnimation;

class Enemy : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Enemy(const QString& name, int maxHp, const QColor& bodyColor,
                   QGraphicsItem* parent = nullptr);

    QString name() const { return m_name; }
    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    bool isDead() const { return m_dead; }
    int dir() const { return m_dir; }

    void setHp(int hp);
    void takeDamage(int dmg);
    void kill();
    void reset();

    /// Load a sprite sheet for this enemy (replaces procedural drawing)
    void setSpriteSheet(const QString& qrcPrefix, int frameCount, int msPerFrame = 100);

    /// Set separate walk / attack sprite sheets (like YAN's structure)
    void setWalkSprites(const QString& qrcPrefix, int frameCount, int msPerFrame = 100);
    void setAttackSprites(const QString& qrcPrefix, int frameCount, int msPerFrame = 80);

    /// Play a specific animation
    void playWalkAnim();
    void playAttackAnim();

    /// Move toward a target X coordinate (used by boss AI)
    void moveToward(qreal targetX, qreal speed);

    /// Face toward a target without moving
    void faceToward(qreal targetX);

    /// Enable jumping movement (桃子怪跳跃移动)
    /// @param jumpHeight  vertical jump amplitude in pixels
    /// @param jumpWidth   horizontal wobble amplitude in pixels
    void enableJumping(qreal jumpHeight = 50.0, qreal jumpWidth = 20.0);

    /// Whether the enemy is currently airborne (jumping)
    bool isAirborne() const { return m_jumpingEnabled && !m_dead && y() < m_baseY - 5.0; }

    /// Override collision size (0 = use frame dimensions)
    void setCollisionSize(qreal w, qreal h) { m_collisionW = w; m_collisionH = h; }

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void hpChanged(const QString& bossName, int current, int max);
    void defeated(const QString& bossName);

private slots:
    void onJumpFrame(int frameIndex);

private:
    void drawBoss(QPainter* p);

    QString m_name;
    int m_hp;
    int m_maxHp;
    QColor m_bodyColor;
    bool m_dead = false;
    int  m_dir = 1;  // 1 = facing right, -1 = facing left

    SpriteAnimation* m_spriteAnim = nullptr;
    SpriteAnimation* m_animWalk   = nullptr;
    SpriteAnimation* m_animAttack = nullptr;
    SpriteAnimation* m_currentAnim = nullptr;  // which anim is playing now
    QPixmap m_spriteFrame;      // fallback single frame
    QPixmap m_idleFrame;        // idle when no anim playing

    // ── Jump behavior ──
    bool   m_jumpingEnabled = false;
    qreal  m_jumpHeight = 0.0;
    qreal  m_jumpWidth  = 0.0;
    qreal  m_baseX = 0.0;
    qreal  m_baseY = 0.0;

    // Actual sprite frame dimensions (set when loading sprites)
    qreal m_frameW = 140;
    qreal m_frameH = 140;

    // Override collision size (0 = use frame dimensions m_frameW/m_frameH)
    qreal m_collisionW = 0;
    qreal m_collisionH = 0;

    static constexpr int W = 140;
    static constexpr int H = 140;
};

// ── Concrete Boss factories ─────────────────────────────

inline Enemy* createPeachBoss(QGraphicsItem* parent = nullptr)
{
    auto* e = new Enemy(QStringLiteral("桃子怪"), GameConfig::PEACH_BOSS_HP,
                        QColor(GameConfig::PlaceholderColor::PeachBoss), parent);
    e->setPos(880, 520);
    e->setSpriteSheet(GameConfig::imagePath(":/images/Peach_Action"), 6, 120);
    e->enableJumping(30.0, 10.0);  // 跳跃高度30px（YAN可跳过），左右摆动10px
    return e;
}

inline Enemy* createExGirlBoss(QGraphicsItem* parent = nullptr)
{
    auto* e = new Enemy(QStringLiteral("XIA"), GameConfig::EXGIRL_BOSS_HP,
                        QColor(GameConfig::PlaceholderColor::ExGirlBoss), parent);
    e->setPos(880, 520);
    // 参照 YAN 的精灵结构：walk 动画 9 帧 + ack 攻击动画 8 帧
    e->setWalkSprites(GameConfig::imagePath(":/images/XIA_Action/walk"), 9, 100);
    e->setAttackSprites(GameConfig::imagePath(":/images/XIA_Action/ack"), 8, 80);
    // walk 帧 1022px 宽（缩放到 204px）远大于实际角色 ~82px，手动收紧碰撞框
    e->setCollisionSize(85, 135);
    return e;
}

inline Enemy* createAngusBoss(QGraphicsItem* parent = nullptr)
{
    auto* e = new Enemy(QStringLiteral("Angus"), GameConfig::ANGUS_BOSS_HP,
                        QColor(GameConfig::PlaceholderColor::AngusBoss), parent);
    e->setPos(880, 520);
    // Use Doc robot sprite as idle frame
    e->setSpriteSheet(GameConfig::imagePath(":/images/Doc_Action/doc_robot"), 1, 100);
    return e;
}

#endif // ENEMY_H
