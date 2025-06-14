/**
 * @file mortal_kombat_info.h
 * @brief Contains core enums, sprite data structures, and constants for Mortal_Kombat_1992.
 *
 * Defines player states, character types, special attacks, and sprite layout information.
 */

#pragma once
#include <array>

namespace mortal_kombat
{
    /// @enum State
    /// @brief Holds the different states of the player.
    enum class State {
        STANCE = 0,
        WALK_FORWARDS,
        WALK_BACKWARDS,
        LOW_PUNCH,
        LOW_PUNCH_SPREE,
        BODY_TO_BODY_PUNCH,
        HIGH_PUNCH,
        HIGH_PUNCH_SPREE,
        BODY_TO_BODY_KICK,
        LOW_KICK,
        LOW_SWEEP_KICK,
        HIGH_KICK,
        HIGH_SWEEP_KICK,
        CROUCH,
        UPPERCUT,
        CROUCH_KICK,
        JUMP,
        JUMP_PUNCH,
        JUMP_HIGH_KICK,
        LANDING,
        JUMP_BACK,
        ROLL,
        FORWARD_JUMP_PUNCH,
        JUMP_LOW_KICK,
        TORSO_HIT,
        HEAD_HIT,
        KICKBACK_TORSO_HIT,
        CROUCH_HIT,
        FALL,
        UPPERCUT_HIT,
        NUTS_HIT,
        FALL_INPLACE,
        GETUP,
        CAUGHT,
        THROWN,
        BLOCK,
        CROUCH_BLOCK,
        TURN_RIGHT_TO_LEFT,
        TURN_LEFT_TO_RIGHT,
        SPECIAL_1,
        SPECIAL_2,
        SPECIAL_3,
        GIDDY,
        FINISH_HIM,
        GIDDY_FALL,
        WIN
    };


    /// @brief Enum SpecialAttacks hold the different special attacks types.
    enum class SpecialAttacks
    {
        FIREBALL,
        EXPLOSION,
        NONE
    };

    /// @brief Enum CharacterType hold the different character types.
    enum class CharacterType
    {
        CAGE,
        KANO,
        RAIDEN,
        LIU_KANG,
        SCORPION,
        SUBZERO,
        SONYA,
        GORO,
        SHANG_TSUNG,
        MOSHE,
        ITAMAR,
        YANIV,
        GEFFEN,
        YONATAN
    };

    static constexpr int CHARACTER_SPRITE_SIZE = 46;
    static constexpr int SPECIAL_ATTACK_SPRITE_SIZE = 2;
    static constexpr int WIN_SPRITE_BY_CHARACTER_SIZE = 14;

    /// @brief SpriteInfo struct holds the sprite information.
    struct SpriteInfo {
        int frameCount = 0;
        float x = 0, y = 0;
        float w = 230, h = 220;
    };

    /**
     * @class SpriteData
     * @brief Holds the sprite data for a character.
     *
     * Contains an array of SpriteInfo objects, each representing a different state of the character.
     * Provides access to the sprite data using an enum class as the index.
     */
    template<class T, std::size_t SIZE>
    class SpriteData {
    public:
        explicit constexpr SpriteData(const std::array<SpriteInfo, SIZE>& spriteArray)
                    : sprite(spriteArray) {}

        /// @brief Returns the sprite information for the given state.
        constexpr const SpriteInfo& operator[](const T& s) const {
            return sprite[static_cast<int>(s)];
        }

        /// @brief Returns the sprite information for the given state.
        constexpr SpriteInfo& operator[](T& s) {
            return sprite[static_cast<int>(s)];
        }

    private:
        std::array<SpriteInfo, SIZE> sprite;
    };

    static constexpr std::array<SpriteInfo, CHARACTER_SPRITE_SIZE>
    SUBZERO_SPRITE_ARRAY{{
                {12, 32,   58, 230, 220}, // Stance
                {9,  3074, 58, 230, 220},  // Walk Forwards
                {9,  3074, 58, 230, 220},  // Walk Backwards
                {5,  32,   580, 230, 220},  // Low Punch
                {10, 1436, 580, 230, 220}, // Low Punch Spree
                {5,  4010, 580, 230, 220},  // Body to Body Punch
                {5,  32,   1052, 230, 220},  // High Punch
                {10, 1436, 1052, 230, 220}, // High Punch Spree
                {5,  4010, 1052, 230, 220},  // Body to Body Kick
                {7,  32,   1574, 230, 220},  // Low Kick
                {8,  1904, 1574, 230, 220},  // Lowkick Sweep
                {10, 32,   2046, 230, 220}, // High Kick
                {8,  2606, 2046, 230, 220},  // Highkick Sweep
                {5,  32,   2568, 230, 220},  // Crouch
                {5,  1436, 2568, 230, 220},  // Uppercut
                {5,  2840, 2568, 230, 220},  // Crouch Kick
                {1,  32,   3090, 230, 220},  // Jump
                {5,  500,  3090, 230, 220},  // Jump Punch
                {3,  1904, 3090, 230, 220},  // Jump Highkick
                {3,  2840, 3090, 230, 220},  // Landing
                {1,  32,   3562, 230, 220},  // Jump Back
                {7,  500,  3562, 230, 220},  // Roll
                {5,  2372, 3562, 230, 220},  // Forward Jump Punch
                {5,  3776, 3562, 230, 220},  // Jump Lowkick
                {4,  32,   4084, 230, 220},  // Torso Hit
                {4,  1202, 4084, 230, 220},  // Head Hit
                {6,  2372, 4084, 230, 220},  // Kickback Torso Hit
                {3,  4010, 4084, 230, 220},  // Crouch Hit
                {5,  32,   4606, 230, 220},  // Fall
                {6,  1436, 4606, 230, 220},  // Uppercut Hit
                {6,  3074, 4606, 230, 220},  // Nuts Hit
                {6,  32,   5078, 230, 220},  // Fall Inplace
                {5,  1670, 5078, 230, 220},  // Getup
                {6,  32,   5600, 230, 220},  // Caught
                {7,  1670, 5600, 230, 220},  // Thrown
                {5,  32,   6122, 230, 220},  // Block
                {3,  1436, 6122, 230, 220},  // Crouch Block
                {4,  2372, 6122, 230, 220},  // Turn Right to Left
                {4,  3542, 6122, 230, 220},  // Turn Left to Right
                {10, 32,   6644, 281, 220}, // Special 1
                {3,  3542, 6644, 230, 220},  // Special 2
                {-1, -1,   -1, 230, 220},  // Special 3
                {7,  32,   7166, 230, 220},  // Giddy
                {0,  1904, 7166, 230, 220},  // Finish Him
                {7,  2606, 7166, 230, 220},  // Giddy Fall
                {4,  32,   7688, 230, 220},  // Win
    }};

    static constexpr std::array<SpriteInfo, SPECIAL_ATTACK_SPRITE_SIZE>
    SUBZERO_SPECIAL_SPRITE_ARRAY{{
                {1, 2881, 6720, 80, 28}, // Ice-Ball
                {4, 2964, 6669, 76, 123} // Ice-Ball Hit
    }};

    static constexpr std::array<SpriteInfo, CHARACTER_SPRITE_SIZE>
    LIU_KANG_SPRITE_ARRAY{{
                {8, 32,   58, 230, 220}, // Stance
                {9,  2138, 58, 230, 220},  // Walk Forwards
                {9,  2138, 58, 230, 220},  // Walk Backwards
                {4,  32,   580, 230, 220},  // Low Punch
                {9, 1202, 580, 230, 220}, // Low Punch Spree
                {5,  3542, 580, 230, 220},  // Body to Body Punch
                {4,  32,   1052, 230, 220},  // High Punch
                {9, 1202, 1052, 230, 220}, // High Punch Spree
                {5,  3542, 1052, 230, 220},  // Body to Body Kick
                {7,  32,   1574, 230, 220},  // Low Kick
                {8,  1904, 1574, 230, 220},  // Lowkick Sweep
                {9, 32,   2046, 230, 220}, // High Kick
                {7,  2372, 2046, 230, 220},  // Highkick Sweep
                {5,  32,   2568, 230, 220},  // Crouch
                {5,  1436, 2568, 230, 220},  // Uppercut
                {5,  2840, 2568, 230, 220},  // Crouch Kick
                {1,  32,   3090, 230, 220},  // Jump
                {5,  500,  3090, 230, 220},  // Jump Punch
                {5,  1904, 3090, 230, 220},  // Jump Highkick
                {3,  3308, 3090, 230, 220},  // Landing
                {1,  32,   3562, 230, 220},  // Jump Back
                {7,  500,  3562, 230, 220},  // Roll
                {5,  2372, 3562, 230, 220},  // Forward Jump Punch
                {5,  3776, 3562, 230, 220},  // Jump Lowkick
                {4,  32,   4084, 230, 220},  // Torso Hit
                {4,  1202, 4084, 230, 220},  // Head Hit
                {5,  2372, 4084, 230, 220},  // Kickback Torso Hit
                {3,  3776, 4084, 230, 220},  // Crouch Hit
                {7,  32,   4606, 230, 220},  // Fall
                {8,  1904, 4606, 230, 220},  // Uppercut Hit
                {4,  4010, 4606, 230, 220},  // Nuts Hit
                {6,  32,   5078, 230, 220},  // Fall Inplace
                {7,  1670, 5078, 230, 220},  // Getup
                {7,  32,   5600, 230, 220},  // Caught
                {7,  1904, 5600, 230, 220},  // Thrown
                {5,  32,   6122, 230, 220},  // Block
                {2,  1436, 6122, 230, 220},  // Crouch Block
                {3,  2138, 6122, 230, 220},  // Turn Right to Left
                {3,  3074, 6122, 230, 220},  // Turn Left to Right
                {8, 32,   6644, 230, 220}, // Special 1
                {4,  2606, 6644, 230, 220},  // Special 2
                {-1, -1,   -1, 230, 220},  // Special 3
                {7,  32,   7166, 230, 220},  // Giddy
                {0,  1904, 7166, 230, 220},  // Finish Him
                {7,  2606, 7166, 230, 220},  // Giddy Fall
                {14,  32,   8210, 230, 220},  // Win
    }};

    static constexpr std::array<SpriteInfo, WIN_SPRITE_BY_CHARACTER_SIZE>
    WIN_SPRITE_BY_CHARACTER_ARRAY{{
        {2, 3714, 15, 329, 52}, // Cage
        {2, 3714, 67, 329, 52}, // Kano
        {2, 3714, 119, 329, 52}, // Raiden
        {2, 3714, 171, 329, 52}, // Liu Kang
        {2, 3714, 223, 329, 52}, // Scorpion
        {2, 3714, 275, 329, 52}, // Sub-zero
        {2, 3714, 327, 329, 52}, // Sonya
        {2, 3714, 379, 329, 52}, // Goro
        {2, 3714, 431, 329, 52}, // Shang Tsung
        {2, 3714, 15, 329, 52}, // Moshe = Cage
{2, 3714, 67, 329, 52}, //Itamar = Kano
{2, 3714, 119, 329, 52}, //Yaniv = Raiden
{2, 3714, 223, 329, 52}, //Geffen = Scorpion
{2, 3714, 327, 329, 52}, //Yonatan = Sonya
    }};

    static constexpr std::array<SpriteInfo, SPECIAL_ATTACK_SPRITE_SIZE>
    LIU_KANG_SPECIAL_SPRITE_ARRAY{{
                {1, 1902,   6708, 68, 15}, // Fire-Ball
                {6, 2046, 6665, 65, 87} // Fire-Ball Hit
    }};

    static constexpr SpriteData<State, CHARACTER_SPRITE_SIZE> SUBZERO_SPRITE(SUBZERO_SPRITE_ARRAY);
    static constexpr SpriteData<State, CHARACTER_SPRITE_SIZE> LIU_KANG_SPRITE(LIU_KANG_SPRITE_ARRAY);
    static constexpr SpriteData<SpecialAttacks, SPECIAL_ATTACK_SPRITE_SIZE> SUBZERO_SPECIAL_ATTACK_SPRITE(SUBZERO_SPECIAL_SPRITE_ARRAY);
    static constexpr SpriteData<SpecialAttacks, SPECIAL_ATTACK_SPRITE_SIZE> LIU_SPECIAL_ATTACK_SPRITE(LIU_KANG_SPECIAL_SPRITE_ARRAY);
    static constexpr SpriteData<CharacterType, WIN_SPRITE_BY_CHARACTER_SIZE> WIN_SPRITE(WIN_SPRITE_BY_CHARACTER_ARRAY);
};
