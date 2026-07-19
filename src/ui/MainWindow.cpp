#include "MainWindow.h"
#include "GameView.h"
#include "core/GameConfig.h"

#include <QStackedWidget>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    setWindowTitle(GameConfig::WINDOW_TITLE);
    setFixedSize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    // Layer 0: main menu (placeholder for future)
    // Layer 1: game view (set by GameEngine via setGameView)
}

void MainWindow::setGameView(GameView* view)
{
    m_gameView = view;
    m_stack->addWidget(m_gameView);     // index at position
    m_stack->setCurrentWidget(m_gameView);
    m_gameView->setFocus();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QApplication::quit();
    QMainWindow::closeEvent(event);
}
