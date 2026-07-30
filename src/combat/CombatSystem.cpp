#include "CombatSystem.h"
#include "entity/Player.h"
#include "entity/Enemy.h"
#include "core/EventBus.h"
#include "utils/MathUtils.h"
#include <QDebug>

CombatSystem::CombatSystem(EventBus* bus, QObject* parent)
    : QObject(parent)
    , m_bus(bus)
{
}

bool CombatSystem::bossAlive() const
{
    return m_currentBoss && !m_currentBoss->isDead();
}

void CombatSystem::startCombat(Enemy* boss)
{
    m_currentBoss = boss;
    m_inCombat = true;
    m_enemyAttackTimer = 0;
    m_enemyMoveTimer = 0;
    m_enemyMoveCooldown = 0;
    m_contactDamageTimer = 0;
    m_bossAttacking = false;
    m_bossAttackAnimTimer = 0;
    if (boss) {
        boss->reset();
        qDebug() << "[Combat] Boss spawned:" << boss->name()
                 << "HP:" << boss->hp() << "/" << boss->maxHp()
                 << "at (" << boss->x() << "," << boss->y() << ")";
        emit bossSpawned(boss);
    }
}

void CombatSystem::endCombat()
{
    m_inCombat = false;
    m_currentBoss = nullptr;
    m_enemyAttackTimer = 0;
    m_enemyMoveTimer = 0;
    m_enemyMoveCooldown = 0;
    m_contactDamageTimer = 0;
    m_bossAttacking = false;
    m_bossAttackAnimTimer = 0;
    emit combatEnded();
}

void CombatSystem::onTick()
{
    if (!m_inCombat || !m_player || !m_currentBoss || m_currentBoss->isDead())
        return;

    qreal playerCX = m_player->x() + 50.0;
    qreal bossCX   = m_currentBoss->x() + 70.0;
    qreal bossCY   = m_currentBoss->y() + 70.0;
    qreal dx = playerCX - bossCX;   // 保留方向（正=玩家在右）
    qreal adx = qAbs(dx);

    // ── Boss 始终面向玩家 ──
    m_currentBoss->faceToward(playerCX);

    // ── 攻击动画计时 ──
    if (m_bossAttackAnimTimer > 0) {
        m_bossAttackAnimTimer--;
        if (m_bossAttackAnimTimer == 0) {
            m_bossAttacking = false;
            m_currentBoss->playWalkAnim();  // 攻击结束，回到走路动画
        }
        return;  // 攻击动画期间不移动
    }

    // ── Boss 移动 AI：向玩家接近 ──
    m_enemyMoveTimer++;

    if (m_enemyMoveCooldown > 0) {
        m_enemyMoveCooldown--;
    }

    // 每 BOSS_MOVE_INTERVAL 帧做一次决策
    if (m_enemyMoveTimer >= BOSS_MOVE_INTERVAL && m_enemyMoveCooldown <= 0) {
        m_enemyMoveTimer = 0;

        // 在追逐范围内向玩家移动
        if (adx > BOSS_ATTACK_RANGE && adx <= BOSS_CHASE_RANGE) {
            qreal moveSpeed = BOSS_APPROACH_SPEED + (adx > 250 ? 1.0 : 0.0);
            m_currentBoss->moveToward(playerCX, moveSpeed);
            m_currentBoss->playWalkAnim();
            m_enemyMoveCooldown = BOSS_MOVE_COOLDOWN;
        }
    }

    // ── 接触伤害：玩家贴脸Boss时持续扣血 ──
    m_contactDamageTimer++;
    if (m_contactDamageTimer >= CONTACT_DAMAGE_INTERVAL) {
        m_contactDamageTimer = 0;
        if (adx <= CONTACT_DAMAGE_RANGE) {
            static constexpr int CONTACT_DMG = 1;
            m_player->takeDamage(CONTACT_DMG);
            qDebug() << "[Combat] Contact damage! Player HP:" << m_player->hp();
        }
    }

    // ── 敌人主动攻击逻辑 ──
    m_enemyAttackTimer++;

    if (m_enemyAttackTimer >= 50) {  // ~0.8秒检查一次
        m_enemyAttackTimer = 0;

        // 距离 + 方向双重判定：Boss 必须面向玩家才能攻击
        bool bossFacesPlayer = (dx > 0 && m_currentBoss->dir() == 1) ||
                               (dx < 0 && m_currentBoss->dir() == -1);

        if (adx <= BOSS_ATTACK_RANGE && bossFacesPlayer) {
            // 在攻击范围内且面向玩家 → 播放攻击动画并造成伤害
            m_bossAttacking = true;
            m_bossAttackAnimTimer = 24;  // 攻击动画持续约 0.4 秒 (60fps)
            m_currentBoss->playAttackAnim();

            static constexpr int ENEMY_DAMAGE = 2;
            m_player->takeDamage(ENEMY_DAMAGE);
            qDebug() << "[Combat] Boss attacks! Player HP:" << m_player->hp()
                     << "Boss at:" << bossCX << "Player at:" << playerCX;
        }
    }
}

void CombatSystem::onPlayerAttack()
{
    qDebug() << "[Combat] onPlayerAttack called, inCombat:" << m_inCombat
             << "player:" << (m_player != nullptr)
             << "boss:" << (m_currentBoss != nullptr);

    if (!m_inCombat || !m_player || !m_currentBoss || m_currentBoss->isDead()) {
        qDebug() << "[Combat] Attack BLOCKED — inCombat:" << m_inCombat
                 << "hasPlayer:" << (m_player != nullptr)
                 << "hasBoss:" << (m_currentBoss != nullptr)
                 << "bossDead:" << (m_currentBoss ? m_currentBoss->isDead() : true);
        return;
    }

    static constexpr qreal ATTACK_FORWARD_RANGE = 180.0;   // ~2 player body-widths
    static constexpr qreal ATTACK_VERT_TOLERANCE = 80.0;

    qreal playerCX = m_player->x() + 50.0;
    qreal playerCY = m_player->y() + 90.0;
    qreal bossCX   = m_currentBoss->x() + 70.0;
    qreal bossCY   = m_currentBoss->y() + 70.0;

    qreal dx = bossCX - playerCX;
    qreal dy = bossCY - playerCY;

    int dir = m_player->dir();
    qDebug() << "[Combat] Player(" << playerCX << "," << playerCY << ") dir:" << dir
             << "Boss(" << bossCX << "," << bossCY << ") dx:" << dx << "dy:" << dy;

    // Boss must be in front of player's facing direction
    if ((dir == 1 && dx < 0) || (dir == -1 && dx > 0)) {
        qDebug() << "[Combat] Attack MISS — boss behind player";
        return;
    }

    if (qAbs(dx) <= ATTACK_FORWARD_RANGE && qAbs(dy) <= ATTACK_VERT_TOLERANCE) {
        qDebug() << "[Combat] HIT! Boss takes" << GameConfig::PLAYER_DAMAGE << "damage";
        m_currentBoss->takeDamage(GameConfig::PLAYER_DAMAGE);

        if (m_currentBoss->isDead()) {
            qDebug() << "[Combat] Boss DEFEATED!";
            m_inCombat = false;
            emit bossDefeated(m_currentBoss->name());
            m_bus->bossDefeated(m_currentBoss->name());
        }
    } else {
        qDebug() << "[Combat] Attack MISS — too far, dx:" << qAbs(dx)
                 << "(need <=" << ATTACK_FORWARD_RANGE << ")"
                 << "dy:" << qAbs(dy)
                 << "(need <=" << ATTACK_VERT_TOLERANCE << ")";
    }
}
