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

    /// 跳过当前视频（仅 ending0 允许跳过）
    void skip();

    /// 当前视频是否允许跳过
    bool skippable() const { return m_skippable; }
    void setSkippable(bool s) { m_skippable = s; }

    /// 是否正在播放视频
    bool isPlaying() const;

    /// 公开清理（章节在检测到视频结束后立即调用，消除闪烁）
    void cleanup();

signals:
    void cutsceneStarted(const QString& name);
    void cutsceneFinished(const QString& name);

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlaybackError(QMediaPlayer::Error error, const QString& errorString);

private:
    GameScene*          m_scene       = nullptr;
    QMediaPlayer*       m_player      = nullptr;
    QAudioOutput*       m_audioOutput = nullptr;
    QGraphicsVideoItem* m_videoItem   = nullptr;

    bool    m_playing      = false;
    bool    m_skippable     = true;   // 默认可跳过
    QString m_currentName;
};

#endif // CUTSCENEMANAGER_H
