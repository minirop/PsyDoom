#pragma once

#include "Macros.h"

#include <cstdint>

// What type of game disc is loaded?
enum class GameType : int32_t {
    Doom,
    FinalDoom
};

// What variant of the game is being run?
enum class GameVariant : int32_t {
    NTSC_U,     // North America/US version (NTSC)
    NTSC_J,     // Japanese version (NTSC)
    PAL         // European version (PAL)
};

// Settings for the game.
// These must be synchronized in multiplayer games, and set appropriately for correct demo playback.
struct GameSettings {
    uint8_t     bUsePalTimings;                 // Use 50 Hz vblanks and other various timing adjustments for the PAL version of the game?
    uint8_t     bUseDemoTimings;                // Force player logic to run at a consistent, but slower rate used by demos? (15 Hz for NTSC)
    uint8_t     bUseExtendedPlayerShootRange;   // Extend player max shoot distance from '2048' to '8192' and auto aim distance from '1024' to '8192'? (similar range to ZDoom)
    uint8_t     bUsePlayerRocketBlastFix;       // Apply the fix for the player sometimes not receiving splash damage from rocket blasts?
    uint8_t     bUseSuperShotgunDelayTweak;     // Whether to apply the gameplay tweak that reduces the initial firing delay of the super shotgun
    uint8_t     bUseMoveInputLatencyTweak;      // Use a tweak to player movement which tries to reduce input latency? This affects movement slightly.
    uint8_t     bUseItemPickupFix;              // Fix an original bug where sometimes valid items can't be picked up due to other items nearby that can't be picked up?
    uint8_t     bUseFinalDoomPlayerMovement;    // Whether to use the Final Doom way of doing player movement & turning
    uint8_t     bAllowMovementCancellation;     // Digital movement only: whether opposite move inputs can cancel each other out
    uint8_t     bAllowTurningCancellation;      // Digital turning only: whether opposite turn inputs can cancel each other out
    uint8_t     bFixViewBobStrength;            // Fix the strength of the view bobbing when running at 30 FPS so it's as intense as 15 FPS
    uint8_t     bFixGravityStrength;            // Fix the strength of gravity so it applies consistently regardless of framerate? (weakens overly strong gravity at 30 FPS)
    uint8_t     bNoMonsters;                    // Is the '-nomonsters' command line cheat activated?
    uint8_t     bPistolStart;                   // Is the '-pistolstart' command line switch specified?
    uint8_t     bTurboMode;                     // Is the '-turbo' command line cheat specified?
    uint8_t     bUseLostSoulSpawnFix;           // If true then apply a fix to prevent Lost Souls from spawning outside of the level
    uint8_t     bUseLineOfSightOverflowFix;     // If true then apply a fix to prevent numeric overflows in the enemy line of sight code
    uint8_t     bRemoveMaxCrossLinesLimit;      // If true then the player can cross an unlimited number of line specials per tick rather than '8'
    uint8_t     bFixOutdoorBulletPuffs;         // Fix a Doom engine bug where bullet puffs don't appear sometimes after shooting outdoor walls?
    uint8_t     bFixSoundPropagation;           // Fix an original bug where sometimes sound propagates through closed doors that should block it?
    int32_t     lostSoulSpawnLimit;             // How many lost souls to limit a level to when Pain Elementals try to spawn one. -1 means no limit.
    int32_t     viewBobbingStrengthFixed;       // 16.16 multiplier for view bobbing strength
};

struct String32;

BEGIN_NAMESPACE(Game)

extern GameType         gGameType;
extern GameVariant      gGameVariant;
extern GameSettings     gSettings;
extern bool             gbIsPsxDoomForever;

void determineGameTypeAndVariant() noexcept;
bool isFinalDoom() noexcept;
void getUserGameSettings(GameSettings& settings) noexcept;
void getClassicDemoGameSettings(GameSettings& settings) noexcept;
int32_t getNumMaps() noexcept;
int32_t getNumRegularMaps() noexcept;
const String32& getMapName(const int32_t mapNum) noexcept;
int32_t getNumEpisodes() noexcept;
const String32& getEpisodeName(const int32_t episodeNum) noexcept;
int32_t getEpisodeStartMap(const int32_t episodeNum) noexcept;
int32_t getMapEpisode(const int32_t mapNum) noexcept;
uint16_t getTexPalette_BACK() noexcept;
uint16_t getTexPalette_LOADING() noexcept;
uint16_t getTexPalette_PAUSE() noexcept;
uint16_t getTexPalette_OptionsBg() noexcept;
const char* getTexLumpName_OptionsBg() noexcept;
uint16_t getTexPalette_NETERR() noexcept;
uint16_t getTexPalette_DOOM() noexcept;
uint16_t getTexPalette_BUTTONS() noexcept;
uint16_t getTexPalette_CONNECT() noexcept;
uint16_t getTexPalette_DebugFontSmall() noexcept;
void startLevelTimer() noexcept;
void stopLevelTimer() noexcept;
int64_t getLevelFinishTimeCentisecs() noexcept;

END_NAMESPACE(Game)
