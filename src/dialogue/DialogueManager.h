#ifndef DIALOGUEMANAGER_H
#define DIALOGUEMANAGER_H

#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QElapsedTimer>

class EventBus;

class DialogueManager : public QObject
{
    Q_OBJECT
public:
    explicit DialogueManager(EventBus* bus, QObject* parent = nullptr);

    // ── Content ──
    void loadScript(const QStringList& lines);
    void clear();

    // ── State ──
    bool isActive() const { return m_active; }
    bool isOver() const { return m_over; }
    QString currentDisplayText() const { return m_displayText; }
    int currentIndex() const { return m_curIndex; }
    int totalLines() const { return m_lines.size(); }

    // ── Control ──
    void start();
    void next();      // Skip-to-end or advance to next line
    void skipAll();   // Instantly finish current segment

public slots:
    void onTick();    // Called each frame for typewriter effect

signals:
    void textChanged(const QString& displayText);
    void segmentFinished();   // All lines in current segment done
    void lineAdvanced(int index, int total);

private:
    EventBus* m_bus;
    QStringList m_lines;
    QString m_displayText;
    int m_curIndex = 0;
    int m_charTimer = 0;
    bool m_active = false;
    bool m_over = false;

    static constexpr int CHAR_DELAY = 3;  // frames per character
};

#endif // DIALOGUEMANAGER_H
