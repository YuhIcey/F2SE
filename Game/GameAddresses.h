#pragma once

#include <windows.h>
#include <cstdint>

namespace F2SE::Game {

// Memory addresses for different game versions
struct GameAddresses {
    // Core addresses
    DWORD pipboy;
    DWORD citypatch;
    DWORD nomovie;
    DWORD damages_computation;
    
    // Character creation
    DWORD malestart;
    DWORD femalestart;
    DWORD maledefault;
    DWORD femaledefault;
    
    // Game state
    DWORD startmap;
    DWORD dat_name;
    DWORD mod_name;
    
    // Date/Time
    DWORD start_year;      // 2 bytes
    DWORD start_month;
    DWORD start_day;
    
    // World position
    DWORD world_x;        // 2 bytes
    DWORD world_y;        // 2 bytes

    // Combat
    DWORD random_damage;
    DWORD attack_data;

    // SPECIAL Stats
    static constexpr uintptr_t STAT_STRENGTH     = 0x4AF112;
    static constexpr uintptr_t STAT_PERCEPTION   = 0x4AF174;
    static constexpr uintptr_t STAT_ENDURANCE    = 0x4AF190;
    static constexpr uintptr_t STAT_CHARISMA     = 0x4AF1AE;
    static constexpr uintptr_t STAT_INTELLIGENCE = 0x4AF207;
    static constexpr uintptr_t STAT_AGILITY      = 0x4AF223;
    static constexpr uintptr_t STAT_LUCK         = 0x4AF23E;

    // Skill System Addresses
    struct SkillAddresses {
        // Main skill function return addresses
        static constexpr uintptr_t SKILL_LEVEL_RETURN          = 0x4AA64B;  // Return address after skill level modification
        static constexpr uintptr_t SKILL_DEC_POINT_LIMIT       = 0x4AAA91;  // Return for skill point decrease limit
        static constexpr uintptr_t SKILL_LEVEL_BONUS_RETURN    = 0x4AA5D6;  // Return after skill bonus calculation
        static constexpr uintptr_t SKILL_INC_POINT_COST        = 0x4AA7C1;  // Skill point increase cost handling
        static constexpr uintptr_t SKILL_DEC_POINT_COST        = 0x4AA98D;  // Skill point decrease cost handling
        
        // Special skill functions
        static constexpr uintptr_t STEALING_PATCH              = 0x4ABC62;  // Stealing logic patch
        static constexpr uintptr_t WEAPON_SKILL_HOOK          = 0x47831E;  // Weapon skill handling hook
        static constexpr uintptr_t REMOVE_SKILL_POINTS_LIMIT   = 0x43C2B9;  // Remove unspent points limit
        
        // Skill system hooks and jumps
        static constexpr uintptr_t HOOK_SKILL_INIT            = 0x4AA63C;
        static constexpr uintptr_t HOOK_SKILL_CALC            = 0x4AA847;
        static constexpr uintptr_t HOOK_SKILL_CHECK           = 0x4AA725;
        static constexpr uintptr_t HOOK_SKILL_DEC             = 0x4AAA84;
        static constexpr uintptr_t HOOK_STEAL_CHECK           = 0x4ABC67;
        static constexpr uintptr_t HOOK_SKILL_BONUS           = 0x4AA59D;
        static constexpr uintptr_t HOOK_SKILL_INC             = 0x4AA738;
        static constexpr uintptr_t HOOK_SKILL_POINTS          = 0x4AA940;
        static constexpr uintptr_t HOOK_SKILL_LIMIT_LOW       = 0x4AA9E1;
        static constexpr uintptr_t HOOK_SKILL_LIMIT_HIGH      = 0x4AA9F1;
        
        // Tag skill configuration
        static constexpr uintptr_t TAG_SKILL_BONUS            = 0x4AA612;
        static constexpr uintptr_t TAG_SKILL_MODE             = 0x4AA60E;
        static constexpr uintptr_t TAG_SKILL_CONFIG           = 0x4AA61E;
        
        // Batch operations
        static constexpr uintptr_t BATCH_SKILL_LIMIT_1        = 0x4AA91B;
        static constexpr uintptr_t BATCH_SKILL_LIMIT_2        = 0x4AAA1A;
        static constexpr uintptr_t BATCH_SKILL_LIMIT_3        = 0x4AAA23;
    };

    // Helper functions to read/write stats
    static int8_t GetStat(uintptr_t address);
    static void SetStat(uintptr_t address, int8_t value);

    // Validate stat value is within valid range (1-10 for SPECIAL stats)
    static bool IsValidStatValue(int8_t value, int8_t min = 1, int8_t max = 10);
};

// Combat structures
#pragma pack(push, 1)
struct AttackData {
    DWORD who;           // Attacker
    DWORD target;        // Target
    DWORD weapon_id;     // Weapon ID
    DWORD damage_type;   // Type of damage
    DWORD damage_mult;   // Damage multiplier
    DWORD bonus_ranged;  // Ranged attack bonus
    DWORD hits;         // Number of hits
};

struct DamageComputation {
    int bonus_ranged;    // Ranged attack bonus
    int dam_mult;        // Damage multiplier
    int dam_div;        // Damage divider
    int cbt_diff_mod;   // Combat difficulty modifier
    int dmg_thresh;     // Damage threshold
    int ammo_dr_adj;    // Ammo DR adjustment
    int nbhit;          // Number of hits
    int damages;        // Final damage value
};
#pragma pack(pop)

} // namespace F2SE::Game 