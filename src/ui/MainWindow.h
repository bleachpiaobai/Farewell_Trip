#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class GameView;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setGameView(GameView* view);
    QStackedWidget* stack() const { return m_stack; }

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupUi();

    QStackedWidget* m_stack = nullptr;
    GameView*       m_gameView = nullptr;
};

#endif // MAINWINDOW_H
