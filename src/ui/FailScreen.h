#ifndef FAILSCREEN_H
#define FAILSCREEN_H

#include <QWidget>
#include <QPixmap>

class QPushButton;

// ── 失败画面：玩家死亡时显示 ──────────────────────────

class FailScreen : public QWidget
{
    Q_OBJECT
public:
    explicit FailScreen(QWidget* parent = nullptr);

    void setFailImage(const QString& qrcPath);

signals:
    void returnToMenuRequested();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap      m_failPixmap;
    QPushButton* m_btnReturn = nullptr;
};

#endif // FAILSCREEN_H
