#ifndef LASER_H
#define LASER_H

#include <QGraphicsObject>
#include <QPainter>
#include <QColor>

// ╔══════════════════════════════════════════════════════════╗
// ║  竖直激光束 —— XIA 专属攻击                                ║
// ║  两阶段：预警（半透明不扣血）→ 激活（亮色扣血）               ║
// ╚══════════════════════════════════════════════════════════╝

class Laser : public QGraphicsObject
{
    Q_OBJECT
public:
    /// @param x          激光顶部 X 坐标
    /// @param yTop       激光顶部 Y
    /// @param height     激光高度
    /// @param width      激光宽度
    /// @param color      激活时颜色
    /// @param telegraphFrames  预警阶段帧数（不扣血）
    /// @param activeFrames     激活阶段帧数（扣血）
    /// @param damage     激活时伤害
    explicit Laser(qreal x, qreal yTop, qreal height, qreal width,
                   const QColor& color,
                   int telegraphFrames, int activeFrames, int damage,
                   QGraphicsItem* parent = nullptr);

    int damage() const { return m_damage; }
    bool isActive() const { return m_phase == ACTIVE; }
    bool isExpired() const { return m_expired; }
    bool hasHit() const { return m_hasHit; }
    void markHit() { m_hasHit = true; }

    /// 每帧更新阶段
    void tick();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:
    enum Phase { TELEGRAPH, ACTIVE, DONE };
    Phase m_phase = TELEGRAPH;
    QColor m_color;
    qreal  m_laserWidth;
    qreal  m_laserHeight;
    int    m_damage;
    int    m_telegraphFrames;
    int    m_activeFrames;
    int    m_timer = 0;
    bool   m_expired = false;
    bool   m_hasHit = false;
};

#endif // LASER_H
