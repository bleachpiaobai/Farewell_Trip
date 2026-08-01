#include "SpriteAnimation.h"

SpriteAnimation::SpriteAnimation(QObject* parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, &SpriteAnimation::onTimeout);
}

void SpriteAnimation::addFrame(const QPixmap& pixmap)
{
    m_frames.append(pixmap);
}

void SpriteAnimation::setFrames(const QList<QPixmap>& frames)
{
    m_frames = frames;
    m_currentIndex = 0;
}

void SpriteAnimation::setFrameDuration(int msPerFrame)
{
    m_timer->setInterval(msPerFrame);
}

void SpriteAnimation::start()
{
    if (m_frames.isEmpty()) return;
    m_running = true;
    m_currentIndex = 0;
    m_timer->start();
}

void SpriteAnimation::stop()
{
    m_running = false;
    m_timer->stop();
}

void SpriteAnimation::restart()
{
    stop();
    start();
}

bool SpriteAnimation::isRunning() const
{
    return m_running && m_timer->isActive();
}

const QPixmap& SpriteAnimation::currentFrame() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_frames.size())
        return m_frames[m_currentIndex];
    return m_emptyFrame;
}

void SpriteAnimation::setLooping(bool looping)
{
    m_looping = looping;
}

void SpriteAnimation::onTimeout()
{
    if (m_frames.isEmpty()) return;

    // Non-looping: stop at last frame
    if (!m_looping && m_currentIndex >= m_frames.size() - 1) {
        m_running = false;
        m_timer->stop();
        emit finished();
        return;
    }

    m_currentIndex = (m_currentIndex + 1) % m_frames.size();
    emit frameChanged(m_currentIndex);
}
