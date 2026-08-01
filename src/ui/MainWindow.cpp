#include "MainWindow.h"
#include "MainMenu.h"
#include "GameView.h"
#include "FailScreen.h"
#include "core/GameConfig.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QApplication>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainterPath>
#include <QBitmap>
#include <QIcon>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Frameless window, draggable
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setupUi();
    setupWindowControls();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    setWindowTitle(GameConfig::WINDOW_TITLE);
    resize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

    // Rounded corner mask (only in windowed mode)
    updateWindowMask();

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    // Layer 0: main menu
    m_menu = new MainMenu(this);
    m_stack->addWidget(m_menu);      // index 0
    m_stack->setCurrentWidget(m_menu);

    // Connect quit
    connect(m_menu, &MainMenu::quitRequested, qApp, &QApplication::quit);
}

void MainWindow::setupWindowControls()
{
    // ── Minimize button ──
    m_btnMinimize = new QPushButton(this);
    m_btnMinimize->setIcon(QIcon(":/images/ui/mini.png"));
    m_btnMinimize->setIconSize(QSize(20, 20));
    m_btnMinimize->setFixedSize(32, 32);
    m_btnMinimize->setFlat(true);
    m_btnMinimize->setCursor(Qt::PointingHandCursor);
    m_btnMinimize->setObjectName("btnMinimize");
    m_btnMinimize->setFocusPolicy(Qt::NoFocus);  // don't steal keys from GameView
    connect(m_btnMinimize, &QPushButton::clicked, this, &QWidget::showMinimized);

    // ── Maximize / Restore button ──
    m_btnMaximize = new QPushButton(this);
    m_btnMaximize->setIcon(QIcon(":/images/ui/max.png"));
    m_btnMaximize->setIconSize(QSize(20, 20));
    m_btnMaximize->setFixedSize(32, 32);
    m_btnMaximize->setFlat(true);
    m_btnMaximize->setCursor(Qt::PointingHandCursor);
    m_btnMaximize->setObjectName("btnMaximize");
    m_btnMaximize->setFocusPolicy(Qt::NoFocus);  // don't steal keys from GameView
    connect(m_btnMaximize, &QPushButton::clicked, this, &MainWindow::toggleFullscreen);

    // ── Close button ──
    m_btnClose = new QPushButton(this);
    m_btnClose->setIcon(QIcon(":/images/ui/close.png"));
    m_btnClose->setIconSize(QSize(20, 20));
    m_btnClose->setFixedSize(32, 32);
    m_btnClose->setFlat(true);
    m_btnClose->setCursor(Qt::PointingHandCursor);
    m_btnClose->setObjectName("btnClose");
    m_btnClose->setFocusPolicy(Qt::NoFocus);  // don't steal keys from GameView
    connect(m_btnClose, &QPushButton::clicked, qApp, &QApplication::quit);

    // Position buttons in top-right corner
    repositionButtons();
}

void MainWindow::repositionButtons()
{
    constexpr int BTN_SIZE  = 32;
    constexpr int PADDING   = 8;
    constexpr int TOP_MARGIN = 8;

    int rightEdge = width();
    int y = TOP_MARGIN;

    // Close  → rightmost
    m_btnClose->move(rightEdge - PADDING - BTN_SIZE, y);
    // Maximize
    m_btnMaximize->move(rightEdge - PADDING - BTN_SIZE * 2 - 4, y);
    // Minimize
    m_btnMinimize->move(rightEdge - PADDING - BTN_SIZE * 3 - 8, y);

    // Ensure buttons stay on top of the stacked widget
    m_btnMinimize->raise();
    m_btnMaximize->raise();
    m_btnClose->raise();
}

void MainWindow::toggleFullscreen()
{
    if (m_isFullscreen) {
        // Restore to windowed mode
        showNormal();
        m_isFullscreen = false;
        updateWindowMask();
        // Re-apply frameless hint (lost after showNormal on some platforms)
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
        resize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
        show();
    } else {
        // Go fullscreen
        m_isFullscreen = true;
        clearMask();  // remove rounded corners
        showFullScreen();
    }
}

void MainWindow::updateWindowMask()
{
    if (m_isFullscreen) return;

    constexpr int RADIUS = 16;
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT),
                        RADIUS, RADIUS);
    setMask(QRegion(path.toFillPolygon().toPolygon()));
}

void MainWindow::setGameView(GameView* view)
{
    m_gameView = view;
    m_stack->addWidget(m_gameView);     // index 1
}

void MainWindow::showMenu()
{
    m_stack->setCurrentWidget(m_menu);
}

void MainWindow::showGame()
{
    m_stack->setCurrentWidget(m_gameView);
    m_gameView->setFocus();
    repositionButtons();
}

void MainWindow::showFailScreen()
{
    // Lazy-create FailScreen (first death)
    if (!m_failScreen) {
        m_failScreen = new FailScreen(this);
        m_failScreen->setFailImage(GameConfig::imagePath(":/images/fail.png"));
        m_stack->addWidget(m_failScreen);  // index 2

        connect(m_failScreen, &FailScreen::returnToMenuRequested, this, [this]() {
            emit returnToMenuRequested();
        });
    }
    m_stack->setCurrentWidget(m_failScreen);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QApplication::quit();
    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    repositionButtons();
}

// ── Frameless window mouse drag ──

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragStartPos);
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}
