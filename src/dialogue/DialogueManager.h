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
    QString currentSpeaker() const { return m_speaker; }
    QString currentDisplayText() const { return m_displayText; }
    QString currentFullText() const { return m_fullText; }
    bool isNarration() const { return m_isNarration; }
    int currentIndex() const { return m_curIndex; }
    int totalLines() const { return m_lines.size(); }

    // ── Control ──
    void start();
    void next();      // Skip-to-end or advance to next line
    void skipAll();   // Instantly finish current segment

public slots:
    void onTick();    // Called each frame for typewriter effect

signals:
    void speakerChanged(const QString& speaker);  // emitted immediately on line advance
    void textChanged(const QString& displayText);  // types out character by character
    void segmentFinished();   // All lines in current segment done
    void lineAdvanced(int index, int total);

private:
    /// Split a script line into (speaker, dialogue) pair.
    /// Returns true if this is a spoken line (has speaker), false if narration.
    struct ParsedLine {
        QString speaker;   // empty for narration
        QString dialogue;  // the text to type out
        bool isNarration;  // true → show instantly, no typewriter
    };
    ParsedLine parseLine(const QString& raw) const;

    EventBus* m_bus;
    QStringList m_lines;
    QString m_displayText;
    QString m_fullText;      // full dialogue text for current line
    QString m_speaker;       // current speaker (empty for narration)
    bool m_isNarration = false;
    int m_curIndex = 0;
    int m_charTimer = 0;
    bool m_active = false;
    bool m_over = false;

    static constexpr int CHAR_DELAY = 3;  // frames per character
};

#endif // DIALOGUEMANAGER_H
