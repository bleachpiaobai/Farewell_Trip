#ifndef HUD_H
#define HUD_H

#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QProgressBar>
#include <QLabel>

class Player;
class QGraphicsOpacityEffect;

// ── Chapter Title Card — full-screen black opening screen ──

class ChapterTitleCard : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ChapterTitleCard(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    void show(const QString& title);
    void hide();

    bool isVisible() const { return QGraphicsObject::isVisible(); }

signals:
    void dismissed();  // user clicked or timer expired

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QString m_title;
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

    /// Set the speaker name (shown immediately, styled)
    void setSpeaker(const QString& speaker);
    /// Set the dialogue text (revealed with typewriter effect)
    void setText(const QString& text);
    void showNextIndicator(bool visible);
    void show();
    void hide();

    bool isVisible() const { return QGraphicsRectItem::isVisible(); }

private:
    static QColor speakerColor(const QString& speaker);

    QGraphicsTextItem* m_speakerItem = nullptr;  // speaker name line
    QGraphicsTextItem* m_textItem    = nullptr;  // dialogue text line
    QGraphicsTextItem* m_nextHint    = nullptr;  // "press space to continue"
};

#endif // HUD_H
