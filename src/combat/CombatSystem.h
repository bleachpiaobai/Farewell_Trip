#ifndef COMBATSYSTEM_H
#define COMBATSYSTEM_H

#include <QObject>
#include <QList>
#include <QGraphicsScene>
#include "core/GameConfig.h"

class Player;
class Enemy;
class Projectile;
class Laser;
class EventBus;

class CombatSystem : public QObject
{
    Q_OBJECT
public:
    explicit CombatSystem(EventBus* bus, QObject* parent = nullptr);

    void setPlayer(Player* player) { m_player = player; }
    void setScene(QGraphicsScene* scene) { m_scene = scene; }

    Player* player() const { return m_player; }
    bool inCombat() const { return m_inCombat; }

    void startCombat(Enemy* boss);
    void endCombat();

public slots:
    void onTick();
    void onPlayerAttack();

signals:
    void bossSpawned(Enemy* boss);
    void bossDefeated(const QString& name);
    void combatEnded();

private:
    // Boss 弹幕
    void spawnBossBullet(const QColor& color, qreal radius, int damage,
                         qreal vx, qreal vy, qreal spawnY,
                         qreal gravity = 0.0, int lifetime = -1,
                         bool bouncing = false);
    void updateBossProjectiles();
    void clearAllBossProjectiles();

    // 玩家红色光球
    void spawnPlayerFireball();
    void updatePlayerProjectiles();
    void clearAllPlayerProjectiles();

    // 激光
    void spawnLaser(qreal x);
    void spawnLaserPattern(int count);
    void updateLasers();
    void clearAllLasers();

    void tickPeachPatterns();
    void tickXIAPatterns();

    QGraphicsScene* m_scene = nullptr;
    EventBus* m_bus;
    Player* m_player = nullptr;
    Enemy* m_currentBoss = nullptr;
    bool m_inCombat = false;
    int  m_enemyAttackTimer = 0;
    int  m_enemyMoveTimer   = 0;
    int  m_enemyMoveCooldown = 0;
    int  m_contactDamageTimer = 0;
    bool m_bossAttacking    = false;
    int  m_bossAttackAnimTimer = 0;

    QList<Projectile*> m_bossProjectiles;
    int m_bulletTimer = 0;

    QList<Projectile*> m_playerProjectiles;

    QList<Laser*> m_lasers;
    int m_laserTimer = 0;

    bool m_barrageTriggered = false;
    int  m_barrageWave = 0;
    int  m_barrageTimer = 0;

    // 弹幕固定高度：地面层，站立命中，跳跃可躲
    static constexpr qreal BULLET_GROUND_Y = 530.0;

    static constexpr int BOSS_MOVE_INTERVAL = 10;
    static constexpr int BOSS_MOVE_COOLDOWN = 5;
    static constexpr int BOSS_ATTACK_RANGE  = 120;
    static constexpr int BOSS_CHASE_RANGE   = 2000;
    static constexpr int BOSS_APPROACH_SPEED = 5;
    static constexpr int CONTACT_DAMAGE_RANGE = 80;
    static constexpr int CONTACT_DAMAGE_INTERVAL = 45;
    static constexpr int BOUNCE_HIT_COOLDOWN = 30;  // 弹射球命中冷却（帧），防止连续帧反复扣血
    int m_bulletHitCooldown = 0;
};

#endif // COMBATSYSTEM_H
