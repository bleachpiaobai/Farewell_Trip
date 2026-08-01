#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QString>
#include <QCoreApplication>
#include <QDir>

namespace GameConfig {

// ── Resource path helper ───────────────────────────
// Converts Qt resource paths (:/images/...) to filesystem paths.
// Images are copied to build dir by CMake and loaded from disk at runtime.
// Directory names match Farewell_Trip.doc (e.g. ch01_awaken, ch02_peach, ...).
inline QString imagePath(const QString& qrcPath) {
    static const QString PREFIX = QStringLiteral(":/images/");
    if (qrcPath.startsWith(PREFIX)) {
        return QCoreApplication::applicationDirPath()
               + QStringLiteral("/images/")
               + qrcPath.mid(PREFIX.size());
    }
    return qrcPath;  // keep non-image resources as-is (e.g. :/styles/)
}

// ── Window ──────────────────────────────────────────
constexpr int WINDOW_WIDTH  = 1280;
constexpr int WINDOW_HEIGHT = 720;

// ── Frame Rate ──────────────────────────────────────
constexpr int TARGET_FPS        = 60;
constexpr int FRAME_INTERVAL_MS = 1000 / TARGET_FPS;   // ~16 ms

// ── Player ──────────────────────────────────────────
constexpr int PLAYER_MOVE_SPEED = 6;           // pixels per frame (wider dodge window)
constexpr int PLAYER_HP          = 10;
constexpr int ATTACK_RANGE       = 100;         // must be within 100 px
constexpr int PLAYER_DAMAGE     = 5;
constexpr int ATTACK_COOLDOWN    = 33;          // frames — match 9×60ms=540ms animation
constexpr qreal PLAYER_GROUND_Y  = 530.0;       // Y coordinate on ground (adjusted for larger sprite H=180)
constexpr qreal JUMP_VELOCITY    = -20.0;       // initial upward speed — high enough to clear bullets
constexpr qreal GRAVITY          = 0.85;        // downward acceleration — floaty dodge window

// ── Boss ────────────────────────────────────────────
constexpr int BOSS_APPROACH_DISTANCE = 100;
constexpr int PEACH_BOSS_HP   = 100;
constexpr int EXGIRL_BOSS_HP  = 100;
constexpr int ANGUS_BOSS_HP   = 120;

// ── Scene Exit ──────────────────────────────────────
constexpr int SCENE_EXIT_X    = 1100;         // walk right past this to advance
constexpr int WALK_SCENE_WIDTH = 3000;        // wide scene for chapters 2 & 3

// ── Dialogue ────────────────────────────────────────
constexpr int DIALOGUE_CHARS_PER_SEC = 20;
constexpr int DIALOGUE_TIMER_MS      = 1000 / DIALOGUE_CHARS_PER_SEC;   // 50 ms
constexpr int DIALOGUE_COMMA_PAUSE   = 200;  // ms
constexpr int DIALOGUE_PUNCT_PAUSE   = 400;  // ms for . ! ?

// ── Transition ──────────────────────────────────────
constexpr int TRANSITION_DURATION_MS = 500;
constexpr int TRANSITION_FADE_MS     = 400;  // quick cross-fade

// ── Scene Enum ──────────────────────────────────────
enum class SceneId : int {
    SCENE_OPEN = 0,
    SCENE_FOREST,
    SCENE_PEACH,
    SCENE_NEWYORK,
    SCENE_SHANGHAI,
    SCENE_SUFA,
    SCENE_NANJING,
    SCENE_AIRBRIDGE,
    SCENE_EMPTY,
    SCENE_POEM,
    SCENE_COUNT,
    SCENE_ENDING
};

inline const char* sceneIdName(SceneId id)
{
    switch (id) {
    case SceneId::SCENE_OPEN:      return "OPEN";
    case SceneId::SCENE_FOREST:    return "FOREST";
    case SceneId::SCENE_PEACH:     return "PEACH";
    case SceneId::SCENE_NEWYORK:   return "NEWYORK";
    case SceneId::SCENE_SHANGHAI:  return "SHANGHAI";
    case SceneId::SCENE_SUFA:      return "SUFA";
    case SceneId::SCENE_NANJING:   return "NANJING";
    case SceneId::SCENE_AIRBRIDGE: return "AIRBRIDGE";
    case SceneId::SCENE_EMPTY:     return "EMPTY";
    case SceneId::SCENE_POEM:      return "POEM";
    default: return "UNKNOWN";
    }
}

// ── Game State ──────────────────────────────────────
enum class GameState {
    MAIN_MENU,
    PLAYING,
    DIALOGUE,
    COMBAT,
    CUTSCENE,
    PAUSED,
    SCENE_TRANSITION
};

// ── Animation ───────────────────────────────────────
enum class AnimState {
    IDLE,
    WALK_LEFT,
    WALK_RIGHT,
    ATTACK,
    HIT,
    DEATH
};

// ── Placeholder Colors (per entity) ─────────────────
namespace PlaceholderColor {
    inline constexpr auto Player      = "#4A90D9";  // steel blue
    inline constexpr auto PeachBoss   = "#E8917E";  // peach pink
    inline constexpr auto ExGirlBoss  = "#D94A8A";  // magenta
    inline constexpr auto AngusBoss   = "#4A2C2A";  // dark corrupted red-brown
    inline constexpr auto NPC         = "#7B7B7B";  // gray
}

// ── Window Title ────────────────────────────────────
inline const QString WINDOW_TITLE = QStringLiteral("YAN · 一场道别的旅途");

} // namespace GameConfig

#endif // GAMECONFIG_H
