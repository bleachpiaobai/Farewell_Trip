#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class GameView;
class MainMenu;
class FailScreen;
class QStackedWidget;
class QPushButton;
class QMouseEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setGameView(GameView* view);
    QStackedWidget* stack() const { return m_stack; }
    MainMenu* menuWidget() const { return m_menu; }

    void showMenu();
    void showGame();
    void showFailScreen();

signals:
    void gameStarted();

protected:
    void closeEvent(QCloseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void toggleFullscreen();

private:
    void setupUi();
    void setupWindowControls();
    void updateWindowMask();
    void repositionButtons();

    QStackedWidget* m_stack = nullptr;
    MainMenu*       m_menu = nullptr;
    GameView*       m_gameView = nullptr;
    FailScreen*     m_failScreen = nullptr;

    // Window control buttons (top-right corner, always visible)
    QPushButton* m_btnMinimize = nullptr;
    QPushButton* m_btnMaximize = nullptr;
    QPushButton* m_btnClose    = nullptr;

    // Frameless window dragging
    bool    m_dragging    = false;
    QPoint  m_dragStartPos;

    // Fullscreen state
    bool    m_isFullscreen = false;
};

#endif // MAINWINDOW_H
