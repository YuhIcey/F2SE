#pragma once

#include "GameAddresses.h"
#include <cstdint>
#include <array>

namespace F2SE::Game {

class SkillSystem {
public:
    // Skill indices as they appear in the game
    enum SkillIndex {
        SMALL_GUNS,
        BIG_GUNS,
        ENERGY_WEAPONS,
        UNARMED,
        MELEE_WEAPONS,
        THROWING,
        FIRST_AID,
        DOCTOR,
        SNEAK,
        LOCKPICK,
        STEAL,
        TRAPS,
        SCIENCE,
        REPAIR,
        SPEECH,
        BARTER,
        GAMBLING,
        OUTDOORSMAN,
        MAX_SKILLS
    };

    // Initialize the skill system hooks
    static bool Initialize();
    
    // Get/Set skill levels
    static int GetSkillLevel(SkillIndex skill);
    static bool SetSkillLevel(SkillIndex skill, int level);
    
    // Tag skill operations
    static bool IsTaggedSkill(SkillIndex skill);
    static bool SetTaggedSkill(SkillIndex skill, bool tagged);
    
    // Skill points management
    static int GetAvailableSkillPoints();
    static bool SetAvailableSkillPoints(int points);
    
    // Skill cost calculations
    static int CalculateSkillIncreaseCost(SkillIndex skill);
    static int CalculateSkillDecreaseCost(SkillIndex skill);
    
    // Skill checks
    static bool PerformSkillCheck(SkillIndex skill, int difficulty);
    static int CalculateSkillBonus(SkillIndex skill);

private:
    // Internal helper functions
    static bool InstallHooks();
    static bool ValidateSkillIndex(SkillIndex skill);
    static bool ValidateSkillLevel(int level);
    
    // Memory addresses for skill data
    static constexpr uintptr_t SKILL_POINTS_BASE = 0x0;  // TODO: Add actual base address
    static constexpr uintptr_t TAGGED_SKILLS_BASE = 0x0; // TODO: Add actual base address
    
    // Skill level limits
    static constexpr int MIN_SKILL_LEVEL = 0;
    static constexpr int MAX_SKILL_LEVEL = 300;
    static constexpr int MAX_TAG_SKILLS = 4;
};

} // namespace F2SE::Game 