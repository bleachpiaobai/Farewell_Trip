#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <QGraphicsObject>
#include <QPainter>
#include <QColor>

class Projectile : public QGraphicsObject
{
    Q_OBJECT
public:
    /// @param color     子弹颜色
    /// @param radius    子弹半径（像素）— 碰撞判定仅用此半径
    /// @param damage    伤害
    /// @param vx, vy    速度（像素/帧）
    /// @param gravity   重力加速度
    /// @param lifetime  最大存活帧数
    explicit Projectile(const QColor& color, qreal radius, int damage,
                        qreal vx, qreal vy, qreal gravity = 0.0,
                        int lifetime = -1, QGraphicsItem* parent = nullptr);

    int damage() const { return m_damage; }
    bool isExpired() const { return m_expired; }

    void setBouncing(bool b) { m_bouncing = b; }
    bool isBouncing() const { return m_bouncing; }

    void tick();

    // boundingRect = 精确碰撞（仅实体圆，无发光余量）
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:
    QColor m_color;
    qreal  m_radius;       // 碰撞半径
    int    m_damage;
    qreal  m_vx, m_vy;
    qreal  m_gravity;
    int    m_lifetime;
    bool   m_expired = false;
    bool   m_bouncing = false;

    QList<QPointF> m_trail;
    static constexpr int MAX_TRAIL = 5;
    static constexpr qreal GLOW = 6.0;   // 发光余量（仅绘制，不计入碰撞）
};

#endif // PROJECTILE_H
