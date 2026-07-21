#include "HUD.h"
#include "core/GameConfig.h"
#include <QFont>
#include <QPen>
#include <QPainter>
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

// ═══════════════════════════════════════════════════════════
//  Player HP Bar (top-left corner)
// ═══════════════════════════════════════════════════════════

PlayerHPBar::PlayerHPBar(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    setPos(20, 12);
    setZValue(500);  // above everything
}

QRectF PlayerHPBar::boundingRect() const
{
    // Name + bar + text
    return QRectF(0, 0, 220, 48);
}

void PlayerHPBar::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
    p->setRenderHint(QPainter::Antialiasing);

    constexpr qreal BAR_W = 200;
    constexpr qreal BAR_H = 16;
    constexpr qreal BAR_Y = 24;

    // ── Player name ──
    p->setPen(QColor(220, 220, 255));
    QFont nameFont("Microsoft YaHei", 12);
    nameFont.setBold(true);
    p->setFont(nameFont);
    p->drawText(QRectF(0, 0, BAR_W, 20), Qt::AlignLeft | Qt::AlignVCenter, "YAN");

    // ── HP bar background ──
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(0, 0, 0, 160));
    p->drawRoundedRect(QRectF(0, BAR_Y, BAR_W, BAR_H), 4, 4);

    // ── HP fill ──
    qreal ratio = static_cast<qreal>(m_currentHp) / m_maxHp;
    QColor hpColor;
    if (ratio > 0.5)
        hpColor = QColor(60, 200, 80);        // green
    else if (ratio > 0.25)
        hpColor = QColor(240, 180, 40);       // yellow
    else
        hpColor = QColor(230, 50, 50);        // red

    if (ratio > 0.0) {
        p->setBrush(hpColor);
        p->drawRoundedRect(QRectF(0, BAR_Y, BAR_W * ratio, BAR_H), 4, 4);
    }

    // ── Border ──
    p->setPen(QPen(QColor(180, 180, 200, 180), 1));
    p->setBrush(Qt::NoBrush);
    p->drawRoundedRect(QRectF(0, BAR_Y, BAR_W, BAR_H), 4, 4);

    // ── HP text ──
    p->setPen(Qt::white);
    QFont hpFont("Consolas", 10);
    hpFont.setBold(true);
    p->setFont(hpFont);
    p->drawText(QRectF(0, BAR_Y, BAR_W, BAR_H), Qt::AlignCenter,
                QString("%1 / %2").arg(m_currentHp).arg(m_maxHp));
}

void PlayerHPBar::onHpChanged(int current, int max)
{
    m_currentHp = current;
    m_maxHp = max;
    update();
}

// ═══════════════════════════════════════════════════════════
//  Boss HP Bar (bottom center, during boss fights)
// ═══════════════════════════════════════════════════════════

BossHPBar::BossHPBar(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    // Position: centered at bottom, above dialogue box area
    setPos((GameConfig::WINDOW_WIDTH - 500) / 2, 470);
    setZValue(500);
    setVisible(false);
}

QRectF BossHPBar::boundingRect() const
{
    return QRectF(0, -30, 500, 56);  // name above + bar
}

void BossHPBar::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!m_visible) return;

    p->setRenderHint(QPainter::Antialiasing);

    constexpr qreal BAR_W = 500;
    constexpr qreal BAR_H = 22;

    // ── Boss name ──
    p->setPen(QColor(255, 200, 180));
    QFont nameFont("Microsoft YaHei", 14);
    nameFont.setBold(true);
    p->setFont(nameFont);
    p->drawText(QRectF(0, -28, BAR_W, 24), Qt::AlignCenter, m_bossName);

    // ── HP bar background ──
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(20, 20, 20, 200));
    p->drawRoundedRect(QRectF(0, 0, BAR_W, BAR_H), 6, 6);

    // ── HP fill ──
    qreal ratio = static_cast<qreal>(m_currentHp) / m_maxHp;
    // Boss uses dramatic red gradient
    QColor hpColor = (ratio > 0.5) ? QColor(200, 60, 60)
                     : (ratio > 0.25) ? QColor(220, 30, 30)
                     : QColor(240, 10, 10);

    if (ratio > 0.0) {
        p->setBrush(hpColor);
        p->drawRoundedRect(QRectF(0, 0, BAR_W * ratio, BAR_H), 6, 6);
    }

    // ── Border ──
    p->setPen(QPen(QColor(200, 150, 130, 200), 2));
    p->setBrush(Qt::NoBrush);
    p->drawRoundedRect(QRectF(0, 0, BAR_W, BAR_H), 6, 6);

    // ── HP text ──
    p->setPen(Qt::white);
    QFont hpFont("Consolas", 11);
    hpFont.setBold(true);
    p->setFont(hpFont);
    p->drawText(QRectF(0, 0, BAR_W, BAR_H), Qt::AlignCenter,
                QString("%1 / %2").arg(m_currentHp).arg(m_maxHp));
}

void BossHPBar::showForBoss(const QString& bossName, int current, int max)
{
    m_bossName = bossName;
    m_currentHp = current;
    m_maxHp = max;
    m_visible = true;
    setVisible(true);
    update();
}

void BossHPBar::hideBar()
{
    m_visible = false;
    setVisible(false);
}

void BossHPBar::onHpChanged(const QString& bossName, int current, int max)
{
    m_bossName = bossName;
    m_currentHp = current;
    m_maxHp = max;
    update();
}

// ═══════════════════════════════════════════════════════════
//  DialogueBox
// ═══════════════════════════════════════════════════════════

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
