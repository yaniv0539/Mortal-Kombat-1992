/**
 * @file mortal_kombat.h
 * @brief Mortal_Kombat_1992 game engine header file.
 *
 * This file contains the main game class, components
 **/

#pragma once
#include "mortal_kombat_info.h"
#include <functional>
#include <string>
#include <unordered_map>

#include "SDL3/SDL.h"
#include "box2d/box2d.h"
#include "bagel.h"
#include "lib/box2d/src/body.h"

/**
 * @namespace mortal_kombat
 * @brief Contains all core logic and data structures for the Mortal Kombat game.
 */
namespace mortal_kombat
{
    /**
     * @class MK
     * @brief Main game class for Mortal_Kombat_1992.
     *
     * Handles initialization, game loop, systems, and entity/component management.
     */
    class MK
    {
    public:
        /// @brief Constructs the MK game object and starts the game.
        MK() {start();}
        /// @brief Destructor. Cleans up resources.
        ~MK() {destroy();}

        void initialScreen() const;
        std::pair<int, int> chooseFighterScreen() const;

        /// @brief Runs the main game loop.
        void run() const;

        /// @brief Initializes the game.
        void start();

        ///@brief Destroys and cleans up the game.
        void destroy() const;

    private:

        static constexpr int FPS = 60;
        static constexpr float	BOX2D_STEP = 1.f/FPS;

        static constexpr Uint32 FRAME_DELAY = 1000 / FPS;
        static constexpr Uint32 ACTION_FRAME_DELAY = 4;
        static constexpr Uint32 INPUT_FRAME_DELAY = 2;

        static constexpr int WINDOW_WIDTH = 800.0f;
        static constexpr int WINDOW_HEIGHT = 600.0f;
        static constexpr int WINDOW_SCALE = 100.0f;

        static constexpr int BOUNDARY_WIDTH = 500.0f;

        static constexpr float SCALE_CHARACTER = 1.5f;
        static constexpr float CHARACTER_WIDTH = 50;
        static constexpr float CHARACTER_HEIGHT = 135;

        static constexpr int CHAR_SQUARE_WIDTH = 230;
        static constexpr int CHAR_SQUARE_HEIGHT = 220;
        static constexpr int NEXT_FRAME_OFFSET = 4;
        static constexpr int SHADOW_OFFSET = 8;

        static constexpr int NONE = -1;

        static constexpr int PLAYER_1_BASE_X = WINDOW_WIDTH / 4 - (CHAR_SQUARE_WIDTH / 2) - 100;
        static constexpr int PLAYER_2_BASE_X = (WINDOW_WIDTH / 4) * 3 - (CHAR_SQUARE_WIDTH / 2);
        static constexpr int PLAYER_BASE_Y = WINDOW_HEIGHT / 2.0f - 20;

        static constexpr bool LEFT = true;
        static constexpr bool RIGHT = false;

        // Background constants
        // -------------------------------------------------------
        static constexpr int fenceX = 290;
        static constexpr int fenceY = 300;
        static constexpr int fenceW = 300;
        static constexpr int fenceH = 400;

        static constexpr int templeX = 290;
        static constexpr int templeY = 0;
        static constexpr int templeW = 300;
        static constexpr int templeH = 245;
        // --------------------------------------------------------
        static constexpr int numOfFighters = 5;


        SDL_Renderer* ren{};
        mutable SDL_Texture* winTextTexture = nullptr;
        SDL_Window* win{};
        b2WorldId boxWorld{};

        /* =============== Components =============== */
        /// @brief Position component holds the x and y coordinates of an object.
        struct Position {
            float x = 0.0f, y = 0.0f;
        };

        /// @brief Movement component holds the velocity of the entity.
        struct Movement {
            float vx = 0, vy = 0; // Velocity in x and y directions
            void reset() {vx = vy = 0;}
        };

        /// @brief Texture component holds the SDL texture and its rectangle for rendering.
        struct Texture {
            SDL_Texture *tex = nullptr;
            SDL_FRect srcRect = {0, 0, 0, 0}; // Source rectangle for texture
            SDL_FRect rect = {0, 0, 0, 0}; // Destination rectangle for rendering
        };

        /// @brief Collider component holds the physics body and shape.
        struct Collider {
            b2BodyId body = b2_nullBodyId; // Default invalid body ID
            b2ShapeId shape = b2_nullShapeId; // Default invalid shape ID
            bool isPlayerSensor = false; // Whether the collider is touching a player
            bool isLeftBoundarySensor = false; // Whether the collider is touching the Left boundary
            bool isRightBoundarySensor = false; // Whether the collider is touching a Right boundary
        };

        /// @brief Player_state component holds the state for the player.
        struct PlayerState {
            State state = State::STANCE;
            bool direction = RIGHT;
            bool isJumping = false; // Whether the player is jumping
            bool isCrouching = false; // Whether the player is crouching
            bool isAttacking = false; // Whether the player is attacking
            bool isSpecialAttack = false; // Whether the player is attacking with a special attack
            bool isLaying = false; // Whether the player is laying down
            bool busy = false; // Whether the player is busy
            int playerNumber = 1; // Player number (1 or 2)
            int busyFrames = 0; // Total frames spent in the current state
            int currFrame = 0; //  Frames spent in the current state
            int freezeFrame = NONE; // Frame to freeze the player
            int freezeFrameDuration = 0; // Duration of the freeze-frame

            /// @brief Resets the player state to default values.
            void reset()
            {
                state = State::STANCE;
                isJumping = false;
                isCrouching = false;
                isAttacking = false;
                isSpecialAttack = false;
                isLaying = false;
                busy = false;
                busyFrames = 0;
                currFrame = 0;
                freezeFrame = NONE;
                freezeFrameDuration = 0;
            }
        };

        /// @brief Input variable for player's key inputs
        using Input = Uint16;

        /// @brief Inputs component holds the input, and input history for the player.
        struct Inputs {
            static constexpr int MAX_HISTORY = 3;

            Input history[MAX_HISTORY] = {};
            int index = 0;

            static constexpr Input UP = 1;
            static constexpr Input DOWN = 1 << 1;
            static constexpr Input LEFT = 1 << 2;
            static constexpr Input RIGHT = 1 << 3;
            static constexpr Input LOW_PUNCH = 1 << 4;
            static constexpr Input HIGH_PUNCH = 1 << 5;
            static constexpr Input LOW_KICK = 1 << 6;
            static constexpr Input HIGH_KICK = 1 << 7;
            static constexpr Input BLOCK = 1 << 8;
            static constexpr Input DIRECTION_RIGHT = 1 << 9;
            static constexpr Input DIRECTION_LEFT = 1 << 10;
            static constexpr Input JUMPING = 1 << 11;
            static constexpr Input RESET = 0;

            static constexpr Input UPPERCUT = Inputs::DOWN | Inputs::HIGH_PUNCH;
            static constexpr Input CROUCH_KICK = Inputs::DOWN | Inputs::LOW_KICK;
            static constexpr Input LOW_SWEEP_KICK_RIGHT = Inputs::LEFT | Inputs::LOW_KICK | Inputs::DIRECTION_RIGHT;
            static constexpr Input LOW_SWEEP_KICK_LEFT = Inputs::RIGHT | Inputs::LOW_KICK | Inputs::DIRECTION_LEFT;
            static constexpr Input HIGH_SWEEP_KICK_RIGHT = Inputs::LEFT | Inputs::HIGH_KICK | Inputs::DIRECTION_RIGHT;
            static constexpr Input HIGH_SWEEP_KICK_LEFT = Inputs::RIGHT | Inputs::HIGH_KICK | Inputs::DIRECTION_LEFT;
            static constexpr Input CROUCH_BLOCK = Inputs::DOWN | Inputs::BLOCK;
            static constexpr Input WALK_FORWARDS_RIGHT = Inputs::RIGHT | Inputs::DIRECTION_RIGHT;
            static constexpr Input WALK_FORWARDS_LEFT = Inputs::LEFT | Inputs::DIRECTION_LEFT;
            static constexpr Input WALK_BACKWARDS_RIGHT = Inputs::LEFT | Inputs::DIRECTION_RIGHT;
            static constexpr Input WALK_BACKWARDS_LEFT = Inputs::RIGHT | Inputs::DIRECTION_LEFT;
            static constexpr Input JUMP_PUNCH = Inputs::LOW_PUNCH | Inputs::JUMPING;
            static constexpr Input JUMP_HIGH_KICK = Inputs::HIGH_KICK | Inputs::JUMPING;
            static constexpr Input JUMP_LOW_KICK = Inputs::LOW_KICK | Inputs::JUMPING;
            static constexpr Input JUMP_BACK_RIGHT = Inputs::UP | Inputs::LEFT | Inputs::DIRECTION_RIGHT;
            static constexpr Input JUMP_BACK_LEFT = Inputs::UP | Inputs::RIGHT | Inputs::DIRECTION_LEFT;
            static constexpr Input ROLL_RIGHT = Inputs::UP | Inputs::RIGHT | Inputs::DIRECTION_RIGHT;
            static constexpr Input ROLL_LEFT = Inputs::UP | Inputs::LEFT | Inputs::DIRECTION_LEFT;


            /// @brief Checks if the input is in the most recent history has the given input's bits.
            /// @param input bit map of wanted inputs
            bool operator==(const Input input) const
            {
                return (history[index] & input) == input;
            }

            /// @brief Checks if the inputs array of both players has the same input's bits.
            /// @param inputs array containing bit map of wanted inputs
            bool operator==(const Input inputs[]) const
            {
                return ((*this)[0] & inputs[0]) == inputs[0]
                        && ((*this)[1] & inputs[1]) == inputs[1]
                        && ((*this)[2] & inputs[2]) == inputs[2]
                        && inputs[0] != 0;
            }

            /// @brief Returns the input at the given index in the history.
            Input operator[](const int i) const
            {
                return history[(index - i + MAX_HISTORY) % MAX_HISTORY];
            }

            /// @brief Returns the input at the given index in the history.
            /// @param i index of the input
            /// @return reference to the input at the given index.
            Input& operator[](const int i)
            {
                return history[(index - i + MAX_HISTORY) % MAX_HISTORY];
            }

            /// @brief Increments the index and resets the current input.
            /// @return The new index.
            int operator++(int)
            {
                index = (index + 1) % MAX_HISTORY;
                history[index] = Inputs::RESET;
                return index;
            }
        };

        /// @brief Attack component holds the attack type, damage, hitbox, and hitbox type.
        struct Attack {
            State type;
            int attacker;

            static constexpr int ATTACK_LIFE_TIME = 1;
        };

        /// @brief SpecialAttack component holds the special move type and inputs for the attack.
        struct SpecialAttack {
            SpecialAttacks type = SpecialAttacks::NONE;
            bool direction = RIGHT;
            int frame = 0;
            int totalFrames = 0;
            bool explode = false;

            static constexpr int SPECIAL_ATTACK_LIFE_TIME = 70;
        };

        /// @brief Character component holds the character information of the player.
        struct Character {
            static constexpr int SPECIAL_ATTACKS_COUNT = 3;
            static constexpr int COMBO_LENGTH = 3;

            char name[10] = {};
            SpriteData<State, CHARACTER_SPRITE_SIZE> sprite;
            SpriteData<SpecialAttacks, SPECIAL_ATTACK_SPRITE_SIZE> specialAttackSprite; // Updated type
            float specialAttackOffset_y{};
            // SpecialAttack are times 2 because of the direction
            Input specialAttacks[SPECIAL_ATTACKS_COUNT * 2][COMBO_LENGTH] = {};
            SDL_FRect leftBarNameSource{};
            SDL_FRect rightBarNameSource{};
            SpriteInfo winText;
        };

        /// @brief Health component holds the maximum and current health of the player.
        struct Health {
            float max_health = 100.0f;
            float health = 100.0f;
        };

        /// @brief Time component holds the time remaining in the match.
        struct Time {
            float time = 0.0f;
        };

        /// @brief Boundary tag component is used to identify boundary entities.
        struct Boundary
        {
            bool side; // true for left, false for right
        };

        /// @brief DamageVisual holds a delayed representation of health for damage effect.
        struct DamageVisual {
            float trailingHealth = 100.0f;  // Initially equal to max health
        };

        /// @brief HealthBarReference component holds a reference to the actual player entity.
        struct HealthBarReference {
            bagel::ent_type target;  // Reference to actual player entity
        };

        /// @brief Tag to indicate this is a win message UI entity.
        struct WinMessage {
        };

        /* =============== Systems =============== */

        /// @brief Updates the position of entities based on their movement components.
        static void MovementSystem();

        /// @brief returns the position of the entity in the Box2D world.
        static b2Vec2 getPosition(const Position& position)
        {
            return {position.x / WINDOW_SCALE, position.y / WINDOW_SCALE};
        }

        /// @brief returns the position of the entity in the Box2D world.
        static b2Vec2 getPosition(const float x, const float y)
        {
            return {x / WINDOW_SCALE, y / WINDOW_SCALE};
        }

        /// @brief Renders entities with position and texture components to the screen.
        void RenderSystem() const;

        /// @brief Returns the sprite rectangle for a given action and frame.
        /// @param character Character data for the player.
        /// @param action Action state of the character.
        /// @param frame Frame number of the action.
        /// @param shadow Whether to return shadow.
        /// @return SDL_FRect representing the sprite rectangle.
        static SDL_FRect getSpriteFrame(const Character& character, State action,
                                            int frame, bool shadow = false);

        /// @brief Returns the sprite rectangle for a given action and frame.
        /// @param character Character data for the player.
        /// @param action Action state of the SpecialAttack.
        /// @param frame Frame number of the action.
        /// @return SDL_FRect representing the sprite rectangle.
        static SDL_FRect getSpriteFrame(const Character& character, SpecialAttacks action,
                                            int frame);

        /// @brief Returns the sprite rectangle for a given action and frame.
        /// @param character Character data for the player.
        /// @param frame Frame number of the action.
        static SDL_FRect getWinSpriteFrame(const Character &character, int frame) ;

        /// @brief Manages player-specific logic, such as state and character updates.
        void PlayerSystem() const;

        /// @brief Handles collision detection and response for entities with colliders.
        void CollisionSystem() const;

        /// @brief Updates the game clock and manages time-related logic.
        static void ClockSystem();

        /// @brief Processes player inputs and updates input history.
        static void InputSystem();

        /// @brief Manages special attack detection.
        static void SpecialAttackSystem();

        /// @brief Handles combat logic, such has damage application, and player hit state.
        /// @param eAttack Entity representing the attack.
        /// @param ePlayer Entity representing the attacked player.
        static void CombatSystem(bagel::Entity &eAttack, bagel::Entity &ePlayer);

        /// @brief Handles attack's entity destruction and decay logic.
        static void AttackDecaySystem();

        /// @brief Handles and store a cache of SDL textures.
        class TextureSystem
        {
        public:
            enum class IgnoreColorKey
            {
                CHARACTER,
                BACKGROUND,
                NAME_BAR,
                DAMAGE_BAR,
                WIN_TEXT,
            };
            /// @brief Loads a texture from a file and caches it for future use.
            static SDL_Texture* getTexture(SDL_Renderer* renderer, const std::string& filePath, IgnoreColorKey ignoreColorKey);

            /// @brief Clears the texture cache and destroys all cached textures.
            static void clearCache() {
                for (auto& pair : textureCache) {
                    SDL_DestroyTexture(pair.second);
                }
                textureCache.clear();
            }
        private:
            static constexpr Uint8 CHARACTER_COLOR_IGNORE_RED = 165;
            static constexpr Uint8 CHARACTER_COLOR_IGNORE_GREEN = 231;
            static constexpr Uint8 CHARACTER_COLOR_IGNORE_BLUE = 255;

            static constexpr Uint8 BACKGROUND_COLOR_IGNORE_RED = 252;
            static constexpr Uint8 BACKGROUND_COLOR_IGNORE_GREEN = 0;
            static constexpr Uint8 BACKGROUND_COLOR_IGNORE_BLUE = 252;

            // Color key for the bar texture
            static constexpr Uint8 COLOR_KEY_DAMAGE_BAR_RED = 82;
            static constexpr Uint8 COLOR_KEY_DAMAGE_BAR_GREEN = 1;
            static constexpr Uint8 COLOR_KEY_DAMAGE_BAR_BLUE = 1;

            // Color key for the bar texture
            static constexpr Uint8 COLOR_KEY_NAME_BAR_RED = 0;
            static constexpr Uint8 COLOR_KEY_NAME_BAR_GREEN = 165;
            static constexpr Uint8 COLOR_KEY_NAME_BAR_BLUE = 0;

            static constexpr Uint8 WIN_TEXT_COLOR_IGNORE_RED = 245;
            static constexpr Uint8 WIN_TEXT_COLOR_IGNORE_GREEN = 10;
            static constexpr Uint8 WIN_TEXT_COLOR_IGNORE_BLUE = 237;


            static std::unordered_map<std::string, SDL_Texture*> textureCache;
        };

        static void HealthBarSystem();

        /* =============== Entities =============== */
        /// @brief Entity is a unique identifier for each game object.

        /// @brief Creates a player's character (like Scorpion, Sub-Zero, etc.)
        /// @param x,y Position of the entity in the game world.
        /// @param character Character data for the player.
        /// @param playerNumber Player number (1 or 2).
        bagel::ent_type createPlayer(float x, float y, Character character, int playerNumber) const;

        /// @brief Creates an Attack entity (like a punch or kick).
        /// @param x,y Position of the entity in the game world.
        /// @param type Type of the attack.
        /// @param playerNumber Player number (1 or 2).
        /// @param direction attack direction.
        void createAttack(float x, float y, State type, int playerNumber, bool direction) const;

        /// @brief Creates a special attack entity.
        /// @param x,y Position of the entity in the game world.
        /// @param type Type of the special attack.
        /// @param playerNumber Player number (1 or 2).
        /// @param direction attack direction.
        /// @param character Character data for the player.
        void createSpecialAttack(float x, float y, SpecialAttacks type, int playerNumber,
                                bool direction, Character& character) const;

        /// @brief Creates a static platform/boundary.
        /// @param side boundary side (left or right).
        void createBoundary(bool side) const;

        /// @brief Creates a background entity.
        /// @param backgroundName SDL texture for the background.
        inline void createBackground(const std::string& backgroundName) const;

        /// @brief Creates a health bar entity.
        /// tracks the health of the players.
        /// @param player1 Player 1 entity.
        /// @param player2 Player 2 entity.
        void createBar(bagel::Entity player1, bagel::Entity player2) const;

        /// @brief Creates a win text entity.
        /// @param winCharacter Character data for the winning player.
        void createWinText(const Character& winCharacter) const;

        /**
         * @struct Characters
         * @brief Represents the characters in the game.
         *
         * Contains all relevant data for a character, including name, health, position, and state.
         * Used for managing character logic, rendering, and interactions in the game world.
         */
        struct Characters
        {
            constexpr static Character SUBZERO = {
                .name = "Sub-Zero",
                .sprite = SUBZERO_SPRITE,
                .specialAttackSprite = SUBZERO_SPECIAL_ATTACK_SPRITE,
                .specialAttackOffset_y = 88,
                .specialAttacks = {{Inputs::LOW_PUNCH, Inputs::LEFT | Inputs::DIRECTION_RIGHT, Inputs::RIGHT | Inputs::DIRECTION_RIGHT},
                            {Inputs::LOW_PUNCH, Inputs::RIGHT | Inputs::DIRECTION_LEFT, Inputs::LEFT | Inputs::DIRECTION_LEFT}},
                .leftBarNameSource = { 5406, 173, 163, 12 },
                .rightBarNameSource = { 5579, 173, 163, 12 },
                .winText = WIN_SPRITE[CharacterType::SUBZERO],
            };

            constexpr static Character LIU_KANG = {
                .name = "Liu Kang",
                .sprite = LIU_KANG_SPRITE,
                .specialAttackSprite = LIU_SPECIAL_ATTACK_SPRITE,
                .specialAttackOffset_y = 72,
                .specialAttacks = {{Inputs::LOW_PUNCH, Inputs::LEFT | Inputs::DIRECTION_RIGHT, Inputs::RIGHT | Inputs::DIRECTION_RIGHT},
                        {Inputs::LOW_PUNCH, Inputs::RIGHT | Inputs::DIRECTION_LEFT, Inputs::LEFT | Inputs::DIRECTION_LEFT}},
                .leftBarNameSource = { 5406, 142, 163, 12 },
                .rightBarNameSource = { 5579, 142, 163, 12 },
                .winText = WIN_SPRITE[CharacterType::LIU_KANG],
            };

            constexpr static Character MOSHE = {
                .name = "Sub-Moshe",
                .sprite = SUBZERO_SPRITE,
                .specialAttackSprite = SUBZERO_SPECIAL_ATTACK_SPRITE,
                .specialAttackOffset_y = 88,
                .specialAttacks = {{Inputs::LOW_PUNCH, Inputs::LEFT | Inputs::DIRECTION_RIGHT, Inputs::RIGHT | Inputs::DIRECTION_RIGHT},
                            {Inputs::LOW_PUNCH, Inputs::RIGHT | Inputs::DIRECTION_LEFT, Inputs::LEFT | Inputs::DIRECTION_LEFT}},
                .leftBarNameSource = { 5406, 99, 163, 12 }, //Cage
                .rightBarNameSource = { 5579, 99, 163, 12 },
                .winText = WIN_SPRITE[CharacterType::MOSHE],
            };

            constexpr static Character ITAMAR = {
                .name = "Itamar-Fu",
                .sprite = LIU_KANG_SPRITE,
                .specialAttackSprite = LIU_SPECIAL_ATTACK_SPRITE,
                .specialAttackOffset_y = 72,
                .specialAttacks = {{Inputs::LOW_PUNCH, Inputs::LEFT | Inputs::DIRECTION_RIGHT, Inputs::RIGHT | Inputs::DIRECTION_RIGHT},
                        {Inputs::LOW_PUNCH, Inputs::RIGHT | Inputs::DIRECTION_LEFT, Inputs::LEFT | Inputs::DIRECTION_LEFT}},
                .leftBarNameSource = { 5406, 114, 163, 12 }, //Kano
                .rightBarNameSource = { 5579, 114, 163, 12 },
                .winText = WIN_SPRITE[CharacterType::ITAMAR],
            };

            constexpr static Character YANIV = {
                .name = "Yaniv",
                .sprite = SUBZERO_SPRITE,
                .specialAttackSprite = SUBZERO_SPECIAL_ATTACK_SPRITE,
                .specialAttackOffset_y = 88,
                .specialAttacks = {{Inputs::LOW_PUNCH, Inputs::LEFT | Inputs::DIRECTION_RIGHT, Inputs::RIGHT | Inputs::DIRECTION_RIGHT},
                            {Inputs::LOW_PUNCH, Inputs::RIGHT | Inputs::DIRECTION_LEFT, Inputs::LEFT | Inputs::DIRECTION_LEFT}},
                .leftBarNameSource = { 5406, 129, 163, 12 }, //Raiden
                .rightBarNameSource = { 5579, 129, 163, 12 },
                .winText = WIN_SPRITE[CharacterType::YANIV],
            };

            constexpr static Character GEFFEN = {
                .name = "Geffen",
                .sprite = LIU_KANG_SPRITE,
                .specialAttackSprite = LIU_SPECIAL_ATTACK_SPRITE,
                .specialAttackOffset_y = 72,
                .specialAttacks = {{Inputs::LOW_PUNCH, Inputs::LEFT | Inputs::DIRECTION_RIGHT, Inputs::RIGHT | Inputs::DIRECTION_RIGHT},
                        {Inputs::LOW_PUNCH, Inputs::RIGHT | Inputs::DIRECTION_LEFT, Inputs::LEFT | Inputs::DIRECTION_LEFT}},
                .leftBarNameSource = { 5406, 159, 163, 12 }, //Scorpion
                .rightBarNameSource = { 5579, 159, 163, 12 },
                .winText = WIN_SPRITE[CharacterType::GEFFEN],
            };

            constexpr static Character YONATAN = {
                .name = "Yonatan",
                .sprite = SUBZERO_SPRITE,
                .specialAttackSprite = SUBZERO_SPECIAL_ATTACK_SPRITE,
                .specialAttackOffset_y = 88,
                .specialAttacks = {{Inputs::LOW_PUNCH, Inputs::LEFT | Inputs::DIRECTION_RIGHT, Inputs::RIGHT | Inputs::DIRECTION_RIGHT},
                            {Inputs::LOW_PUNCH, Inputs::RIGHT | Inputs::DIRECTION_LEFT, Inputs::LEFT | Inputs::DIRECTION_LEFT}},
                .leftBarNameSource = { 5406, 189, 163, 12 }, //Sonya
                .rightBarNameSource = { 5579, 189, 163, 12 },
                .winText = WIN_SPRITE[CharacterType::YONATAN],
            };

            constexpr static std::array<Character, numOfFighters> ALL_CHARACTERS = {
                MOSHE,
                ITAMAR,
                YANIV,
                GEFFEN,
                YONATAN
            };
        };

    };
}


