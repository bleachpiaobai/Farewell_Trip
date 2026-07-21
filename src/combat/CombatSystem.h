#ifndef COMBATSYSTEM_H
#define COMBATSYSTEM_H

#include <QObject>
#include "core/GameConfig.h"

class Player;
class Enemy;
class EventBus;

class CombatSystem : public QObject
{
    Q_OBJECT
public:
    explicit CombatSystem(EventBus* bus, QObject* parent = nullptr);

    void setPlayer(Player* player) { m_player = player; }
    void setCurrentBoss(Enemy* boss) { m_currentBoss = boss; }

    Player* player() const { return m_player; }
    Enemy* currentBoss() const { return m_currentBoss; }

    bool bossAlive() const;
    bool inCombat() const { return m_inCombat; }

    void startCombat(Enemy* boss);
    void endCombat();

public slots:
    void onTick();              // Check attack range & apply damage each frame
    void onPlayerAttack();      // Player pressed J

signals:
    void bossSpawned(Enemy* boss);
    void bossDefeated(const QString& name);
    void combatEnded();

private:
    EventBus* m_bus;
    Player* m_player = nullptr;
    Enemy* m_currentBoss = nullptr;
    bool m_inCombat = false;
};

#endif // COMBATSYSTEM_H
