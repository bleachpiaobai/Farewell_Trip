#ifndef HUD_H
#define HUD_H

#include <QGraphicsTextItem>
#include <QGraphicsRectItem>

class Player;

// ── Scene title overlay (top center) ───────────────────

class SceneTitleLabel : public QGraphicsTextItem
{
public:
    explicit SceneTitleLabel(QGraphicsItem* parent = nullptr);
    void setTitle(const QString& title, const QString& hint = QString());
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
