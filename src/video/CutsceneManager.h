#ifndef CUTSCENEMANAGER_H
#define CUTSCENEMANAGER_H

#include <QObject>
#include <QStringList>

class EventBus;

class CutsceneManager : public QObject
{
    Q_OBJECT
public:
    explicit CutsceneManager(EventBus* bus, QObject* parent = nullptr);

    void playCutscene(const QString& name, const QStringList& lines);
    void skip();
    bool isPlaying() const { return m_playing; }

signals:
    void cutsceneLine(const QString& text);
    void cutsceneFinished(const QString& name);

private:
    EventBus* m_bus;
    bool m_playing = false;
    QString m_currentName;
};

#endif // CUTSCENEMANAGER_H
