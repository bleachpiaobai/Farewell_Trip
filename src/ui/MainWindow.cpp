#include "MainWindow.h"
#include "MainMenu.h"
#include "GameView.h"
#include "core/GameConfig.h"

#include <QStackedWidget>
#include <QApplication>
#include <QCloseEvent>

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

    // Layer 0: main menu
    m_menu = new MainMenu(this);
    m_stack->addWidget(m_menu);      // index 0
    m_stack->setCurrentWidget(m_menu);

    // Connect quit
    connect(m_menu, &MainMenu::quitRequested, qApp, &QApplication::quit);
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
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QApplication::quit();
    QMainWindow::closeEvent(event);
}
