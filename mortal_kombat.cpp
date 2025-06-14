#include "mortal_kombat_info.h"
#include "mortal_kombat.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <box2d/box2d.h>

namespace mortal_kombat
{
    std::unordered_map<std::string, SDL_Texture*> MK::TextureSystem::textureCache;

    void MK::start()
    {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cout << SDL_GetError() << std::endl;
            return;
        }

        if (!SDL_CreateWindowAndRenderer(
                "MK1992", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &win, &ren)) {
            std::cout << SDL_GetError() << std::endl;
            return;
                }

        SDL_SetRenderDrawColor(ren, 255,255,255,0);

        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = {0,0};
        boxWorld = b2CreateWorld(&worldDef);

        createBackground("res/Background.png");

        createBoundary(LEFT);
        createBoundary(RIGHT);
        initialScreen();  // Show intro splash before the game loop
        auto [p1Index, p2Index] = chooseFighterScreen();
        Character character1 = Characters::ALL_CHARACTERS[p1Index];
        Character character2 = Characters::ALL_CHARACTERS[p2Index];

        bagel::Entity player1 = createPlayer(PLAYER_1_BASE_X, PLAYER_BASE_Y, character1, 1);
        bagel::Entity player2 = createPlayer(PLAYER_2_BASE_X, PLAYER_BASE_Y, character2, 2);

        createBar(player1, player2);
    }

    void MK::destroy() const
    {
        for (bagel::ent_type e = {0}; e.id <= bagel::World::maxId().id; ++e.id)
        {
            if (bagel::Entity entity{e}; entity.has<Collider>())
            {
                if (b2Body_IsValid(entity.get<Collider>().body)) {
                    const auto* e_p = static_cast<bagel::ent_type*>(b2Body_GetUserData(entity.get<Collider>().body));
                    b2DestroyBody(entity.get<Collider>().body);
                    delete e_p;
                }
                entity.get<Collider>().body = b2_nullBodyId;
                bagel::World::destroyEntity(e);
            }
        }
        TextureSystem::clearCache();
        if (b2World_IsValid(boxWorld))
            b2DestroyWorld(boxWorld);
        if (ren != nullptr)
            SDL_DestroyRenderer(ren);
        if (win != nullptr)
            SDL_DestroyWindow(win);

        SDL_Quit();
    }

    void MK::initialScreen() const {
        SDL_Texture* menuTexture = TextureSystem::getTexture(ren, "res/Menus&Text.png", TextureSystem::IgnoreColorKey::NAME_BAR);
        if (!menuTexture) {
            SDL_Log("Failed to load initial screen texture");
            return;
        }

        // Define the area of the menu texture to show — adjust these values to match your design.
        SDL_FRect srcRect = {2315, 1553, 392, 249}; // Example values — update as needed
        SDL_FRect destRect = {
            0.0f,
            0.0f,
            static_cast<float>(WINDOW_WIDTH),
            static_cast<float>(WINDOW_HEIGHT)
        };

        SDL_Event event;
        bool waitForKey = true;

        while (waitForKey) {
            SDL_PumpEvents();
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    exit(0);
                } else if (event.type == SDL_EVENT_KEY_DOWN) {
                    waitForKey = false;
                }
            }

            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            SDL_RenderClear(ren);
            SDL_RenderTexture(ren, menuTexture, &srcRect, &destRect);
            SDL_RenderPresent(ren);

            SDL_Delay(16); // ~60 FPS
        }
    }

    std::pair<int, int> MK::chooseFighterScreen() const {
    SDL_Texture* menuTexture = TextureSystem::getTexture(
        ren, "res/Menus&Text.png", TextureSystem::IgnoreColorKey::NAME_BAR);
    if (!menuTexture) {
        SDL_Log("Failed to load fighter selection screen");
        return {-1, -1};
    }

    // Load character images
    SDL_Texture* characterTextures[numOfFighters] = {
        TextureSystem::getTexture(ren, "res/moshe_w_pic.png", TextureSystem::IgnoreColorKey::CHARACTER),
        TextureSystem::getTexture(ren, "res/itamar_w_pic.png", TextureSystem::IgnoreColorKey::CHARACTER),
        TextureSystem::getTexture(ren, "res/yaniv_w_pic.png", TextureSystem::IgnoreColorKey::CHARACTER),
        TextureSystem::getTexture(ren, "res/geffen_w_pic.png", TextureSystem::IgnoreColorKey::CHARACTER),
        TextureSystem::getTexture(ren, "res/yonatan_w_pic.png", TextureSystem::IgnoreColorKey::CHARACTER)
    };

    SDL_FRect srcRect = {900, 381, 64*numOfFighters + 10, 183};
    SDL_FRect destRect = {
        0.0f,
        0.0f,
        static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT)
    };

    const float boxX = 903.0f - 900.0f;
    const float boxY = 409.0f - 381.0f;
    const float boxW = 65.0f;
    const float boxH = 80.0f;

    const float scaleX = destRect.w / srcRect.w;
    const float scaleY = destRect.h / srcRect.h;

    const float scaledBoxW = boxW * scaleX;
    const float scaledBoxH = boxH * scaleY;
    const float startX = boxX * scaleX;
    const float startY = boxY * scaleY;

    constexpr int GRID_COLS = numOfFighters;

    int selectedP1 = 0;  // Top row (0 - numOfFighters-1)
    int selectedP2 = numOfFighters;  // Bottom row (numOfFighters - 2numOfFighters-1)

    SDL_Event event;
    bool choosing = true;

    while (choosing) {
        SDL_PumpEvents();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) exit(0);
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_LEFT:
                        if (selectedP1 % GRID_COLS > 0) selectedP1--;
                        break;
                    case SDLK_RIGHT:
                        if (selectedP1 % GRID_COLS < GRID_COLS - 1) selectedP1++;
                        break;
                    case SDLK_A:
                        if (selectedP2 % GRID_COLS > 0) selectedP2--;
                        break;
                    case SDLK_D:
                        if (selectedP2 % GRID_COLS < GRID_COLS - 1) selectedP2++;
                        break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        choosing = false;
                        break;
                    case SDLK_ESCAPE:
                        exit(0);
                }
            }
        }

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        SDL_RenderTexture(ren, menuTexture, &srcRect, &destRect);

        // Draw character textures in grid
        for (int i = 0; i < numOfFighters; ++i) {
            int col = i % GRID_COLS;

            // Top row (Player 1)
            SDL_FRect dstTop = {
                startX + col * scaledBoxW,
                startY + 0 * scaledBoxH,  // row 0
                scaledBoxW,
                scaledBoxH
            };
            SDL_RenderTexture(ren, characterTextures[i], nullptr, &dstTop);

            // Bottom row (Player 2)
            SDL_FRect dstBottom = {
                startX + col * scaledBoxW,
                startY + 1 * scaledBoxH,  // row 1
                scaledBoxW,
                scaledBoxH
            };
            SDL_RenderTexture(ren, characterTextures[i], nullptr, &dstBottom);
        }

        // Draw Player 1 highlight (red)
        {
            int row = selectedP1 / GRID_COLS;
            int col = selectedP1 % GRID_COLS;

            SDL_FRect highlightRect = {
                startX + col * scaledBoxW,
                startY + row * scaledBoxH,
                scaledBoxW,
                scaledBoxH
            };

            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
            for (int i = 0; i < 4; ++i) {
                SDL_FRect r = {
                    highlightRect.x + i,
                    highlightRect.y + i,
                    highlightRect.w - 2 * i,
                    highlightRect.h - 2 * i
                };
                SDL_RenderRect(ren, &r);
            }
        }

        // Draw Player 2 highlight (yellow)
        {
            int row = selectedP2 / GRID_COLS;
            int col = selectedP2 % GRID_COLS;

            SDL_FRect highlightRect = {
                startX + col * scaledBoxW,
                startY + row * scaledBoxH,
                scaledBoxW,
                scaledBoxH
            };

            SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
            for (int i = 0; i < 4; ++i) {
                SDL_FRect r = {
                    highlightRect.x + i,
                    highlightRect.y + i,
                    highlightRect.w - 2 * i,
                    highlightRect.h - 2 * i
                };
                SDL_RenderRect(ren, &r);
            }
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    // Return column index in each row
    return {selectedP1 % GRID_COLS, selectedP2 % GRID_COLS};
}








    // ------------------------------- Game Loop -------------------------------

    void MK::run() const
    {
        int frame_count = 0;
        while (true)
        {
            Uint32 frameStart = SDL_GetTicks();

            if (frame_count % INPUT_FRAME_DELAY == 0) InputSystem();
            if (++frame_count % ACTION_FRAME_DELAY == 0) PlayerSystem();
            ClockSystem();
            CollisionSystem();
            SpecialAttackSystem();
            MovementSystem();
            RenderSystem();
            HealthBarSystem();
            AttackDecaySystem();

            if (Uint32 frameTime = SDL_GetTicks() - frameStart; FRAME_DELAY > frameTime) {
                SDL_Delay(FRAME_DELAY - frameTime);
            }
        }
    }

    // ------------------------------- Systems -------------------------------

    void MK::MovementSystem()
    {
        static constexpr float WALK_SPEED_BACKWARDS = 3.0f * SCALE_CHARACTER;
        static constexpr float WALK_SPEED_FORWARDS = 4.0f * SCALE_CHARACTER;
        static constexpr float KICKBACK_SPEED = 3.0f * SCALE_CHARACTER;
        static constexpr float FALL_SPEED = 4.0f * SCALE_CHARACTER;

        static constexpr float JUMP_INITIAL_VELOCITY = -14.0f * SCALE_CHARACTER;
        static constexpr float GRAVITY = 0.7f * SCALE_CHARACTER;
        static constexpr float JUMP_HORIZONTAL_SPEED = 4.0f * SCALE_CHARACTER;
        static constexpr float FLOOR_Y = PLAYER_BASE_Y;  // Base floor position

        static const bagel::Mask mask = bagel::MaskBuilder()
            .set<Position>()
            .set<Movement>()
            .set<Collider>()
            .build();

        static const bagel::Mask maskPlayer = bagel::MaskBuilder()
            .set<PlayerState>()
            .set<Character>()
            .build();

        for (bagel::ent_type e = {0}; e.id <= bagel::World::maxId().id; ++e.id)
        {
            if (bagel::Entity entity{e}; entity.test(mask))
            {
                auto& position = entity.get<Position>();
                auto& movement = entity.get<Movement>();
                auto& collider = entity.get<Collider>();

                if (entity.test(maskPlayer))
                {
                    auto& playerState = entity.get<PlayerState>();
                    auto& character = entity.get<Character>();

                    switch (playerState.state)
                    {
                    case State::WALK_BACKWARDS:
                        movement.vx = WALK_SPEED_BACKWARDS
                                        * (playerState.direction == LEFT ? 1.0f : -1.0f)
                                        * (collider.isRightBoundarySensor && playerState.direction == LEFT ? 0.0f : 1.0f)
                                        * (collider.isLeftBoundarySensor && playerState.direction == RIGHT ? 0.0f : 1.0f);

                        break;
                    case State::WALK_FORWARDS:
                        movement.vx = WALK_SPEED_FORWARDS
                                        * (playerState.direction == LEFT ? -1.0f : 1.0f)
                                        * (collider.isPlayerSensor ? 0.0f : 1.0f)
                                        * (collider.isRightBoundarySensor && playerState.direction == RIGHT ? 0.0f : 1.0f)
                                        * (collider.isLeftBoundarySensor && playerState.direction == LEFT ? 0.0f : 1.0f);
;
                        break;
                    case State::KICKBACK_TORSO_HIT:
                        movement.vx = KICKBACK_SPEED
                                        * (playerState.direction == LEFT ? 1.0f : -1.0f);
                        break;
                    case State::JUMP:
                        if (!playerState.isJumping) {
                            playerState.isJumping = true;
                            movement.vy = JUMP_INITIAL_VELOCITY;
                        }
                        movement.vx = 0; // Vertical jump - no horizontal movement
                        break;
                    case State::JUMP_BACK:
                        if (!playerState.isJumping) {
                            playerState.isJumping = true;
                            movement.vy = JUMP_INITIAL_VELOCITY;
                        }
                        movement.vx = JUMP_HORIZONTAL_SPEED
                                    * (playerState.direction == LEFT ? 1.0f : -1.0f)
                                    * (collider.isRightBoundarySensor && playerState.direction == LEFT ? 0.0f : 1.0f)
                                    * (collider.isLeftBoundarySensor && playerState.direction == RIGHT ? 0.0f : 1.0f);
                        break;
                    case State::ROLL:
                        // Forward jump
                        if (!playerState.isJumping) {
                            playerState.isJumping = true;
                            movement.vy = JUMP_INITIAL_VELOCITY;
                        }
                        movement.vx = JUMP_HORIZONTAL_SPEED
                                    * (playerState.direction == LEFT ? -1.0f : 1.0f)
                                    * (collider.isRightBoundarySensor && playerState.direction == RIGHT ? 0.0f : 1.0f)
                                    * (collider.isLeftBoundarySensor && playerState.direction == LEFT ? 0.0f : 1.0f);
                        break;
                    case State::UPPERCUT_HIT:
                        if (playerState.currFrame < character.sprite[playerState.state].frameCount / 2)
                        {
                            movement.vx = FALL_SPEED
                                        * (playerState.direction == LEFT ? 1.0f : -1.0f);
                            break;
                        }
                        movement.reset();
                        break;
                    default:
                        if (!playerState.isJumping)
                            movement.reset();
                        break;
                    }

                    // Apply gravity and handle jumping physics
                    if (playerState.isJumping)
                    {
                        // Apply gravity to vertical velocity
                        movement.vy += GRAVITY;

                        // Check if player has landed
                        if (position.y + movement.vy >= FLOOR_Y) {
                            position.y = FLOOR_Y;
                            movement.vy = 0;
                            playerState.isJumping = false;

                            // Change state to landing animation when landing
                            if (playerState.state == State::JUMP ||
                                playerState.state == State::JUMP_BACK ||
                                playerState.state == State::ROLL ||
                                playerState.state == State::JUMP_PUNCH ||
                                playerState.state == State::JUMP_HIGH_KICK ||
                                playerState.state == State::JUMP_LOW_KICK)
                            {
                                playerState.reset();
                                playerState.state = State::LANDING;
                                playerState.currFrame = 0;
                                playerState.busy = true;
                                playerState.busyFrames = character.sprite[State::LANDING].frameCount;
                            }
                        }
                    }
                }

                position.x += movement.vx;
                position.y += movement.vy;

                if (entity.has<PlayerState>() && entity.get<PlayerState>().isCrouching)
                {
                    b2Body_SetTransform(
                            collider.body,
                            getPosition(position.x, position.y - (CHARACTER_HEIGHT/2.0f)),
                            b2Rot_identity);
                }
                else if (entity.has<SpecialAttack>())
                {
                    auto& sprite = entity.get<Character>().specialAttackSprite[entity.get<SpecialAttack>().type];
                    b2Body_SetTransform(
                            collider.body,
                            getPosition(position.x - sprite.w, position.y - entity.get<Character>().specialAttackOffset_y + (sprite.h / 2.0f)), // NOLINT(*-narrowing-conversions)
                            b2Rot_identity);
                }
                else
                {
                    b2Body_SetTransform(
                              collider.body,
                              getPosition(position),
                              b2Rot_identity);
                }
            }
        }
    }

    void MK::RenderSystem() const
    {
        static const bagel::Mask mask = bagel::MaskBuilder()
            .set<Position>()
            .set<Texture>()
            .build();

        static const bagel::Mask maskPlayer = bagel::MaskBuilder()
            .set<PlayerState>()
            .set<Health>()
            .set<Character>()
            .build();

        static const bagel::Mask maskSpecialAttack = bagel::MaskBuilder()
            .set<SpecialAttack>()
            .set<Character>()
            .build();

        static const bagel::Mask maskWin = bagel::MaskBuilder()
            .set<Position>()
            .set<WinMessage>()
            .set<Texture>()
            .set<Time>()
            .build();

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                exit(0);
            }
        }
        SDL_RenderClear(ren);

        for (bagel::ent_type e = {0}; e.id <= bagel::World::maxId().id; ++e.id)
        {
            if (bagel::Entity entity{e}; entity.test(mask))
            {
                SDL_FlipMode flipMode = SDL_FLIP_NONE;

                auto& position = entity.get<Position>();
                auto& texture = entity.get<Texture>();

                if (entity.test(maskPlayer)) {
                    auto& playerState = entity.get<PlayerState>();
                    auto& character = entity.get<Character>();

                    const int frame = (playerState.state == State::WALK_BACKWARDS)
                        ? (playerState.busyFrames - (playerState.currFrame % playerState.busyFrames)): (playerState.currFrame);

                    flipMode = (playerState.direction == LEFT) ?
                        SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

                    texture.srcRect = getSpriteFrame(character, playerState.state, frame);
                    texture.rect.w = static_cast<float>((character.sprite[playerState.state].w)) * SCALE_CHARACTER;
                    texture.rect.h = static_cast<float>((character.sprite[playerState.state].h)) * SCALE_CHARACTER;
                }
                else if (entity.test(maskSpecialAttack))
                {
                    auto& specialAttack = entity.get<SpecialAttack>();
                    auto& character = entity.get<Character>();
                    flipMode = (specialAttack.direction == LEFT) ?
                        SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

                    texture.srcRect = getSpriteFrame(character, specialAttack.type, specialAttack.frame);
                    texture.rect.w = static_cast<float>((character.specialAttackSprite[specialAttack.type].w)) * SCALE_CHARACTER;
                    texture.rect.h = static_cast<float>((character.specialAttackSprite[specialAttack.type].h)) * SCALE_CHARACTER;
                }
                else if (entity.test(maskWin))
                {
                    auto& character = entity.get<Character>();
                    texture.srcRect = getWinSpriteFrame(character, (static_cast<int>(entity.get<Time>().time) / 16));
                    texture.rect.w = static_cast<float>((character.winText.w)) * SCALE_CHARACTER;
                    texture.rect.h = static_cast<float>((character.winText.h)) * SCALE_CHARACTER;
                }

                texture.rect.x = position.x;
                texture.rect.y = position.y;

                SDL_RenderTextureRotated(
                    ren, texture.tex, &texture.srcRect, &texture.rect, 0,
                    nullptr, flipMode);
            }
        }

        SDL_RenderPresent(ren);
    }

    SDL_FRect MK::getSpriteFrame(const Character& character, State action, const int frame,
                                               const bool shadow)
    {
        return {static_cast<float>(character.sprite[action].x
                    + ((frame % character.sprite[(action)].frameCount)
                    * (NEXT_FRAME_OFFSET + character.sprite[action].w))) + 1
                ,static_cast<float>(character.sprite[action].y
                    + (shadow ? (SHADOW_OFFSET + character.sprite[action].h) : 0)) + 1
                ,static_cast<float>(character.sprite[action].w) - 2
                ,static_cast<float>(character.sprite[action].h) - 2};
    }

    SDL_FRect MK::getSpriteFrame(const Character& character, SpecialAttacks action, int frame)
    {
        return {static_cast<float>(character.specialAttackSprite[action].x
                    + ((frame % character.specialAttackSprite[(action)].frameCount)
                    * (NEXT_FRAME_OFFSET + character.specialAttackSprite[action].w))) + 1
                ,static_cast<float>(character.specialAttackSprite[action].y) + 1
                ,static_cast<float>(character.specialAttackSprite[action].w) - 2
                ,static_cast<float>(character.specialAttackSprite[action].h) - 2};
    }

    SDL_FRect MK::getWinSpriteFrame(const Character& character, const int frame)
    {
        return {static_cast<float>(character.winText.x
                    + ((frame % character.winText.frameCount)
                    * (NEXT_FRAME_OFFSET + character.winText.w))) + 2
                ,static_cast<float>(character.winText.y) + 2
                ,static_cast<float>(character.winText.w) - 4
                ,static_cast<float>(character.winText.h) - 4};
    }

    void MK::PlayerSystem() const
    {
        static const bagel::Mask mask = bagel::MaskBuilder()
            .set<Inputs>()
            .set<PlayerState>()
            .set<Character>()
            .build();

        bagel::ent_type player1Entity{}, player2Entity{};
        bool foundPlayer1 = false, foundPlayer2 = false;

        // Helper lambda to map inputs to state
        auto getStateFromInputs = [](const Inputs& inputs, const Character& character, State& state, int& freezeFrame,
                                     int& freezeFrameDuration, bool& busy, bool& crouching, bool& attack, bool& special,
                                     bool& jumping)
        {
            for (int i = 0; i < Character::SPECIAL_ATTACKS_COUNT && !special; ++i)
            {
                if (inputs == character.specialAttacks[i]
                        || inputs == character.specialAttacks[i + 1])
                {
                    state = static_cast<State>(i + static_cast<int>(State::SPECIAL_1));
                    attack = true;
                    special = true;
                    return;
                }
            }

            if (inputs == Inputs::JUMP_PUNCH)
            {
                state = State::JUMP_PUNCH;
                freezeFrame = character.sprite[state].frameCount - 1;
                attack = true;
                jumping = true;
            }
            else if (inputs == Inputs::JUMP_LOW_KICK)
            {
                state = State::JUMP_LOW_KICK;
                freezeFrame = character.sprite[state].frameCount - 1;
                attack = true;
                jumping = true;
            }
            else if (inputs == Inputs::JUMP_HIGH_KICK)
            {
                state = State::JUMP_HIGH_KICK;
                freezeFrame = character.sprite[state].frameCount - 1;
                attack = true;
                jumping = true;
            }
            else if (inputs == Inputs::CROUCH_BLOCK)
            {
                state = State::CROUCH_BLOCK;
                freezeFrame = character.sprite[state].frameCount / 2 + 1;
                freezeFrameDuration = 1;
                crouching = true;
            }
            else if (inputs == Inputs::BLOCK)
            {
                state = State::BLOCK;
                freezeFrame = character.sprite[state].frameCount / 2 + 1;
                freezeFrameDuration = 1;
            }
            else if (inputs == Inputs::CROUCH_KICK)
            {
                state = State::CROUCH_KICK;
                crouching = true;
                attack = true;
            }
            else if (inputs == Inputs::JUMP_BACK_RIGHT
                    || inputs == Inputs::JUMP_BACK_LEFT)
            {
                state = State::JUMP_BACK;
                busy = false;
            }
            else if (inputs == Inputs::ROLL_RIGHT
                    || inputs == Inputs::ROLL_LEFT)
            {
                state = State::ROLL;
                busy = false;
            }
            else if (inputs == Inputs::UP)
            {
                state = State::JUMP;
                busy = false;
            }
            else if (inputs == Inputs::HIGH_SWEEP_KICK_LEFT
                    || inputs == Inputs::HIGH_SWEEP_KICK_RIGHT)
            {
                state = State::HIGH_SWEEP_KICK;
                attack = true;
            }
            else if (inputs == Inputs::LOW_SWEEP_KICK_LEFT
                    || inputs == Inputs::LOW_SWEEP_KICK_RIGHT)
            {
                state = State::LOW_SWEEP_KICK;
                attack = true;
            }
            else if (inputs == Inputs::UPPERCUT)
            {
                state = State::UPPERCUT;
                crouching = true;
                attack = true;
            }
            else if (inputs == Inputs::DOWN)
            {
                state = State::CROUCH;
                freezeFrame = (character.sprite[state].frameCount / 2) + 1;
                freezeFrameDuration = 1;
                crouching = true;
            }
            else if (inputs == Inputs::LOW_PUNCH)
            {
                state = State::LOW_PUNCH;
                attack = true;
            }
            else if (inputs == Inputs::HIGH_PUNCH)
            {
                state = State::HIGH_PUNCH;
                attack = true;
            }
            else if (inputs == Inputs::LOW_KICK)
            {
                state = State::LOW_KICK;
                attack = true;
            }
            else if (inputs == Inputs::HIGH_KICK)
            {
                state = State::HIGH_KICK;
                attack = true;
            }
            else if (inputs == Inputs::WALK_BACKWARDS_RIGHT
                    || inputs == Inputs::WALK_BACKWARDS_LEFT)
            {
                state = State::WALK_BACKWARDS;
                busy = false;
            }
            else if (inputs == Inputs::WALK_FORWARDS_RIGHT
                    || inputs == Inputs::WALK_FORWARDS_LEFT)
            {
                state = State::WALK_FORWARDS;
                busy = false;
            }
            else
            {
                state = State::STANCE;
                busy = false;
            }
        };

        for (bagel::ent_type e = {0}; e.id <= bagel::World::maxId().id; ++e.id)
        {
            if (bagel::Entity entity{e}; entity.test(mask))
            {
                auto& inputs = entity.get<Inputs>();
                auto& playerState = entity.get<PlayerState>();
                auto& character = entity.get<Character>();

                if (playerState.playerNumber == 1) { player1Entity = e; foundPlayer1 = true; }
                else if (playerState.playerNumber == 2) { player2Entity = e; foundPlayer2 = true; }

                // State variables
                State state = State::STANCE;
                int freezeFrame = NONE, freezeFrameDuration = 0;
                bool busy = true, crouching = false, attack = false, special = false, jumping = false;

                // Use helper to determine state and flags
                getStateFromInputs(inputs, character, state, freezeFrame,
                                    freezeFrameDuration, busy, crouching,
                                    attack, special, jumping);

                // Handle busy state and transitions
                if (playerState.busyFrames - 1 <= playerState.currFrame && playerState.freezeFrameDuration <= 0)
                    playerState.busy = false;

                if (playerState.isLaying && !playerState.busy)
                {
                    playerState.reset();
                    playerState.state = State::GETUP;
                    playerState.busyFrames = character.sprite[playerState.state].frameCount;
                    playerState.busy = true;
                }

                // State change logic
                bool shouldChangeState = ((!playerState.busy && (state != playerState.state || attack))
                    || (playerState.state == State::CROUCH && crouching && state != State::CROUCH))
                    && (!playerState.isJumping || jumping);

                if (shouldChangeState)
                {
                    playerState.reset();
                    playerState.state = state;
                    playerState.currFrame = (playerState.isCrouching && state == State::CROUCH) ? 2 : 0;
                    playerState.busyFrames = character.sprite[playerState.state].frameCount;
                    playerState.freezeFrame = freezeFrame;
                    playerState.freezeFrameDuration = freezeFrameDuration;
                    playerState.isJumping = jumping;
                    playerState.isCrouching = crouching;
                    playerState.isAttacking = attack;
                    playerState.isSpecialAttack = special;
                    playerState.busy = busy;
                }

                // Freeze frame logic
                if (playerState.freezeFrame != NONE && state == playerState.state)
                    ++playerState.freezeFrameDuration;

                if (playerState.freezeFrame != NONE
                    && playerState.currFrame + 1 >= playerState.freezeFrame
                    && playerState.freezeFrameDuration > 0)
                {
                    --playerState.freezeFrameDuration;
                    playerState.currFrame = playerState.freezeFrame;
                }
                else if (!playerState.isJumping || playerState.currFrame < playerState.busyFrames-1)
                    ++playerState.currFrame;

                // Attack creation
                if (playerState.busy && playerState.isAttacking)
                {
                    auto& [x, y] = entity.get<Position>();
                    if (playerState.isSpecialAttack
                        && (playerState.currFrame % character.sprite[playerState.state].frameCount) == character.sprite[playerState.state].frameCount / 2)
                        createSpecialAttack(x, y, SpecialAttacks::FIREBALL, playerState.playerNumber, playerState.direction, character);
                    else if (playerState.isJumping
                            || (playerState.currFrame % character.sprite[playerState.state].frameCount) == character.sprite[playerState.state].frameCount / 3)
                        createAttack(x, y, playerState.state, playerState.playerNumber, playerState.direction);
                }
            }
        }

        // Update directions and win/lose states
        if (foundPlayer1 && foundPlayer2) {
            bagel::Entity player1{player1Entity};
            bagel::Entity player2{player2Entity};
            auto& p1State = player1.get<PlayerState>();
            auto& p2State = player2.get<PlayerState>();
            auto& p1Health = player1.get<Health>();
            auto& p2Health = player2.get<Health>();
            auto& p1Char = player1.get<Character>();
            auto& p2Char = player2.get<Character>();

            auto handleWinLose = [&](const bagel::Entity& loser, const bagel::Entity& winner) {
                createWinText(winner.get<Character>());
                bool isJumping = loser.get<PlayerState>().isJumping;
                loser.get<PlayerState>().reset();
                loser.get<PlayerState>().state = State::GIDDY_FALL;
                loser.get<PlayerState>().busy = true;
                loser.get<PlayerState>().isJumping = isJumping;
                loser.get<PlayerState>().busyFrames = loser.get<Character>().sprite[loser.get<PlayerState>().state].frameCount;
                loser.get<PlayerState>().freezeFrame = loser.get<PlayerState>().busyFrames - 1;
                loser.get<PlayerState>().freezeFrameDuration = 1000;

                isJumping = winner.get<PlayerState>().isJumping;
                winner.get<PlayerState>().reset();
                winner.get<PlayerState>().state = State::WIN;
                winner.get<PlayerState>().busy = true;
                winner.get<PlayerState>().isJumping = isJumping;
                winner.get<PlayerState>().busyFrames = winner.get<Character>().sprite[winner.get<PlayerState>().state].frameCount;
                winner.get<PlayerState>().freezeFrame = winner.get<PlayerState>().busyFrames - 1;
                winner.get<PlayerState>().freezeFrameDuration = 1000;
            };

            if (p1Health.health <= 0 && p1State.state != State::GIDDY_FALL)
                handleWinLose(player1, player2);
            if (p2Health.health <= 0 && p2State.state != State::GIDDY_FALL)
                handleWinLose(player2, player1);

            // Direction update
            bool isPlayer1Direction = player1.get<Position>().x < player2.get<Position>().x ? RIGHT : LEFT;
            bool isPlayer2Direction = !isPlayer1Direction;
            auto updateDirection = [](const bagel::Entity& player, bool newDir, const Character& character) {
                auto& state = player.get<PlayerState>();
                if (!state.isJumping && !state.busy && state.direction != newDir) {
                    state.direction = newDir;
                    state.reset();
                    state.state = State::TURN_LEFT_TO_RIGHT;
                    state.busy = true;
                    state.busyFrames = character.sprite[state.state].frameCount;
                }
            };
            updateDirection(player1, isPlayer1Direction, p1Char);
            updateDirection(player2, isPlayer2Direction, p2Char);
        }
    }


    void MK::InputSystem() {
        static const bagel::Mask mask = bagel::MaskBuilder()
            .set<Inputs>()
            .build();

        SDL_PumpEvents();
        auto keyboardState = SDL_GetKeyboardState(nullptr);

        if (keyboardState[SDL_SCANCODE_ESCAPE])
        {
            exit(0);
        }

        for (bagel::ent_type e = {0}; e.id <= bagel::World::maxId().id; ++e.id)
        {
            if (bagel::Entity entity{e}; entity.test(mask))
            {
                auto& inputs = entity.get<Inputs>();
                auto& playerState = entity.get<PlayerState>();

                inputs++;

                inputs[0] |= (playerState.direction == LEFT) ?
                                                Inputs::DIRECTION_LEFT : Inputs::DIRECTION_RIGHT;
                inputs[0] |= (playerState.isJumping) ?
                                                Inputs::JUMPING : 0;

                // Player 1 controls (using WASD for movement, space, etc. for actions)
                if (playerState.playerNumber == 1) {
                    inputs[0] |=
                        (keyboardState[SDL_SCANCODE_H] ? Inputs::BLOCK : 0)
                         | (keyboardState[SDL_SCANCODE_W] ? Inputs::UP : 0)
                         | (keyboardState[SDL_SCANCODE_S] ? Inputs::DOWN : 0)
                         | (keyboardState[SDL_SCANCODE_A] ? Inputs::LEFT : 0)
                         | (keyboardState[SDL_SCANCODE_D] ? Inputs::RIGHT : 0)
                         | (keyboardState[SDL_SCANCODE_F] ? Inputs::LOW_PUNCH : 0)
                         | (keyboardState[SDL_SCANCODE_R] ? Inputs::HIGH_PUNCH : 0)
                         | (keyboardState[SDL_SCANCODE_G] ? Inputs::LOW_KICK : 0)
                         | (keyboardState[SDL_SCANCODE_T] ? Inputs::HIGH_KICK : 0);
                }
                // Player 2 controls (using arrow keys and numpad)
                else if (playerState.playerNumber == 2) {
                    inputs[0] |=
                        (keyboardState[SDL_SCANCODE_APOSTROPHE] ? Inputs::BLOCK : 0)
                         | (keyboardState[SDL_SCANCODE_UP] ? Inputs::UP : 0)
                         | (keyboardState[SDL_SCANCODE_DOWN] ? Inputs::DOWN : 0)
                         | (keyboardState[SDL_SCANCODE_LEFT] ? Inputs::LEFT : 0)
                         | (keyboardState[SDL_SCANCODE_RIGHT] ? Inputs::RIGHT : 0)
                         | (keyboardState[SDL_SCANCODE_K] ? Inputs::LOW_PUNCH : 0)
                         | (keyboardState[SDL_SCANCODE_I] ? Inputs::HIGH_PUNCH : 0)
                         | (keyboardState[SDL_SCANCODE_L] ? Inputs::LOW_KICK : 0)
                         | (keyboardState[SDL_SCANCODE_O] ? Inputs::HIGH_KICK : 0);
                }
            }
        }
    }

    void MK::CollisionSystem() const
    {

        static const bagel::Mask maskAttack = bagel::MaskBuilder()
            .set<Attack>()
            .build();

        static const bagel::Mask maskPlayer = bagel::MaskBuilder()
            .set<PlayerState>()
            .build();

        b2World_Step(boxWorld, BOX2D_STEP, 4);

        const auto se = b2World_GetSensorEvents(boxWorld);

        for (int i = 0; i < se.beginCount; ++i) {
            if (!b2Shape_IsValid(se.beginEvents[i].visitorShapeId)) continue;
            b2BodyId b = b2Shape_GetBody(se.beginEvents[i].visitorShapeId);
            const auto* e_b = static_cast<bagel::ent_type*>(b2Body_GetUserData(b));
            if (!b2Shape_IsValid(se.beginEvents[i].sensorShapeId)) continue;
            b2BodyId s = b2Shape_GetBody(se.beginEvents[i].sensorShapeId);
            const auto* e_s = static_cast<bagel::ent_type*>(b2Body_GetUserData(s));
            if (!e_b || !e_s) continue;

            bagel::Entity eBody = bagel::Entity{(*e_s)};
            bagel::Entity eSensor = bagel::Entity{(*e_b)};

            if (eBody.test(maskPlayer) && eSensor.test(maskPlayer))
                eBody.get<Collider>().isPlayerSensor = true;

            if (eBody.test(maskPlayer) && eSensor.has<Boundary>())
            {
                if (eSensor.get<Boundary>().side == LEFT)
                    eBody.get<Collider>().isLeftBoundarySensor = true;
                if (eSensor.get<Boundary>().side == RIGHT)
                    eBody.get<Collider>().isRightBoundarySensor = true;
            }

            if (eSensor.test(maskAttack) && eBody.test(maskPlayer)
                && eSensor.get<Attack>().attacker != eBody.get<PlayerState>().playerNumber)
                CombatSystem(eSensor, eBody);
        }

        // Handle end events
        for (int i = 0; i < se.endCount; ++i) {
            if (!b2Shape_IsValid(se.endEvents[i].visitorShapeId)) continue;
            b2BodyId b = b2Shape_GetBody(se.endEvents[i].visitorShapeId);
            const auto* e_b = static_cast<bagel::ent_type*>(b2Body_GetUserData(b));
            if (!b2Shape_IsValid(se.endEvents[i].sensorShapeId)) continue;
            b2BodyId s = b2Shape_GetBody(se.endEvents[i].sensorShapeId);
            const auto* e_s = static_cast<bagel::ent_type*>(b2Body_GetUserData(s));
            if (!e_b || !e_s) continue;

            bagel::Entity eBody = bagel::Entity{(*e_s)};
            bagel::Entity eSensor = bagel::Entity{(*e_b)};

            if (eBody.test(maskPlayer) && eSensor.test(maskPlayer))
                eBody.get<Collider>().isPlayerSensor = false;

            if (eBody.test(maskPlayer) && eSensor.has<Boundary>())
            {
                if (eSensor.get<Boundary>().side == LEFT)
                    eBody.get<Collider>().isLeftBoundarySensor = false;
                if (eSensor.get<Boundary>().side == RIGHT)
                    eBody.get<Collider>().isRightBoundarySensor = false;
            }
        }
    }

    void MK::ClockSystem() {
        static const bagel::Mask mask = bagel::MaskBuilder()
            .set<Time>()
            .build();

        for (bagel::ent_type e = {0}; e.id <= bagel::World::maxId().id; ++e.id)
        {
            if (bagel::Entity entity{e}; entity.test(mask))
            {
                --entity.get<Time>().time;
            }
        }
    }

    void MK::CombatSystem(bagel::Entity &eAttack, bagel::Entity &ePlayer) {
        auto& attack = eAttack.get<Attack>();
        auto& playerState = ePlayer.get<PlayerState>();
        auto& health = ePlayer.get<Health>();
        auto& character = ePlayer.get<Character>();

        // Blocking and evading attacks
        if (playerState.state == State::CROUCH_BLOCK || playerState.state == State::GETUP
            || playerState.isLaying || (playerState.state == State::BLOCK
                && attack.type != State::LOW_SWEEP_KICK && attack.type != State::CROUCH_KICK))
        {
            health.health -= 1;
            --(playerState.currFrame);
            return;
        }

        bool isJumping = playerState.isJumping;
        bool isCrouching = playerState.isCrouching;
        switch (attack.type)
        {
            case State::LOW_PUNCH:
                health.health -= 5;
                playerState.reset();
                if (isCrouching)
                {
                    playerState.state = State::CROUCH_HIT;
                }
                else if (isJumping)
                {
                    playerState.state = State::FALL;
                    playerState.busyFrames = character.sprite[playerState.state].frameCount;
                    playerState.freezeFrame = playerState.busyFrames - 1;
                    playerState.freezeFrameDuration = 2;
                    playerState.isLaying = true;
                    playerState.isJumping = true;
                }
                else
                {
                    playerState.state = State::TORSO_HIT;
                }
                playerState.busyFrames = character.sprite[playerState.state].frameCount;
                playerState.busy = true;
                break;
            case State::HIGH_PUNCH:
                health.health -= 5;
                playerState.reset();
                if (isCrouching)
                {
                    playerState.state = State::CROUCH_HIT;
                }
                else if (isJumping)
                {
                    playerState.state = State::FALL;
                    playerState.busyFrames = character.sprite[playerState.state].frameCount;
                    playerState.freezeFrame = playerState.busyFrames - 1;
                    playerState.freezeFrameDuration = 2;
                    playerState.isLaying = true;
                    playerState.isJumping = true;
                }
                else
                {
                    playerState.state = State::HEAD_HIT ;
                }
                playerState.busyFrames = character.sprite[playerState.state].frameCount;
                playerState.busy = true;
                break;
            case State::LOW_KICK:
                health.health -= 8;
                playerState.reset();
                if (isJumping)
                {
                    playerState.state = State::FALL;
                    playerState.busyFrames = character.sprite[playerState.state].frameCount;
                    playerState.freezeFrame = playerState.busyFrames - 1;
                    playerState.freezeFrameDuration = 2;
                    playerState.isLaying = true;
                    playerState.isJumping = true;
                }
                else
                {
                    playerState.state = State::KICKBACK_TORSO_HIT ;
                }
                playerState.busyFrames = character.sprite[playerState.state].frameCount;
                playerState.busy = true;
                break;
            case State::HIGH_KICK:
            case State::JUMP_HIGH_KICK:
            case State::JUMP_PUNCH:
            case State::JUMP_LOW_KICK:
                health.health -= 8;
                playerState.reset();
                if (isJumping)
                {
                    playerState.state = State::FALL;
                    playerState.busyFrames = character.sprite[playerState.state].frameCount;
                    playerState.freezeFrame = playerState.busyFrames - 1;
                    playerState.freezeFrameDuration = 2;
                    playerState.isLaying = true;
                    playerState.isJumping = true;
                }
                else
                {
                    playerState.state = State::HEAD_HIT ;
                }
                playerState.busyFrames = character.sprite[playerState.state].frameCount;
                playerState.busy = true;
                break;
            case State::LOW_SWEEP_KICK:
                health.health -= 12;
                playerState.reset();
                if (isJumping)
                {
                    playerState.state = State::FALL;
                    playerState.isJumping = true;
                }
                else
                {
                    playerState.state = State::FALL_INPLACE ;
                }
                playerState.busyFrames = character.sprite[playerState.state].frameCount;
                playerState.freezeFrame = playerState.busyFrames - 1;
                playerState.freezeFrameDuration = 2;
                playerState.isLaying = true;
                playerState.busy = true;
                break;
            case State::HIGH_SWEEP_KICK:
                health.health -= 14;
                playerState.reset();
                if (isJumping)
                {
                    playerState.isJumping = true;
                }
                playerState.state = State::FALL;
                playerState.busyFrames = character.sprite[playerState.state].frameCount;
                playerState.freezeFrame = playerState.busyFrames - 1;
                playerState.freezeFrameDuration = 2;
                playerState.isLaying = true;
                playerState.busy = true;
                break;
            case State::UPPERCUT:
                health.health -= 14;
                playerState.reset();
                if (isJumping)
                {
                    playerState.state = State::FALL;
                    playerState.isJumping = true;
                }
                else
                {
                    playerState.state = State::UPPERCUT_HIT ;
                }
                playerState.busyFrames = character.sprite[playerState.state].frameCount;
                playerState.freezeFrame = playerState.busyFrames - 1;
                playerState.freezeFrameDuration = 2;
                playerState.isLaying = true;
                playerState.busy = true;
                break;
            case State::CROUCH_KICK:
                health.health -= 7;
                playerState.reset();
                if (isCrouching)
                {
                    playerState.state = State::CROUCH_HIT;
                }
                else if (isJumping)
                {
                    playerState.state = State::FALL;
                    playerState.busyFrames = character.sprite[playerState.state].frameCount;
                    playerState.freezeFrame = playerState.busyFrames - 1;
                    playerState.freezeFrameDuration = 2;
                    playerState.isLaying = true;
                    playerState.isJumping = true;
                }
                else
                {
                    playerState.state = State::TORSO_HIT ;

                }
                playerState.busyFrames = character.sprite[playerState.state].frameCount;
                playerState.busy = true;
                break;
            case State::SPECIAL_1:
            case State::SPECIAL_2:
            case State::SPECIAL_3:
                health.health -= 10;
                playerState.reset();
                if (isCrouching)
                {
                    playerState.state = State::CROUCH_HIT;
                }
                else if (isJumping)
                {
                    playerState.state = State::FALL;
                    playerState.busyFrames = character.sprite[playerState.state].frameCount;
                    playerState.freezeFrame = playerState.busyFrames - 1;
                    playerState.freezeFrameDuration = 2;
                    playerState.isLaying = true;
                    playerState.isJumping = true;
                }
                else
                {
                    playerState.state = State::TORSO_HIT ;

                }
                playerState.busyFrames = character.sprite[playerState.state].frameCount;
                playerState.busy = true;
                if (eAttack.has<SpecialAttack>())
                {
                    eAttack.get<SpecialAttack>().explode = true;
                }
                break;
            default:
                break;
        }
    }

    void MK::AttackDecaySystem() {
        static const bagel::Mask mask = bagel::MaskBuilder()
            .set<Collider>()
            .set<Attack>()
            .set<Time>()
            .build();

        for (bagel::ent_type e = {0}; e.id <= bagel::World::maxId().id; ++e.id) {
            if (bagel::Entity entity{e}; entity.test(mask)) {
                auto& collider = entity.get<Collider>();
                auto& time = entity.get<Time>();

                if (time.time <= 0) {
                    if (b2Body_IsValid(collider.body)) {
                        const auto* e_p = static_cast<bagel::ent_type*>(b2Body_GetUserData(collider.body));
                        b2DestroyBody(collider.body);
                        delete e_p;
                    }
                    collider.body = b2_nullBodyId;
                    bagel::World::destroyEntity(e);
                }
            }
        }
    }

    void MK::SpecialAttackSystem() {
        static const bagel::Mask mask = bagel::MaskBuilder()
            .set<SpecialAttack>()
            .set<Character>()
            .build();

        for (bagel::ent_type e = {0}; e.id <= bagel::World::maxId().id; ++e.id) {
            if (bagel::Entity entity{e}; entity.test(mask))
            {
                if (entity.get<SpecialAttack>().explode)
                {
                    auto& spritePrev = entity.get<Character>().specialAttackSprite[entity.get<SpecialAttack>().type];
                    auto& spriteNext = entity.get<Character>().specialAttackSprite[SpecialAttacks::EXPLOSION];
                    entity.get<Movement>().reset();
                    entity.get<Position>().y -= ((spriteNext.h - spritePrev.h) / 2.0f) * SCALE_CHARACTER;
                    if (entity.get<SpecialAttack>().direction == LEFT)
                        entity.get<Position>().x += 0;
                    if (entity.get<SpecialAttack>().direction == RIGHT)
                        entity.get<Position>().x += ((spriteNext.w) / 2.0f) * SCALE_CHARACTER;
                    entity.get<SpecialAttack>().type = SpecialAttacks::EXPLOSION;
                    entity.get<SpecialAttack>().frame = 0;
                    entity.get<SpecialAttack>().totalFrames = spriteNext.frameCount - 1;
                    entity.get<SpecialAttack>().explode = false;
                    entity.get<Time>().time = 4;
                }
            }
        }
    }

    void MK::HealthBarSystem() {
        static const bagel::Mask mask = bagel::MaskBuilder()
            .set<HealthBarReference>()
            .set<DamageVisual>()
            .set<Texture>()
            .set<Position>()
            .build();

        for (bagel::ent_type e = {0}; e.id <= bagel::World::maxId().id; ++e.id) {
            if (bagel::Entity entity{e}; entity.test(mask)) {

                // Use HealthBarReference to access actual player health
                auto& reference = entity.get<HealthBarReference>();
                if (reference.target.id == -1) continue;

                bagel::Entity player = bagel::Entity{reference.target};
                auto& health = player.get<Health>();

                // Continue as usual
                auto& damage = entity.get<DamageVisual>();
                auto& texture = entity.get<Texture>();

                float ratio = std::max(0.0f, health.health / health.max_health);

                // Smooth trailing damage effect
                if (damage.trailingHealth > health.health) {
                    constexpr float TRAIL_SPEED = 0.5f;
                    damage.trailingHealth -= TRAIL_SPEED;
                    if (damage.trailingHealth < health.health)
                        damage.trailingHealth = health.health;
                }

                texture.rect.w = 250.0f * ratio;
            }
        }
    }

    SDL_Texture* MK::TextureSystem::getTexture(SDL_Renderer* renderer, const std::string& filePath, IgnoreColorKey ignoreColorKey)
    {
        // Check if the texture is already cached
        std::string cacheKey = filePath + "_" + std::to_string(static_cast<int>(ignoreColorKey));

        if (textureCache.find(cacheKey) != textureCache.end()) {
            return textureCache[cacheKey];
        }

        // Load the texture if not cached
        SDL_Surface* surface = IMG_Load(filePath.c_str());
        if (!surface) {
            SDL_Log("Failed to load image: %s, SDL_Error: %s", filePath.c_str(), SDL_GetError());
            return nullptr;
        }

        const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(surface->format);
        switch (ignoreColorKey)
        {
            case IgnoreColorKey::CHARACTER:
                SDL_SetSurfaceColorKey(surface, true, SDL_MapRGB(fmt, nullptr,
                                                      CHARACTER_COLOR_IGNORE_RED,
                                                      CHARACTER_COLOR_IGNORE_GREEN,
                                                      CHARACTER_COLOR_IGNORE_BLUE));
                break;
            case IgnoreColorKey::BACKGROUND:
                SDL_SetSurfaceColorKey(surface, true, SDL_MapRGB(fmt, nullptr,
                                                      BACKGROUND_COLOR_IGNORE_RED,
                                                      BACKGROUND_COLOR_IGNORE_GREEN,
                                                      BACKGROUND_COLOR_IGNORE_BLUE));
                break;
        case IgnoreColorKey::NAME_BAR:
                SDL_SetSurfaceColorKey(surface, true, SDL_MapRGB(fmt, nullptr,
                                                      COLOR_KEY_NAME_BAR_RED,
                                                      COLOR_KEY_NAME_BAR_GREEN,
                                                      COLOR_KEY_NAME_BAR_BLUE));
                break;
        case IgnoreColorKey::DAMAGE_BAR:
                SDL_SetSurfaceColorKey(surface, true, SDL_MapRGB(fmt, nullptr,
                                                      COLOR_KEY_DAMAGE_BAR_RED,
                                                      COLOR_KEY_DAMAGE_BAR_GREEN,
                                                      COLOR_KEY_DAMAGE_BAR_BLUE));
                break;
        case IgnoreColorKey::WIN_TEXT:
                SDL_SetSurfaceColorKey(surface, true, SDL_MapRGB(fmt, nullptr,
                                                      WIN_TEXT_COLOR_IGNORE_RED,
                                                      WIN_TEXT_COLOR_IGNORE_GREEN,
                                                      WIN_TEXT_COLOR_IGNORE_BLUE));
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (!texture) {
            SDL_Log("Failed to create texture: %s, SDL_Error: %s", filePath.c_str(), SDL_GetError());
            return nullptr;
        }

        // Cache the texture
        textureCache[cacheKey] = texture;
        return texture;
    }

    // ------------------------------- Entities -------------------------------

    bagel::ent_type MK::createPlayer(float x, float y, Character character, int playerNumber) const
    {
        // Construct the texture path
        std::string texturePath = "res/" + std::string(character.name) + ".png";
        auto texture = TextureSystem::getTexture(ren, texturePath, TextureSystem::IgnoreColorKey::CHARACTER);

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_kinematicBody;
        bodyDef.position= getPosition(x, y);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.enableSensorEvents = true;
        shapeDef.isSensor = true;

        b2Polygon boxShape = b2MakeBox((CHARACTER_WIDTH / 2.0f) / WINDOW_SCALE,
                                        (CHARACTER_HEIGHT / 2.0f) / WINDOW_SCALE);

        b2BodyId body = b2CreateBody(boxWorld, &bodyDef);
        b2ShapeId shape = b2CreatePolygonShape(body, &shapeDef, &boxShape);

        // Add components to the entity
        PlayerState playerState;
        playerState.playerNumber = playerNumber;
        playerState.direction = (playerNumber == 1) ? RIGHT : LEFT;

        bagel::Entity entity = bagel::Entity::create();
        entity.addAll(Position{x, y},
                      Movement{0, 0},
                      Collider{body, shape},
                      Texture{texture},
                      playerState,
                      Inputs{},
                      character,
                      Health{100, 100});

        b2Body_SetUserData(body, new bagel::ent_type{entity.entity()});
        return entity.entity();
    }

        void MK::createAttack(float x, float y, State type, int playerNumber, bool direction) const
        {

            float width = 0.0f;
            float height = 0.0f;
            float xOffset = 0.0f;
            float yOffset = 0.0f;

            switch (type)
            {
                case State::FORWARD_JUMP_PUNCH:
                case State::JUMP_PUNCH:
                case State::JUMP_LOW_KICK:
                case State::JUMP_HIGH_KICK:
                case State::LOW_PUNCH:
                case State::HIGH_PUNCH:
                    width = 70.0f;
                    height = 40.0f;
                    xOffset = width / 2.0f * (direction == LEFT ? -1.0f : 1.0f);
                    yOffset = 40.0f;
                    break;
                case State::LOW_KICK:
                case State::HIGH_KICK:
                    width = 95.0f;
                    height = 40.0f;
                    xOffset = width / 2.0f * (direction == LEFT ? -1.0f : 1.0f);
                    yOffset = 0.0f;
                    break;
                case State::HIGH_SWEEP_KICK:
                    width = 95.0f;
                    height = 40.0f;
                    xOffset = width / 2.0f * (direction == LEFT ? -1.0f : 1.0f);
                    yOffset = 40.0f;
                    break;
                case State::CROUCH_KICK:
                case State::LOW_SWEEP_KICK:
                    width = 85.0f;
                    height = 40.0f;
                    xOffset = width / 2.0f * (direction == LEFT ? -1.0f : 1.0f);
                    yOffset = -40.0f;
                    break;
                case State::UPPERCUT:
                    width = 50.0f;
                    height = 40.0f;
                    xOffset = width / 2.0f * (direction == LEFT ? -1.0f : 1.0f);
                    yOffset = 40.0f;
                    break;
                default: // Type is not a valid attack
                    return;
            }

            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_kinematicBody;
            bodyDef.position= getPosition(x + xOffset, y + yOffset);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.enableSensorEvents = true;
            shapeDef.isSensor = true;

            b2Polygon boxShape = b2MakeBox((width / 2.0f) / WINDOW_SCALE,
                                           (height / 2.0f) / WINDOW_SCALE);

            b2BodyId body = b2CreateBody(boxWorld, &bodyDef);
            b2ShapeId shape = b2CreatePolygonShape(body, &shapeDef, &boxShape);

            bagel::Entity entity = bagel::Entity::create();
            entity.addAll(Position{x, y},
                          Collider{body, shape},
                          Attack{type, playerNumber},
                          Time{Attack::ATTACK_LIFE_TIME});

            b2Body_SetUserData(body, new bagel::ent_type{entity.entity()});
        }


        void MK::createSpecialAttack(float x, float y, SpecialAttacks type, int playerNumber,
                                    bool direction, Character& character) const
        {
            // Construct the texture path
            std::string texturePath = "res/" + std::string(character.name) + ".png";
            auto texture = TextureSystem::getTexture(ren, texturePath, TextureSystem::IgnoreColorKey::CHARACTER);

            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_kinematicBody;
            bodyDef.position= getPosition(x, y + CHARACTER_HEIGHT / 2.0f);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.enableSensorEvents = true;
            shapeDef.isSensor = true;

            b2Polygon boxShape = b2MakeBox((character.specialAttackSprite[type].h / 2.0f) / WINDOW_SCALE,
                                         (character.specialAttackSprite[type].w / 2.0f) / WINDOW_SCALE);

            b2BodyId body = b2CreateBody(boxWorld, &bodyDef);
            b2ShapeId shape = b2CreatePolygonShape(body, &shapeDef, &boxShape);

            State state;
            switch (type)
            {
                case SpecialAttacks::FIREBALL:
                    state = State::SPECIAL_1;
                    break;
                    // add more special attacks here
                default:
                    return; // Invalid special attack type
            }

            // Add components to the entity
            bagel::Entity entity = bagel::Entity::create();
            entity.addAll(Position{x + ((CHAR_SQUARE_WIDTH / 2.0f) * SCALE_CHARACTER),
                                    y + ((character.specialAttackOffset_y - (character.specialAttackSprite[type].h / 2.0f)) * SCALE_CHARACTER)},
                       Movement{(direction == LEFT) ? -15.0f : 15.0f, 0},
                       Collider{body, shape},
                       Texture{texture},
                       Attack{state, playerNumber},
                       SpecialAttack{type, direction},
                       character,
                       Time{SpecialAttack::SPECIAL_ATTACK_LIFE_TIME});

            b2Body_SetUserData(body, new bagel::ent_type{entity.entity()});
        }

        void MK::createBoundary(bool side) const
        {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_staticBody;
            if (side == LEFT)
            {
                bodyDef.position= getPosition(BOUNDARY_WIDTH / -1.2f, WINDOW_HEIGHT / 2.0f);
            }
            else
            {
                bodyDef.position= getPosition(WINDOW_WIDTH + (BOUNDARY_WIDTH / 6.5f), WINDOW_HEIGHT / 2.0f);
            }

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.enableSensorEvents = true;
            shapeDef.isSensor = true;

            b2Polygon boxShape = b2MakeBox(BOUNDARY_WIDTH / 2.0f / WINDOW_SCALE,
                                            WINDOW_HEIGHT / 2.0f / WINDOW_SCALE);

            b2BodyId body = b2CreateBody(boxWorld, &bodyDef);
            b2ShapeId shape = b2CreatePolygonShape(body, &shapeDef, &boxShape);

            bagel::Entity entity = bagel::Entity::create();

            entity.addAll(Collider{body, shape},
                          Boundary{side});

            b2Body_SetUserData(body, new bagel::ent_type{entity.entity()});
        }

        void MK::createBackground(const std::string& backgroundPath) const
    {

        auto texture = TextureSystem::getTexture(ren, backgroundPath, TextureSystem::IgnoreColorKey::BACKGROUND);

        // Create fence
        bagel::Entity fence = bagel::Entity::create();
        fence.addAll(
            Position{0, 0},
            Texture{
                texture,
                { fenceX, fenceY, fenceW, fenceH }, // Only show the red/black part
                { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 1.3f} // Stretch or place as needed
            }
        );

        // Create temple
        bagel::Entity temple = bagel::Entity::create();
        temple.addAll(
            Position{0, 0},
            Texture{
                texture,
                { templeX, templeY, templeW, templeH }, // Only show the red/black part
                { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT } // Stretch to fit window
            }
        );
    }

    void MK::createBar(bagel::Entity player1, bagel::Entity player2) const
    {

        // Dimensions of the green health bar in the texture
        SDL_FRect GREEN_BAR_SRC = { 5406, 49, 163, 12 };  // Green (top bar)
        SDL_FRect RED_BAR_SRC   = { 5406, 63, 163, 12 }; // Red  (bottom bar)

        // Bar dimensions
        constexpr float BAR_WIDTH = 250.0f;
        constexpr float BAR_HEIGHT = 18.0f;

        // Positioning constants
        constexpr float OFFSET_Y = 10.0f;
        constexpr float MARGIN = 50.0f;
        constexpr float xRight = WINDOW_WIDTH - BAR_WIDTH - MARGIN;

        // Load the image as a surface
        auto barTexture = TextureSystem::getTexture(ren, "res/Menus&Text.png", TextureSystem::IgnoreColorKey::DAMAGE_BAR);
        auto nameTexture = TextureSystem::getTexture(ren, "res/Menus&Text.png", TextureSystem::IgnoreColorKey::NAME_BAR);

        // Player 1 - RED background bar
        bagel::Entity red1 = bagel::Entity::create();
        red1.addAll(
            Position{ MARGIN, OFFSET_Y },
            Texture{
                barTexture,
                RED_BAR_SRC,
                SDL_FRect{ MARGIN, OFFSET_Y, BAR_WIDTH, BAR_HEIGHT }
            }
        );

        // Player 1 - GREEN health bar
        bagel::Entity green1 = bagel::Entity::create();
        green1.addAll(
            Position{ MARGIN, OFFSET_Y },
            Texture{
                barTexture,
                GREEN_BAR_SRC,
                SDL_FRect{ MARGIN, OFFSET_Y, BAR_WIDTH, BAR_HEIGHT }
            },
            DamageVisual{100.0f},
            HealthBarReference{ player1.entity() }
        );

        // Player 1 - Player's name
        bagel::Entity name1 = bagel::Entity::create();
        name1.addAll(
            Position{ MARGIN, OFFSET_Y },
            Texture{
                nameTexture,
                player1.get<Character>().leftBarNameSource,
                SDL_FRect{ MARGIN, OFFSET_Y, BAR_WIDTH, BAR_HEIGHT }
            }
        );

        // Player 2 - RED background bar
        bagel::Entity red2 = bagel::Entity::create();
        red2.addAll(
            Position{ xRight, OFFSET_Y },
            Texture{
                barTexture,
                RED_BAR_SRC,
                SDL_FRect{ xRight, OFFSET_Y, BAR_WIDTH, BAR_HEIGHT }
            }
        );

        // Player 2 - GREEN health bar
        bagel::Entity green2 = bagel::Entity::create();
        green2.addAll(
            Position{ xRight, OFFSET_Y },
            Texture{
                barTexture,
                GREEN_BAR_SRC,
                SDL_FRect{ xRight, OFFSET_Y, BAR_WIDTH, BAR_HEIGHT }
            },
            DamageVisual{100.0f},
            HealthBarReference{ player2.entity() }
        );

        // Player 2 - Player's name
        bagel::Entity name2 = bagel::Entity::create();
        name2.addAll(
            Position{ xRight, OFFSET_Y },
            Texture{
                nameTexture,
                player2.get<Character>().rightBarNameSource,
                SDL_FRect{ xRight, OFFSET_Y, BAR_WIDTH, BAR_HEIGHT }
            }
        );
    }

    void MK::createWinText(const Character& winCharacter) const
    {
        // Load the image as a surface
        const auto texture = TextureSystem::getTexture(ren, "res/Menus&Text.png", TextureSystem::IgnoreColorKey::WIN_TEXT);

        // Create win text entity
        bagel::Entity winText = bagel::Entity::create();
        winText.addAll(
            Position{(WINDOW_WIDTH / 2.0f) - (getWinSpriteFrame(winCharacter, 0).w / 1.3f), WINDOW_HEIGHT / 3.0f},
            winCharacter,
            Texture{texture},
            Time{100000},
            WinMessage{}
        );
    }
}