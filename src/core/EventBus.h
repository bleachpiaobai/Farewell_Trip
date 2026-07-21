#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <QObject>

/// Cross-system signal/slot communication.
/// Subsystems connect to these signals in GameEngine's constructor.
class EventBus : public QObject
{
    Q_OBJECT
public:
    explicit EventBus(QObject* parent = nullptr) : QObject(parent) {}

signals:
    // Dialogue → Scene
    void dialogueFinished(int sceneId);
    void commandTriggered(const QString& command, const QString& arg);

    // Combat → UI
    void bossHPChanged(const QString& bossName, int currentHP, int maxHP);
    void bossDefeated(const QString& bossName);
    void playerHPChanged(int currentHP, int maxHP);

    // Scene → Engine
    void sceneComplete(int sceneId);

    // Cutscene → Engine
    void cutsceneFinished(const QString& name);

    // UI → Engine
    void pauseRequested();
    void resumeRequested();
    void newGameRequested();
    void quitRequested();
};

#endif // EVENTBUS_H
