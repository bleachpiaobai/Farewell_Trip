#include "CutsceneManager.h"
#include "core/EventBus.h"

CutsceneManager::CutsceneManager(EventBus* bus, QObject* parent)
    : QObject(parent)
    , m_bus(bus)
{
}

void CutsceneManager::playCutscene(const QString& name, const QStringList& lines)
{
    Q_UNUSED(lines);
    m_playing = true;
    m_currentName = name;
    // Cutscenes are handled by the DialogueManager — this is a lightweight wrapper
    // for future video/cutscene expansion
}

void CutsceneManager::skip()
{
    if (m_playing) {
        m_playing = false;
        emit cutsceneFinished(m_currentName);
    }
}
