#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class GameView;
class MainMenu;
class QStackedWidget;

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

signals:
    void gameStarted();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupUi();

    QStackedWidget* m_stack = nullptr;
    MainMenu*       m_menu = nullptr;
    GameView*       m_gameView = nullptr;
};

#endif // MAINWINDOW_H
