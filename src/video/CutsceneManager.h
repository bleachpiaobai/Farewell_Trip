#ifndef CUTSCENEMANAGER_H
#define CUTSCENEMANAGER_H

#include <QObject>
#include <QMediaPlayer>

class GameScene;
class QGraphicsVideoItem;
class QAudioOutput;

// ╔══════════════════════════════════════════════════════════════╗
// ║  CutsceneManager — 过场视频管理器                            ║
// ║                                                              ║
// ║  使用 QGraphicsVideoItem 在 GameScene 中渲染视频：            ║
// ║    视频 item 放置于 Z=9000（高于所有 UI，低于过渡效果）       ║
// ║    播放时阻塞游戏输入（通过 CUTSCENE 状态）                   ║
// ║    跳过（空格/鼠标）在 GameEngine::processInput 中处理        ║
// ║                                                              ║
// ║  用法：                                                       ║
// ║    m_cutscene->playCutscene("YAN_Ack",                       ║
// ║                             "videos/ch04_survivor/YAN_Ack.mp4");║
// ╚══════════════════════════════════════════════════════════════╝

class CutsceneManager : public QObject
{
    Q_OBJECT
public:
    explicit CutsceneManager(GameScene* scene, QObject* parent = nullptr);
    ~CutsceneManager() override;

    /// 播放视频 — name 仅用于日志/signal，videoPath 是相对于可执行目录的路径
    void playCutscene(const QString& name, const QString& videoPath);

    /// 跳过当前视频（等同于播放结束）
    void skip();

    /// 是否正在播放视频
    bool isPlaying() const;

signals:
    void cutsceneStarted(const QString& name);
    void cutsceneFinished(const QString& name);

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlaybackError(QMediaPlayer::Error error, const QString& errorString);

private:
    void cleanup();               // 停止播放、隐藏视频 item

    GameScene*          m_scene       = nullptr;
    QMediaPlayer*       m_player      = nullptr;
    QAudioOutput*       m_audioOutput = nullptr;
    QGraphicsVideoItem* m_videoItem   = nullptr;

    bool    m_playing      = false;
    QString m_currentName;
};

#endif // CUTSCENEMANAGER_H
