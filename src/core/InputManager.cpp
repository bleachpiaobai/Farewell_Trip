#include "InputManager.h"
#include <QKeyEvent>

InputManager::InputManager(QObject* parent)
    : QObject(parent)
{
}

bool InputManager::isKeyDown(int key) const
{
    return m_heldKeys.contains(key);
}

bool InputManager::isKeyPressed(int key) const
{
    return m_pressedThisFrame.contains(key);
}

bool InputManager::isKeyReleased(int key) const
{
    return m_releasedThisFrame.contains(key);
}

bool InputManager::moveLeft() const
{
    return isKeyDown(Qt::Key_A) || isKeyDown(Qt::Key_Left);
}

bool InputManager::moveRight() const
{
    return isKeyDown(Qt::Key_D) || isKeyDown(Qt::Key_Right);
}

bool InputManager::attackPressed() const
{
    return isKeyPressed(Qt::Key_J) || isKeyPressed(Qt::Key_Space);
}

bool InputManager::advancePressed() const
{
    return isKeyPressed(Qt::Key_Space) || m_mouseClicked;
}

bool InputManager::mouseClicked() const
{
    return m_mouseClicked;
}

void InputManager::handleKeyPress(int key)
{
    if (!m_heldKeys.contains(key)) {
        m_pressedThisFrame.insert(key);
        emit keyPressed(key);
        if (key == Qt::Key_Space)
            emit spacePressed();
        if (key == Qt::Key_J)
            emit attackPressedSignal();
    }
    m_heldKeys.insert(key);
}

void InputManager::handleKeyRelease(int key)
{
    m_heldKeys.remove(key);
    m_releasedThisFrame.insert(key);
    emit keyReleased(key);
}

void InputManager::handleMousePress(Qt::MouseButton button, const QPointF& pos)
{
    if (button == Qt::LeftButton) {
        m_mouseClicked = true;
        m_mousePos = pos;
        emit mousePressed(pos);
    }
}

void InputManager::endFrame()
{
    m_pressedThisFrame.clear();
    m_releasedThisFrame.clear();
    m_mouseClicked = false;
}
