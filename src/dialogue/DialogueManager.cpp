#include "DialogueManager.h"
#include "core/EventBus.h"

DialogueManager::DialogueManager(EventBus* bus, QObject* parent)
    : QObject(parent)
    , m_bus(bus)
{
}

void DialogueManager::loadScript(const QStringList& lines)
{
    m_lines = lines;
    m_curIndex = 0;
    m_displayText.clear();
    m_charTimer = 0;
    m_active = true;
    m_over = false;
    emit textChanged(m_displayText);
}

void DialogueManager::clear()
{
    m_lines.clear();
    m_curIndex = 0;
    m_displayText.clear();
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

    // If current line not fully displayed, show it all
    if (m_displayText.length() < m_lines[m_curIndex].length()) {
        m_displayText = m_lines[m_curIndex];
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
        emit textChanged(m_displayText);
    }
}

void DialogueManager::skipAll()
{
    if (m_lines.isEmpty()) return;
    m_curIndex = m_lines.size();
    m_displayText = m_lines.last();
    m_over = true;
    m_active = false;
    emit textChanged(m_displayText);
    emit segmentFinished();
}

void DialogueManager::onTick()
{
    if (!m_active || m_over) return;
    if (m_curIndex >= m_lines.size()) return;

    m_charTimer++;
    if (m_charTimer >= CHAR_DELAY) {
        m_charTimer = 0;
        if (m_displayText.length() < m_lines[m_curIndex].length()) {
            m_displayText += m_lines[m_curIndex][m_displayText.length()];
            emit textChanged(m_displayText);
        }
    }
}
