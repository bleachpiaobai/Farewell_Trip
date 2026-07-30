#include "HUD.h"
#include "core/GameConfig.h"
#include <QFont>
#include <QPen>
#include <QPainter>
#include <QTextDocument>
#include <QTextOption>

// ── ChapterTitleCard — full-screen black opening screen ──

ChapterTitleCard::ChapterTitleCard(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    setZValue(10001);  // above everything including transition overlay
    setVisible(false);
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
}

QRectF ChapterTitleCard::boundingRect() const
{
    return QRectF(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
}

void ChapterTitleCard::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
    // Full-screen black
    p->fillRect(boundingRect(), Qt::black);

    if (m_title.isEmpty()) return;

    // ── Chapter number (first line) ──
    QStringList lines = m_title.split('\n');
    QString chapterLabel = lines.value(0);  // "第X章"
    QString chapterName  = lines.value(1);  // "少女！..."

    // ── "第X章" — smaller, above ──
    QFont numFont("Microsoft YaHei", 28);
    numFont.setBold(true);
    p->setFont(numFont);
    p->setPen(QColor(200, 200, 200));
    p->drawText(QRectF(0, 260, GameConfig::WINDOW_WIDTH, 50),
                Qt::AlignHCenter | Qt::AlignVCenter, chapterLabel);

    // ── Chapter name — large, center ──
    QFont titleFont("Microsoft YaHei", 42);
    titleFont.setBold(true);
    p->setFont(titleFont);
    p->setPen(Qt::white);
    p->drawText(QRectF(80, 320, GameConfig::WINDOW_WIDTH - 160, 80),
                Qt::AlignHCenter | Qt::AlignVCenter, chapterName);

    // ── "点击继续" hint at bottom ──
    QFont hintFont("Microsoft YaHei", 16);
    p->setFont(hintFont);
    p->setPen(QColor(150, 150, 150));
    p->drawText(QRectF(0, 560, GameConfig::WINDOW_WIDTH, 30),
                Qt::AlignHCenter | Qt::AlignVCenter,
                QStringLiteral("点击或按空格键继续"));
}

void ChapterTitleCard::show(const QString& title)
{
    m_title = title;
    setVisible(true);
    update();
}

void ChapterTitleCard::hide()
{
    m_title.clear();
    setVisible(false);
}

void ChapterTitleCard::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    emit dismissed();
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
    // Position: bottom-center, below the player character area
    setPos((GameConfig::WINDOW_WIDTH - 600) / 2, 655);
    setZValue(500);
    setVisible(false);
}

QRectF BossHPBar::boundingRect() const
{
    constexpr qreal TOTAL_W = 600;
    constexpr qreal ROW_H   = 26;
    return QRectF(0, 0, TOTAL_W, ROW_H);
}

void BossHPBar::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!m_visible) return;

    p->setRenderHint(QPainter::Antialiasing);

    constexpr qreal TOTAL_W = 600;
    constexpr qreal NAME_W  = 160;
    constexpr qreal GAP     = 12;
    constexpr qreal BAR_W   = TOTAL_W - NAME_W - GAP;  // ~428
    constexpr qreal BAR_H   = 22;

    // ── Boss name (within TOTAL_W, left-aligned, centered with bar) ──
    p->setPen(QColor(255, 200, 180));
    QFont nameFont("Microsoft YaHei", 14);
    nameFont.setBold(true);
    p->setFont(nameFont);
    p->drawText(QRectF(0, 0, NAME_W, BAR_H), Qt::AlignLeft | Qt::AlignVCenter, m_bossName);

    // ── HP bar X offset ──
    qreal barX = NAME_W + GAP;

    // ── HP bar background ──
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(20, 20, 20, 200));
    p->drawRoundedRect(QRectF(barX, 0, BAR_W, BAR_H), 6, 6);

    // ── HP fill ──
    qreal ratio = static_cast<qreal>(m_currentHp) / m_maxHp;
    QColor hpColor = (ratio > 0.5) ? QColor(200, 60, 60)
                     : (ratio > 0.25) ? QColor(220, 30, 30)
                     : QColor(240, 10, 10);

    if (ratio > 0.0) {
        p->setBrush(hpColor);
        p->drawRoundedRect(QRectF(barX, 0, BAR_W * ratio, BAR_H), 6, 6);
    }

    // ── Border ──
    p->setPen(QPen(QColor(200, 150, 130, 200), 2));
    p->setBrush(Qt::NoBrush);
    p->drawRoundedRect(QRectF(barX, 0, BAR_W, BAR_H), 6, 6);

    // ── HP text (centered within bar) ──
    p->setPen(Qt::white);
    QFont hpFont("Consolas", 11);
    hpFont.setBold(true);
    p->setFont(hpFont);
    p->drawText(QRectF(barX, 0, BAR_W, BAR_H), Qt::AlignCenter,
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
//  DialogueBox — Galgame-style: speaker name + typewriter text
// ═══════════════════════════════════════════════════════════

DialogueBox::DialogueBox(QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
{
    constexpr qreal BOX_X = 50;
    constexpr qreal BOX_Y = 510;
    constexpr qreal BOX_W = GameConfig::WINDOW_WIDTH - 100;
    constexpr qreal BOX_H = 175;

    setRect(BOX_X, BOX_Y, BOX_W, BOX_H);
    setBrush(QColor(0, 0, 0, 140));
    setPen(QPen(Qt::white, 1));
    setZValue(200);

    // ── Speaker name (top-left, gold color, bold) ──
    m_speakerItem = new QGraphicsTextItem(this);
    m_speakerItem->setDefaultTextColor(QColor(255, 215, 0));  // gold
    QFont speakerFont("Microsoft YaHei", 17);
    speakerFont.setBold(true);
    m_speakerItem->setFont(speakerFont);
    m_speakerItem->setPos(BOX_X + 25, BOX_Y + 18);
    m_speakerItem->setTextWidth(BOX_W - 50);
    m_speakerItem->setZValue(201);
    m_speakerItem->hide();

    // ── Dialogue text (below speaker, white, main area) ──
    m_textItem = new QGraphicsTextItem(this);
    m_textItem->setDefaultTextColor(QColor(255, 255, 255));
    m_textItem->setFont(QFont("Microsoft YaHei", 18));
    m_textItem->setPos(BOX_X + 25, BOX_Y + 52);
    m_textItem->setTextWidth(BOX_W - 50);
    m_textItem->setZValue(201);

    // ── "Press Space to continue" indicator ──
    m_nextHint = new QGraphicsTextItem(this);
    m_nextHint->setDefaultTextColor(QColor(200, 200, 200));
    m_nextHint->setFont(QFont("Microsoft YaHei", 12));
    m_nextHint->setPos(BOX_X + BOX_W - 170, BOX_Y + BOX_H - 30);
    m_nextHint->setZValue(201);
    m_nextHint->setPlainText(QStringLiteral("【空格/点击 继续】"));
    m_nextHint->hide();
}

void DialogueBox::setSpeaker(const QString& speaker)
{
    if (speaker.isEmpty()) {
        m_speakerItem->hide();
    } else {
        m_speakerItem->setPlainText(speaker);
        m_speakerItem->show();
    }
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
    m_speakerItem->setVisible(false);
}
