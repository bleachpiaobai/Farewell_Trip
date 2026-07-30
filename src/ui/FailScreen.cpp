#include "FailScreen.h"
#include "core/GameConfig.h"

#include <QPushButton>
#include <QPainter>

FailScreen::FailScreen(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("FailScreen");
    setFixedSize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

    // ── 返回主菜单按钮（放在左下角） ──
    m_btnReturn = new QPushButton(QStringLiteral("返回主菜单"), this);
    m_btnReturn->setObjectName("btnReturn");
    m_btnReturn->move(40, GameConfig::WINDOW_HEIGHT - 100);
    m_btnReturn->resize(220, 56);

    connect(m_btnReturn, &QPushButton::clicked,
            this, &FailScreen::returnToMenuRequested);
}

void FailScreen::setFailImage(const QString& qrcPath)
{
    m_failPixmap.load(qrcPath);
    update();
}

void FailScreen::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // ── 失败图片铺满整个画面 ──
    if (!m_failPixmap.isNull()) {
        QPixmap scaled = m_failPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding,
                                             Qt::SmoothTransformation);
        // 居中裁剪
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;
        painter.drawPixmap(x, y, scaled);
    } else {
        // Fallback: 暗色半透明遮罩
        painter.fillRect(rect(), QColor(0, 0, 0, 180));
    }
}
