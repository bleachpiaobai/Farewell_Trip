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
    int  m_enemyAttackTimer = 0;
    int  m_enemyMoveTimer   = 0;       // 移动决策计时器
    int  m_enemyMoveCooldown = 0;      // 移动后冷却帧数
    int  m_contactDamageTimer = 0;     // 接触伤害计时器
    bool m_bossAttacking    = false;   // Boss 正在攻击动画中
    int  m_bossAttackAnimTimer = 0;    // 攻击动画剩余帧数
    static constexpr int BOSS_MOVE_INTERVAL = 10;    // 移动决策间隔（帧）
    static constexpr int BOSS_MOVE_COOLDOWN = 5;     // 移动后冷却
    static constexpr int BOSS_ATTACK_RANGE  = 120;   // 攻击距离（须面向玩家）
    static constexpr int BOSS_CHASE_RANGE   = 2000;  // 开始追逐的距离（覆盖全屏）
    static constexpr int BOSS_APPROACH_SPEED = 5;    // 接近速度（像素/帧）
    static constexpr int CONTACT_DAMAGE_RANGE = 80;  // 接触伤害距离
    static constexpr int CONTACT_DAMAGE_INTERVAL = 45; // 接触伤害间隔（帧）
};

#endif // COMBATSYSTEM_H
