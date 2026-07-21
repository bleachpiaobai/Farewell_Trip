#include "MainMenu.h"
#include "core/GameConfig.h"

#include <QPushButton>
#include <QPainter>
#include <QResizeEvent>
#include <QApplication>

MainMenu::MainMenu(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("MainMenu");
    setFixedSize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

    // Load menu wallpaper
    m_bgPixmap.load(":/images/menu/menu.png");

    // ── Start button ──
    m_btnStart = new QPushButton(QStringLiteral("开始游戏"), this);
    m_btnStart->setObjectName("btnStart");
    m_btnStart->move(100, 540);
    m_btnStart->resize(220, 56);

    connect(m_btnStart, &QPushButton::clicked, this, &MainMenu::startGameRequested);

    // ── Quit button ──
    m_btnQuit = new QPushButton(QStringLiteral("退出游戏"), this);
    m_btnQuit->setObjectName("btnQuit");
    m_btnQuit->move(100, 612);
    m_btnQuit->resize(220, 56);

    connect(m_btnQuit, &QPushButton::clicked, this, &MainMenu::quitRequested);
}

void MainMenu::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    // Draw background
    if (!m_bgPixmap.isNull()) {
        QPixmap scaled = m_bgPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding,
                                           Qt::SmoothTransformation);
        int x = (scaled.width() - width()) / 2;
        int y = (scaled.height() - height()) / 2;
        painter.drawPixmap(-x, -y, scaled);
    } else {
        painter.fillRect(rect(), QColor(10, 10, 26));
    }
}

void MainMenu::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}
