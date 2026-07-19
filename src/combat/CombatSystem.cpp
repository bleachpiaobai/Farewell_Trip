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
    emit combatEnded();
}

void CombatSystem::onTick()
{
    if (!m_inCombat || !m_player || !m_currentBoss || m_currentBoss->isDead())
        return;
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

    static constexpr qreal ATTACK_FORWARD_RANGE = 120.0;
    static constexpr qreal ATTACK_VERT_TOLERANCE = 60.0;

    qreal playerCX = m_player->x() + 30.0;
    qreal playerCY = m_player->y() + 55.0;
    qreal bossCX   = m_currentBoss->x() + 40.0;
    qreal bossCY   = m_currentBoss->y() + 40.0;

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
