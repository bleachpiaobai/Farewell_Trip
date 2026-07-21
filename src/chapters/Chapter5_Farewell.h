#pragma once
#include "chapters/ChapterBase.h"
#include <QTimer>
#include <QStringList>

// ╔══════════════════════════════════════════════════════════╗
// ║  第五章  少女！告别银河旅行                                ║
// ║  终章诗歌 —— 游戏完结画面                                  ║
// ╚══════════════════════════════════════════════════════════╝

class Chapter5_Farewell : public ChapterBase {
    Q_OBJECT
public:
    explicit Chapter5_Farewell(QObject* parent = nullptr);

    void onEnter() override;
    void onExit() override;
    void update() override;
    bool isComplete() const override { return false; }  // 终章不结束
    ChapterInfo currentInfo() const override;

private slots:
    void onSlideshowTick();

private:
    bool m_started = false;
    QTimer* m_slideshowTimer = nullptr;
    QStringList m_slides;
    int m_slideIndex = 0;
};
