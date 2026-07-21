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

    void setHp(int hp);
    void takeDamage(int dmg);
    void kill();
    void reset();

    /// Load a sprite sheet for this enemy (replaces procedural drawing)
    void setSpriteSheet(const QString& qrcPrefix, int frameCount, int msPerFrame = 100);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void hpChanged(const QString& bossName, int current, int max);
    void defeated(const QString& bossName);

private:
    void drawBoss(QPainter* p);

    QString m_name;
    int m_hp;
    int m_maxHp;
    QColor m_bodyColor;
    bool m_dead = false;

    SpriteAnimation* m_spriteAnim = nullptr;
    QPixmap m_spriteFrame;      // fallback single frame

    static constexpr int W = 80;
    static constexpr int H = 80;
};

// ── Concrete Boss factories ─────────────────────────────

inline Enemy* createPeachBoss(QGraphicsItem* parent = nullptr)
{
    auto* e = new Enemy(QStringLiteral("梨花怪"), GameConfig::PEACH_BOSS_HP,
                        QColor(GameConfig::PlaceholderColor::PeachBoss), parent);
    e->setPos(900, 540);
    e->setSpriteSheet(":/images/peach_action", 6, 120);
    return e;
}

inline Enemy* createExGirlBoss(QGraphicsItem* parent = nullptr)
{
    auto* e = new Enemy(QStringLiteral("旧日执念·前女友"), GameConfig::EXGIRL_BOSS_HP,
                        QColor(GameConfig::PlaceholderColor::ExGirlBoss), parent);
    e->setPos(900, 540);
    e->setSpriteSheet(":/images/xia_action", 2, 150);
    return e;
}

inline Enemy* createZhaNanBoss(QGraphicsItem* parent = nullptr)
{
    auto* e = new Enemy(QStringLiteral("幻境执念·渣男"), GameConfig::ZHANAN_BOSS_HP,
                        QColor(GameConfig::PlaceholderColor::ZhaNanBoss), parent);
    e->setPos(900, 540);
    // No dedicated sprite — falls back to procedural drawing
    return e;
}

#endif // ENEMY_H
