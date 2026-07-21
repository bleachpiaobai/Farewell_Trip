#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>

class QPushButton;

class MainMenu : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget* parent = nullptr);

signals:
    void startGameRequested();
    void quitRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QPixmap      m_bgPixmap;
    QPushButton* m_btnStart = nullptr;
    QPushButton* m_btnQuit  = nullptr;
};

#endif // MAINMENU_H
