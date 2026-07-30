#include "DialogueManager.h"
#include "core/EventBus.h"

DialogueManager::DialogueManager(EventBus* bus, QObject* parent)
    : QObject(parent)
    , m_bus(bus)
{
}

DialogueManager::ParsedLine DialogueManager::parseLine(const QString& raw) const
{
    ParsedLine result;
    if (raw.isEmpty()) {
        result.speaker.clear();
        result.dialogue.clear();
        result.isNarration = true;
        return result;
    }

    // Narration: lines that start with （ or ( are stage directions
    if (raw.startsWith(QStringLiteral("（")) || raw.startsWith('(')) {
        result.speaker.clear();
        result.dialogue = raw;
        result.isNarration = true;
        return result;
    }

    // Spoken dialogue: split at the first fullwidth colon ：
    int colonPos = raw.indexOf(QStringLiteral("："));
    if (colonPos > 0) {
        result.speaker = raw.left(colonPos);
        result.dialogue = raw.mid(colonPos + 1);
        result.isNarration = false;
        return result;
    }

    // Fallback: no speaker found, treat as narration
    result.speaker.clear();
    result.dialogue = raw;
    result.isNarration = true;
    return result;
}

void DialogueManager::loadScript(const QStringList& lines)
{
    m_lines = lines;
    m_curIndex = 0;
    m_displayText.clear();
    m_fullText.clear();
    m_speaker.clear();
    m_isNarration = false;
    m_charTimer = 0;
    m_active = true;
    m_over = false;

    // Immediately show the first line's speaker
    if (!m_lines.isEmpty()) {
        auto parsed = parseLine(m_lines[0]);
        m_speaker = parsed.speaker;
        m_fullText = parsed.dialogue;
        m_isNarration = parsed.isNarration;
        emit speakerChanged(m_speaker);
    }
    emit textChanged(m_displayText);
}

void DialogueManager::clear()
{
    m_lines.clear();
    m_curIndex = 0;
    m_displayText.clear();
    m_fullText.clear();
    m_speaker.clear();
    m_isNarration = false;
    m_charTimer = 0;
    m_active = false;
    m_over = true;
    emit textChanged(m_displayText);
}

void DialogueManager::start()
{
    if (m_lines.isEmpty()) {
        m_over = true;
        m_active = false;
        emit segmentFinished();
        return;
    }
    m_active = true;
    m_over = false;
    m_curIndex = 0;
    m_displayText.clear();
    m_charTimer = 0;

    auto parsed = parseLine(m_lines[0]);
    m_speaker = parsed.speaker;
    m_fullText = parsed.dialogue;
    m_isNarration = parsed.isNarration;
    emit speakerChanged(m_speaker);
    emit textChanged(m_displayText);
}

void DialogueManager::next()
{
    if (m_lines.isEmpty() || m_curIndex >= m_lines.size()) {
        m_over = true;
        m_active = false;
        emit segmentFinished();
        return;
    }

    // If current line not fully displayed, show it all instantly
    if (m_displayText.length() < m_fullText.length()) {
        m_displayText = m_fullText;
        emit textChanged(m_displayText);
        return;
    }

    // Advance to next line
    m_curIndex++;
    m_displayText.clear();
    m_charTimer = 0;
    emit lineAdvanced(m_curIndex, m_lines.size());

    if (m_curIndex >= m_lines.size()) {
        m_over = true;
        m_active = false;
        emit segmentFinished();
    } else {
        auto parsed = parseLine(m_lines[m_curIndex]);
        m_speaker = parsed.speaker;
        m_fullText = parsed.dialogue;
        m_isNarration = parsed.isNarration;
        emit speakerChanged(m_speaker);
        emit textChanged(m_displayText);
    }
}

void DialogueManager::skipAll()
{
    if (m_lines.isEmpty()) return;
    m_curIndex = m_lines.size();
    auto parsed = parseLine(m_lines.last());
    m_displayText = parsed.dialogue;
    m_speaker = parsed.speaker;
    m_isNarration = parsed.isNarration;
    m_over = true;
    m_active = false;
    emit speakerChanged(m_speaker);
    emit textChanged(m_displayText);
    emit segmentFinished();
}

void DialogueManager::onTick()
{
    if (!m_active || m_over) return;
    if (m_curIndex >= m_lines.size()) return;

    // Narration lines appear instantly (no typewriter)
    if (m_isNarration) {
        if (m_displayText.isEmpty()) {
            m_displayText = m_fullText;
            emit textChanged(m_displayText);
        }
        return;
    }

    // Typewriter effect for spoken dialogue
    m_charTimer++;
    if (m_charTimer >= CHAR_DELAY) {
        m_charTimer = 0;
        if (m_displayText.length() < m_fullText.length()) {
            m_displayText += m_fullText[m_displayText.length()];
            emit textChanged(m_displayText);
        }
    }
}
