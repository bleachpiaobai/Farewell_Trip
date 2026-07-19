#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QObject>
#include <QSet>
#include <QPointF>

class InputManager : public QObject
{
    Q_OBJECT
public:
    explicit InputManager(QObject* parent = nullptr);

    // ── Key state ──
    bool isKeyDown(int key) const;
    bool isKeyPressed(int key) const;   // true only on the frame it was pressed
    bool isKeyReleased(int key) const;  // true only on the frame it was released

    // ── Convenience queries ──
    bool moveLeft() const;
    bool moveRight() const;
    bool attackPressed() const;
    bool advancePressed() const;   // Space or click

    // ── Mouse ──
    bool mouseClicked() const;
    QPointF mousePos() const { return m_mousePos; }

    // ── Called by MainWindow ──
    void handleKeyPress(int key);
    void handleKeyRelease(int key);
    void handleMousePress(Qt::MouseButton button, const QPointF& pos);

    // ── Called each frame to reset edge-triggered state ──
    void endFrame();

signals:
    void keyPressed(int key);
    void keyReleased(int key);
    void spacePressed();
    void attackPressedSignal();
    void mousePressed(const QPointF& pos);

private:
    QSet<int> m_heldKeys;
    QSet<int> m_pressedThisFrame;
    QSet<int> m_releasedThisFrame;
    bool m_mouseClicked = false;
    QPointF m_mousePos;
};

#endif // INPUTMANAGER_H
