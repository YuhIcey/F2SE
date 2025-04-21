#pragma once

#include <windows.h>
#include <string>
#include <functional>

namespace F2SE {

// Version information
constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

// Basic game types
using ObjectId = DWORD;
using CritterId = DWORD;
using ItemId = DWORD;
using MapId = DWORD;
using ScriptId = DWORD;

// Game enums
enum class DamageType {
    Normal = 0,
    Laser = 1,
    Fire = 2,
    Plasma = 3,
    Electrical = 4,
    EMP = 5,
    Explosion = 6
};

enum class BodyPart {
    Head = 0,
    LeftArm = 1,
    RightArm = 2,
    Torso = 3,
    RightLeg = 4,
    LeftLeg = 5,
    Eyes = 6,
    Groin = 7,
    Uncalled = 8
};

enum class CombatState {
    NotInCombat = 0,
    InCombat = 1,
    CantInvoke = 2
};

enum class Skill {
    SmallGuns = 0,
    BigGuns = 1,
    EnergyWeapons = 2,
    Unarmed = 3,
    Melee = 4,
    Throwing = 5,
    FirstAid = 6,
    Doctor = 7,
    Sneak = 8,
    Lockpick = 9,
    Steal = 10,
    Traps = 11,
    Science = 12,
    Repair = 13,
    Speech = 14,
    Barter = 15,
    Gambling = 16,
    Outdoorsman = 17
};

// Game structures
#pragma pack(push, 1)

struct Point {
    short x;
    short y;
};

struct WorldPos {
    short tile;      // Tile number
    short elevation; // Elevation level
    Point pos;       // Position within tile
};

struct ItemData {
    ItemId id;
    DWORD type;
    DWORD count;
    DWORD flags;
    DWORD weight;    // In pounds * 100
    DWORD cost;
};

struct CritterData {
    CritterId id;
    char name[32];
    Point position;
    short current_hp;
    short max_hp;
    short current_ap;
    short max_ap;
    short armor_class;
    BYTE level;
    BYTE gender;     // 0 = male, 1 = female
    
    // SPECIAL stats
    BYTE strength;
    BYTE perception;
    BYTE endurance;
    BYTE charisma;
    BYTE intelligence;
    BYTE agility;
    BYTE luck;

    // Skills (0-300)
    short skills[18];  // Indexed by Skill enum

    // Status flags
    DWORD flags;
    
    // Team and AI
    BYTE team_num;
    BYTE ai_packet;
    
    // Combat
    DWORD who_hit_me;
    BodyPart last_target;
    DamageType damage_type;
    CombatState combat_state;
};

struct ScriptData {
    ScriptId id;
    DWORD type;
    DWORD flags;
    DWORD local_vars[50];  // Script local variables
};

// Extended inventory item
struct InventoryItem : ItemData {
    Point location;      // Position in inventory grid
    bool equipped;       // Whether item is equipped
    DWORD condition;     // Item condition (0-100)
    DWORD ammo_count;    // Current ammo if weapon
    DWORD ammo_type;     // Type of ammo loaded
};

// Combat targeting data
struct TargetData {
    CritterId target;
    BodyPart body_part;
    short hit_chance;
    short damage;
    bool critical;
    BYTE attack_type;    // 0 = normal, 1 = aimed, 2 = called
};

// Map data
struct MapData {
    MapId id;
    char name[64];
    short num_tiles;
    BYTE num_levels;
    Point size;          // Map dimensions
    DWORD flags;
    DWORD music_num;     // Background music number
    DWORD ambient_sfx;   // Ambient sound effects
};

#pragma pack(pop)

// Callback function types
using ScriptFunction = std::function<void(ScriptData*)>;
using CombatCallback = std::function<void(CritterData*, TargetData*)>;
using ItemCallback = std::function<void(InventoryItem*)>;
using MapCallback = std::function<void(MapData*)>;

// Game memory addresses
namespace Memory {
    constexpr DWORD MAIN_LOOP = 0x1234;
    constexpr DWORD SCRIPT_PROC = 0x5678;
    constexpr DWORD GAME_INIT = 0x9ABC;
    
    // Player data
    constexpr DWORD PLAYER_NAME = 0x0000;
    constexpr DWORD PLAYER_LEVEL = 0x0000;
    constexpr DWORD PLAYER_XP = 0x0000;
    constexpr DWORD PLAYER_HP = 0x0000;
    constexpr DWORD PLAYER_AP = 0x0000;
    
    // SPECIAL stats
    constexpr DWORD PLAYER_STRENGTH = 0x0000;
    constexpr DWORD PLAYER_PERCEPTION = 0x0000;
    constexpr DWORD PLAYER_ENDURANCE = 0x0000;
    constexpr DWORD PLAYER_CHARISMA = 0x0000;
    constexpr DWORD PLAYER_INTELLIGENCE = 0x0000;
    constexpr DWORD PLAYER_AGILITY = 0x0000;
    constexpr DWORD PLAYER_LUCK = 0x0000;
}

// Game structures
struct PlayerData {
    char name[32];
    int level;
    int experience;
    int currentHP;
    int maxHP;
    int currentAP;
    int maxAP;
    
    // SPECIAL stats
    int strength;
    int perception;
    int endurance;
    int charisma;
    int intelligence;
    int agility;
    int luck;
};

// Script parameter types
enum class ScriptParamType {
    None,
    Integer,
    Float,
    String,
    Object
};

// Script parameter
struct ScriptParam {
    ScriptParamType type;
    union {
        int intValue;
        float floatValue;
        char* stringValue;
        void* objectValue;
    };
};

// Script context
struct ScriptContext {
    void* scriptObject;
    ScriptParam* params;
    int paramCount;
    ScriptParam returnValue;
};

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual const char* GetName() = 0;
    virtual const char* GetAuthor() = 0;
    virtual const char* GetVersion() = 0;
};

} // namespace F2SE 