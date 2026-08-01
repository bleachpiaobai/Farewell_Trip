#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QPixmap>

// ── Frame-based sprite animation ───────────────────────

class SpriteAnimation : public QObject
{
    Q_OBJECT
public:
    explicit SpriteAnimation(QObject* parent = nullptr);

    void addFrame(const QPixmap& pixmap);
    void setFrames(const QList<QPixmap>& frames);
    void setFrameDuration(int msPerFrame);

    void start();
    void stop();
    void restart();
    bool isRunning() const;

    void setLooping(bool looping);
    bool isLooping() const { return m_looping; }

    const QPixmap& currentFrame() const;
    int currentIndex() const { return m_currentIndex; }
    int frameCount() const { return m_frames.size(); }

signals:
    void frameChanged(int index);
    void finished();

private slots:
    void onTimeout();

private:
    QList<QPixmap> m_frames;
    QPixmap m_emptyFrame;
    QTimer* m_timer;
    int m_currentIndex = 0;
    bool m_running = false;
    bool m_looping = true;
};

#endif // SPRITEANIMATION_H
