#include "CutsceneManager.h"
#include "ui/GameScene.h"

#include <QGraphicsVideoItem>
#include <QAudioOutput>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>

CutsceneManager::CutsceneManager(GameScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
{
    // ── 媒体播放器 ──
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(1.0);

    // ── 视频渲染 item（预先创建，按需显示） ──
    m_videoItem = new QGraphicsVideoItem();
    m_videoItem->setSize(QSizeF(1280, 720));
    m_videoItem->setZValue(9000);          // 高于所有 UI，低于 TransitionEffect (10000)
    m_videoItem->hide();

    m_scene->addItem(m_videoItem);
    m_player->setVideoOutput(m_videoItem);

    // ── 信号连接 ──
    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, &CutsceneManager::onMediaStatusChanged);
    connect(m_player, &QMediaPlayer::errorOccurred,
            this, &CutsceneManager::onPlaybackError);
}

CutsceneManager::~CutsceneManager()
{
    if (m_videoItem) {
        m_scene->removeItem(m_videoItem);
    }
}

// ═══════════════════════════════════════════════════════════════
//  公共 API
// ═══════════════════════════════════════════════════════════════

void CutsceneManager::playCutscene(const QString& name, const QString& videoPath)
{
    // If already playing, stop first
    if (m_playing) {
        m_player->stop();
    }

    m_currentName = name;

    // ── Resolve absolute path ──
    // CMake copies videos to the build directory relative to the executable
    QString absPath = QDir(QCoreApplication::applicationDirPath())
                          .absoluteFilePath(videoPath);

    // Fallback: relative to current working directory (Qt Creator compatibility)
    if (!QFile::exists(absPath)) {
        absPath = QDir::currentPath() + "/" + videoPath;
    }

    if (!QFile::exists(absPath)) {
        qWarning() << "[Cutscene] Video file not found:" << absPath
                   << "-- skipping cutscene" << name;
        emit cutsceneFinished(m_currentName);
        return;
    }

    qDebug() << "[Cutscene] Playing:" << name << "->" << absPath;

    // ── Show video item and play ──
    m_videoItem->show();
    m_player->setSource(QUrl::fromLocalFile(absPath));
    m_player->play();

    m_playing = true;

    emit cutsceneStarted(m_currentName);
}

void CutsceneManager::skip()
{
    if (!m_playing) return;

    qDebug() << "[Cutscene] Skipped:" << m_currentName;
    m_player->stop();
    cleanup();
    emit cutsceneFinished(m_currentName);
}

bool CutsceneManager::isPlaying() const
{
    return m_playing;
}

// ═══════════════════════════════════════════════════════════════
//  媒体回调
// ═══════════════════════════════════════════════════════════════

void CutsceneManager::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::EndOfMedia:
        qDebug() << "[Cutscene] Ended naturally:" << m_currentName;
        cleanup();
        emit cutsceneFinished(m_currentName);
        break;

    case QMediaPlayer::InvalidMedia:
        qWarning() << "[Cutscene] Invalid media:" << m_currentName;
        cleanup();
        emit cutsceneFinished(m_currentName);   // 失败即放行
        break;

    default:
        break;
    }
}

void CutsceneManager::onPlaybackError(QMediaPlayer::Error error, const QString& errorString)
{
    Q_UNUSED(error);
    qWarning() << "[Cutscene] Playback error:" << errorString;
    cleanup();
    emit cutsceneFinished(m_currentName);       // 失败即放行
}

// ═══════════════════════════════════════════════════════════════
//  内部辅助
// ═══════════════════════════════════════════════════════════════

void CutsceneManager::cleanup()
{
    m_player->stop();
    m_videoItem->hide();
    m_playing = false;
}
