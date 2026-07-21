#ifndef HUD_H
#define HUD_H

#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QProgressBar>
#include <QLabel>

class Player;
class QGraphicsOpacityEffect;

// ── Scene title overlay (top center) ───────────────────

class SceneTitleLabel : public QGraphicsTextItem
{
public:
    explicit SceneTitleLabel(QGraphicsItem* parent = nullptr);
    void setTitle(const QString& title, const QString& hint = QString());
};

// ── Player HP Bar (top-left corner) ────────────────────

class PlayerHPBar : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit PlayerHPBar(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

public slots:
    void onHpChanged(int current, int max);

private:
    int m_currentHp = 100;
    int m_maxHp     = 100;
};

// ── Boss HP Bar (bottom center, during boss fights) ─────

class BossHPBar : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit BossHPBar(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

public slots:
    void onHpChanged(const QString& bossName, int current, int max);
    void showForBoss(const QString& bossName, int current, int max);
    void hideBar();

private:
    QString m_bossName;
    int m_currentHp = 80;
    int m_maxHp     = 80;
    bool m_visible  = false;
};

// ── Dialogue box (bottom of screen, Galgame style) ─────

class DialogueBox : public QGraphicsRectItem
{
public:
    explicit DialogueBox(QGraphicsItem* parent = nullptr);

    void setText(const QString& text);
    void showNextIndicator(bool visible);
    void show();
    void hide();

    bool isVisible() const { return QGraphicsRectItem::isVisible(); }

private:
    QGraphicsTextItem* m_textItem = nullptr;
    QGraphicsTextItem* m_nextHint = nullptr;
};

#endif // HUD_H
