#include "CombatSystem.h"
#include "combat/Projectile.h"
#include "combat/Laser.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "core/EventBus.h"
#include "utils/MathUtils.h"
#include <QDebug>
#include <QtMath>
#include <QRandomGenerator>

CombatSystem::CombatSystem(EventBus* bus, QObject* parent)
    : QObject(parent), m_bus(bus) {}

void CombatSystem::startCombat(Enemy* boss)
{
    m_currentBoss = boss;
    m_inCombat = true;
    m_enemyAttackTimer = 0;
    m_enemyMoveTimer = m_enemyMoveCooldown = 0;
    m_contactDamageTimer = 0;
    m_bossAttacking = false;
    m_bossAttackAnimTimer = 0;
    m_bulletTimer = m_laserTimer = 0;
    m_barrageTimer = 0;
    clearAllBossProjectiles();
    clearAllPlayerProjectiles();
    clearAllLasers();

    if (boss) {
        boss->reset();
        qDebug() << "[Combat] start:" << boss->name() << "HP:" << boss->hp();
        emit bossSpawned(boss);

        // XIA 专属：一颗蓝色弹跳球，水平慢速来回弹，不消失
        if (boss->name() == QStringLiteral("XIA")) {
            spawnBossBullet(QColor(60, 140, 255), 14, 5,
                            3.0, 0.0, 620.0, 0.0, -1, true);
        }
    }
}

void CombatSystem::endCombat()
{
    m_inCombat = false;
    m_currentBoss = nullptr;
    clearAllBossProjectiles();
    clearAllPlayerProjectiles();
    clearAllLasers();
    emit combatEnded();
}

// ═══════════════════════════════════════════════════════════════
//  Boss 弹幕  —  固定高度 BULLET_GROUND_Y = 545
//  站立 YAN 碰撞区 Y: 505~735  跳跃峰值 Y: 290~500  弹幕 Y: 533~557
//  跳跃时 500 < 533 → 安全 ✓    站立时 505~735 包含 533~557 → 命中 ✓
// ═══════════════════════════════════════════════════════════════

void CombatSystem::spawnBossBullet(const QColor& color, qreal radius, int damage,
                                    qreal vx, qreal vy, qreal spawnY,
                                    qreal gravity, int lifetime,
                                    bool bouncing)
{
    if (!m_scene || !m_currentBoss) return;

    auto* p = new Projectile(color, radius, damage, vx, vy, gravity, lifetime);
    p->setPos(m_currentBoss->x() + 70, spawnY);
    if (bouncing) p->setBouncing(true);
    m_scene->addItem(p);
    m_bossProjectiles.append(p);
}

void CombatSystem::updateBossProjectiles()
{
    for (int i = m_bossProjectiles.size() - 1; i >= 0; i--) {
        Projectile* p = m_bossProjectiles[i];
        p->tick();

        if (!p->isExpired() && m_player && m_player->hp() > 0) {
            QRectF bulletRect = p->sceneBoundingRect();
            QRectF playerRect = m_player->sceneBoundingRect();
            if (bulletRect.intersects(playerRect)) {
                // 弹跳球有命中冷却，防止连续帧反复扣血
                if (p->isBouncing()) {
                    if (m_bulletHitCooldown > 0) {
                        continue;  // 冷却中，跳过
                    }
                    m_bulletHitCooldown = BOUNCE_HIT_COOLDOWN;
                }
                m_player->takeDamage(p->damage());
                qDebug() << "[Combat] Boss bullet HIT — player HP:" << m_player->hp()
                         << "bullet rect:" << bulletRect << "player rect:" << playerRect;
                if (!p->isBouncing()) {
                    m_bossProjectiles.removeAt(i);
                    if (p->scene()) p->scene()->removeItem(p);
                    delete p;
                }
                continue;
            }
        }

        if (p->isExpired()) {
            m_bossProjectiles.removeAt(i);
            if (p->scene()) p->scene()->removeItem(p);
            delete p;
        }
    }
}

void CombatSystem::clearAllBossProjectiles()
{
    for (auto* p : m_bossProjectiles) {
        if (p->scene()) p->scene()->removeItem(p);
        delete p;
    }
    m_bossProjectiles.clear();
}

// ═══════════════════════════════════════════════════════════════
//  玩家红色光球  —  大号可见，速度适中
// ═══════════════════════════════════════════════════════════════

void CombatSystem::spawnPlayerFireball()
{
    if (!m_scene || !m_player || !m_currentBoss) return;

    int dir = m_player->attackDir();  // 锁定攻击瞬间方向，不受后续移动影响
    qreal sx = m_player->x() + 50.0 + dir * 40.0;
    qreal sy = m_player->y() + 90.0;

    auto* p = new Projectile(
        QColor(255, 60, 30),    // 亮红色
        12,                       // 半径 12px — 可见
        GameConfig::PLAYER_DAMAGE,
        dir * 6.0,               // 速度适中
        0.0, 0.0,
        -1                        // 不限时，出屏或命中 Boss 才消失
    );
    p->setPos(sx, sy);
    m_scene->addItem(p);
    m_playerProjectiles.append(p);
    qDebug() << "[Combat] Player fireball spawned at" << sx << sy << "dir:" << dir;
}

void CombatSystem::updatePlayerProjectiles()
{
    for (int i = m_playerProjectiles.size() - 1; i >= 0; i--) {
        Projectile* p = m_playerProjectiles[i];
        p->tick();

        if (!p->isExpired() && m_currentBoss && !m_currentBoss->isDead()) {
            if (p->sceneBoundingRect().intersects(m_currentBoss->sceneBoundingRect())) {
                m_currentBoss->takeDamage(p->damage());
                qDebug() << "[Combat] Fireball HIT — Boss HP:" << m_currentBoss->hp();

                if (m_currentBoss->isDead()) {
                    m_inCombat = false;
                    emit bossDefeated(m_currentBoss->name());
                    m_bus->bossDefeated(m_currentBoss->name());
                }
                m_playerProjectiles.removeAt(i);
                if (p->scene()) p->scene()->removeItem(p);
                delete p;
                continue;
            }
        }

        if (p->isExpired()) {
            m_playerProjectiles.removeAt(i);
            if (p->scene()) p->scene()->removeItem(p);
            delete p;
        }
    }
}

void CombatSystem::clearAllPlayerProjectiles()
{
    for (auto* p : m_playerProjectiles) {
        if (p->scene()) p->scene()->removeItem(p);
        delete p;
    }
    m_playerProjectiles.clear();
}

// ═══════════════════════════════════════════════════════════════
//  激光
// ═══════════════════════════════════════════════════════════════

void CombatSystem::spawnLaser(qreal x)
{
    if (!m_scene) return;
    auto* laser = new Laser(x, 0.0, GameConfig::WINDOW_HEIGHT, 28.0,
                            QColor(60, 140, 255), 35, 25, 10, nullptr);
    m_scene->addItem(laser);
    m_lasers.append(laser);
}

void CombatSystem::spawnLaserPattern(int count)
{
    qreal margin = 80.0;
    qreal zoneWidth = (GameConfig::WINDOW_WIDTH - margin * 2) / count;
    for (int i = 0; i < count; i++) {
        qreal zoneStart = margin + i * zoneWidth;
        qreal x = zoneStart + QRandomGenerator::global()->bounded(zoneWidth - 30.0);
        spawnLaser(x);
    }
}

void CombatSystem::updateLasers()
{
    for (int i = m_lasers.size() - 1; i >= 0; i--) {
        Laser* laser = m_lasers[i];
        laser->tick();
        if (laser->isActive() && !laser->isExpired() && !laser->hasHit()
            && m_player && m_player->hp() > 0) {
            if (laser->sceneBoundingRect().intersects(m_player->sceneBoundingRect())) {
                m_player->takeDamage(laser->damage());
                laser->markHit();
                qDebug() << "[Combat] Laser hit!";
            }
        }
        if (laser->isExpired()) {
            m_lasers.removeAt(i);
            if (laser->scene()) laser->scene()->removeItem(laser);
            delete laser;
        }
    }
}

void CombatSystem::clearAllLasers()
{
    for (auto* laser : m_lasers) {
        if (laser->scene()) laser->scene()->removeItem(laser);
        delete laser;
    }
    m_lasers.clear();
}

// ═══════════════════════════════════════════════════════════════
//  Peach：低频单点（每 200 帧 = ~3.3s）
// ═══════════════════════════════════════════════════════════════

void CombatSystem::tickPeachPatterns()
{
    m_bulletTimer++;
    qreal dir = (m_player->x() + 50.0 > m_currentBoss->x() + 70.0) ? 1.0 : -1.0;

    if (m_bulletTimer >= 200) {
        m_bulletTimer = 0;
        m_bossAttacking = true;
        m_bossAttackAnimTimer = 18;
        m_currentBoss->playAttackAnim();

        spawnBossBullet(QColor(255, 105, 180), 12, 5,
                        dir * 3.5, 0.0, 620.0, 0.0, 280);
    }
}

// ═══════════════════════════════════════════════════════════════
//  XIA：常驻弹跳球（开局即存在）+ 全屏激光（周期释放）
// ═══════════════════════════════════════════════════════════════

void CombatSystem::tickXIAPatterns()
{
    m_laserTimer++;

    // ── 全屏激光：~200帧（≈3.3s）──
    if (m_laserTimer >= 200) {
        m_laserTimer = 0;
        m_bossAttacking = true;
        m_bossAttackAnimTimer = 30;
        m_currentBoss->playAttackAnim();
        spawnLaserPattern(3);
    }
}

// ═══════════════════════════════════════════════════════════════
//  主 Tick
// ═══════════════════════════════════════════════════════════════

void CombatSystem::onTick()
{
    if (!m_inCombat || !m_player || !m_currentBoss || m_currentBoss->isDead())
        return;

    updateBossProjectiles();
    updatePlayerProjectiles();
    updateLasers();

    if (m_bulletHitCooldown > 0) m_bulletHitCooldown--;

    qreal playerCX = m_player->x() + 50.0;
    qreal bossCX   = m_currentBoss->x() + 70.0;
    qreal dx = playerCX - bossCX;
    qreal adx = qAbs(dx);

    m_currentBoss->faceToward(playerCX);

    if (m_bossAttackAnimTimer > 0) {
        m_bossAttackAnimTimer--;
        if (m_bossAttackAnimTimer == 0) {
            m_bossAttacking = false;
            m_currentBoss->playWalkAnim();
        }
    } else {
        m_enemyMoveTimer++;
        if (m_enemyMoveCooldown > 0) m_enemyMoveCooldown--;
        if (m_enemyMoveTimer >= BOSS_MOVE_INTERVAL && m_enemyMoveCooldown <= 0) {
            m_enemyMoveTimer = 0;
            if (adx > BOSS_ATTACK_RANGE && adx <= BOSS_CHASE_RANGE) {
                qreal spd = BOSS_APPROACH_SPEED + (adx > 250 ? 1.0 : 0.0);
                m_currentBoss->moveToward(playerCX, spd);
                m_currentBoss->playWalkAnim();
                m_enemyMoveCooldown = BOSS_MOVE_COOLDOWN;
            }
        }
    }

    m_contactDamageTimer++;
    if (m_contactDamageTimer >= CONTACT_DAMAGE_INTERVAL) {
        m_contactDamageTimer = 0;
        if (adx <= CONTACT_DAMAGE_RANGE) {
            m_player->takeDamage(1);
            qDebug() << "[Combat] Contact dmg — HP:" << m_player->hp();
        }
    }

    m_enemyAttackTimer++;
    if (m_enemyAttackTimer >= 50) {
        m_enemyAttackTimer = 0;
        bool facing = (dx > 0 && m_currentBoss->dir() == 1) ||
                      (dx < 0 && m_currentBoss->dir() == -1);
        if (adx <= BOSS_ATTACK_RANGE && facing && !m_bossAttacking) {
            m_bossAttacking = true;
            m_bossAttackAnimTimer = 24;
            m_currentBoss->playAttackAnim();
            m_player->takeDamage(5);
            qDebug() << "[Combat] Boss melee — HP:" << m_player->hp();
        }
    }

    if (m_bossAttackAnimTimer <= 0) {
        const QString& n = m_currentBoss->name();
        if (n == QStringLiteral("桃子怪")) tickPeachPatterns();
        else if (n == QStringLiteral("XIA")) tickXIAPatterns();
    }
}

// ═══════════════════════════════════════════════════════════════
//  玩家攻击 → 红色光球
// ═══════════════════════════════════════════════════════════════

void CombatSystem::onPlayerAttack()
{
    if (!m_inCombat || !m_player || !m_currentBoss || m_currentBoss->isDead()) {
        qDebug() << "[Combat] Fireball BLOCKED — inCombat:" << m_inCombat;
        return;
    }
    spawnPlayerFireball();
}
