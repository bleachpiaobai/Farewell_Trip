#include "HUD.h"
#include "core/GameConfig.h"
#include <QFont>
#include <QPen>
#include <QTextDocument>
#include <QTextOption>

// ── SceneTitleLabel ─────────────────────────────────────

SceneTitleLabel::SceneTitleLabel(QGraphicsItem* parent)
    : QGraphicsTextItem(parent)
{
    setDefaultTextColor(QColor(255, 255, 255));
    QFont f("Microsoft YaHei", 30);
    f.setBold(true);
    setFont(f);
    setZValue(100);
    // Default position: top center area
    setPos(0, 80);
    setTextWidth(GameConfig::WINDOW_WIDTH);
    // Center alignment
    QTextOption opt = document()->defaultTextOption();
    opt.setAlignment(Qt::AlignCenter);
    document()->setDefaultTextOption(opt);
}

void SceneTitleLabel::setTitle(const QString& title, const QString& hint)
{
    if (hint.isEmpty()) {
        setHtml(QStringLiteral("<div style='text-align:center;color:white;'>%1</div>").arg(title));
    } else {
        setHtml(QStringLiteral(
            "<div style='text-align:center;color:white;'>%1</div>"
            "<div style='text-align:center;color:#CCCCCC;font-size:18px;margin-top:20px;'>%2</div>"
        ).arg(title, hint));
    }
}

// ── DialogueBox ─────────────────────────────────────────

DialogueBox::DialogueBox(QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
{
    constexpr qreal BOX_X = 50;
    constexpr qreal BOX_Y = 520;
    constexpr qreal BOX_W = GameConfig::WINDOW_WIDTH - 100;
    constexpr qreal BOX_H = 160;

    setRect(BOX_X, BOX_Y, BOX_W, BOX_H);
    setBrush(QColor(0, 0, 0, 200));
    setPen(QPen(Qt::white, 1));
    setZValue(200);

    // Main text
    m_textItem = new QGraphicsTextItem(this);
    m_textItem->setDefaultTextColor(QColor(255, 255, 255));
    m_textItem->setFont(QFont("Microsoft YaHei", 18));
    m_textItem->setPos(BOX_X + 20, BOX_Y + 20);
    m_textItem->setTextWidth(BOX_W - 40);
    m_textItem->setZValue(201);

    // "Press Space to continue" indicator
    m_nextHint = new QGraphicsTextItem(this);
    m_nextHint->setDefaultTextColor(QColor(200, 200, 200));
    m_nextHint->setFont(QFont("Microsoft YaHei", 12));
    m_nextHint->setPos(BOX_X + BOX_W - 160, BOX_Y + BOX_H - 30);
    m_nextHint->setZValue(201);
    m_nextHint->setPlainText(QStringLiteral("【空格/点击 继续】"));
    m_nextHint->hide();
}

void DialogueBox::setText(const QString& text)
{
    m_textItem->setPlainText(text);
}

void DialogueBox::showNextIndicator(bool visible)
{
    m_nextHint->setVisible(visible);
}

void DialogueBox::show()
{
    QGraphicsRectItem::setVisible(true);
    m_textItem->setVisible(true);
}

void DialogueBox::hide()
{
    QGraphicsRectItem::setVisible(false);
    m_textItem->setVisible(false);
}
