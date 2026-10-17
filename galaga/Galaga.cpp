#include "Galaga.h"
#include "GameContext.h"
#include "EngineView.h"
#include "NotificationManager.h"
#include "../engine/Keyboard.h"
#include "Player.h"
#include "Stars.h"
#include "DrawContext.h"
#include "Enemy.h"
#include "WeakEnemy.h"
#include "MidEnemy.h"
#include "StrongEnemy.h"

// save the game window dimensions
Galaga::Galaga(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

void Galaga::Initialize(CMPUT350::GameContext* context)
{
    // get access to the engine and notification manager
    auto engine = context->mEngineView;
    auto manager = context->mNotificationManager;

    // register player movement notifications
    engine->InstallKeyDownNotification(CMPUT350::Key::A, "left_down");
    engine->InstallKeyUpNotification(CMPUT350::Key::A, "left_up");

    engine->InstallKeyDownNotification(CMPUT350::Key::D, "right_down");
    engine->InstallKeyUpNotification(CMPUT350::Key::D, "right_up");

    // register player shooting notifications
    engine->InstallKeyDownNotification(CMPUT350::Key::Space, "fire_down");
    engine->InstallKeyUpNotification(CMPUT350::Key::Space, "fire_up");

    // register coin and start notifications
    engine->InstallKeyDownNotification(CMPUT350::Key::Num5, "coin");
    engine->InstallKeyDownNotification(CMPUT350::Key::Num1, "start");

    // allow galaga to receive coin and start notifications
    manager->Register(context->CurrObject, "coin");
    manager->Register(context->CurrObject, "start");

    // listen for enemies being destroyed
    manager->Register(context->CurrObject, "enemy_destroyed");

    // create the star background
    engine->AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, mWidth, mHeight))
    );
}


void Galaga::ReceiveNotification(const std::string& message)
{
    // insert a coin while waiting for the game to begin
    if (message == "coin" && mState == State::Waiting) {
        mCoins++;
    }

    // start the game when at least one coin is available
    else if (message == "start" && mState == State::Waiting && mCoins > 0) {
        mCoins--;

        // reset the game for a new session
        mScore = 0;
        mLives = 3;
        mLevel = 1;
        mEnemiesRemaining = 0;
        mCurrentWave = 0;
        mEnemiesSpawned = 0;
        mPlayerDeathFrame = -1;
        mLastAttackFrame = -1;
        mNextAttackIndex = 0;

        // move to the startup state
        mState = State::Startup;

        // reset the timer for the startup state
        mStateStartFrame = -1;
    }

    // update the score and enemy count when an enemy is destroyed
    else if (message == "enemy_destroyed" && (mState == State::Waves || mState == State::Gameplay)) {

        for (size_t i = 0; i < mEnemies.size(); ) {
                auto enemy = mEnemies[i].lock();

                // check if the enemy was destroyed
                if (enemy && !enemy->IsAlive()) {
                    mScore += enemy->GetPointValue();

                    if (mEnemiesRemaining > 0) {
                        mEnemiesRemaining--;
                    }

                    // remove the destroyed enemy
                    mEnemies.erase(mEnemies.begin() + i);
                }
                else if (!enemy) {
                    // remove expired enemy references
                    mEnemies.erase(mEnemies.begin() + i);
                }
                else {
                    // move to the next enemy
                    i++;
                }
            }
        }
    }


void Galaga::Update(CMPUT350::GameContext* context)
{
    // wait for a coin and the start key
    if (mState == State::Waiting) {
        return;
    }

    // set up a new level
    else if (mState == State::Startup) {
        // start timing when startup begins
        if (mStateStartFrame == -1) {
            mStateStartFrame = context->currentFrame;
        }

        // create the player if one does not exist
        auto player = mPlayer.lock();

        if (!player || !player->IsAlive()) {
            auto newPlayer = std::make_shared<Player>(
                CMPUT350::Point2D(mWidth / 2, mHeight - 124)
            );

            mPlayer = newPlayer;
            context->mEngineView->AddGameObject(newPlayer);
        }

        // wait about three seconds before starting waves
        if (context->currentFrame - mStateStartFrame >= 90) {
            mEnemiesRemaining = 0;
            mState = State::Waves;
            mStateStartFrame = context->currentFrame;
        }
    }

    // spawn five waves of eight enemies
    else if (mState == State::Waves) {

        // spawn one enemy every 15 frames
        if (mCurrentWave < 5 &&
            mEnemiesSpawned < 8 &&
            (context->currentFrame - mStateStartFrame) % 15 == 0) {

            float x = mWidth / 2.0f - 70 + mEnemiesSpawned * 20;
            float y = 100 + mCurrentWave * 35;

            std::shared_ptr<Enemy> enemy;

            // choose the enemy type based on the wave
            if (mCurrentWave == 0) {
                enemy = std::make_shared<WeakEnemy>(
                    CMPUT350::Point2D(x, y)
                );
            }
            else if (mCurrentWave == 1) {
                enemy = std::make_shared<MidEnemy>(
                    CMPUT350::Point2D(x, y)
                );
            }
            else {
                enemy = std::make_shared<StrongEnemy>(
                    CMPUT350::Point2D(x, y)
                );
            }

            context->mEngineView->AddGameObject(enemy);
            mEnemies.push_back(enemy);

            mEnemiesSpawned++;
            mEnemiesRemaining++;
        }

        // move to the next wave after spawning eight enemies
        if (mEnemiesSpawned == 8) {
            mCurrentWave++;
            mEnemiesSpawned = 0;
        }

        // enter gameplay after all five waves
        if (mCurrentWave == 5) {
            mState = State::Gameplay;
            mStateStartFrame = context->currentFrame;
        }
    }

    // start enemy attacks during gameplay
    else if (mState == State::Gameplay) {

        // only start attacks while the player is alive
        auto player = mPlayer.lock();

        if (player && player->IsAlive()) {

            // start an attack every 90 frames
            if (mLastAttackFrame == -1 || context->currentFrame - mLastAttackFrame >= 90) {

                // search for the next living enemy
                for (size_t i = 0; i < mEnemies.size(); i++) {
                    size_t index = (mNextAttackIndex + i) % mEnemies.size();
                    auto enemy = mEnemies[index].lock();

                    if (enemy && enemy->IsAlive() && enemy->IsInFormation()) {
                        enemy->StartAttack();
                        mNextAttackIndex = (index + 1) % mEnemies.size();
                        mLastAttackFrame = context->currentFrame;
                        break;
                    }
                }
            }
        }
    }

    // show game over before returning to the waiting screen
    else if (mState == State::GameOver) {
        // wait three seconds
        if (context->currentFrame - mStateStartFrame >= 90) {

            // remove the old player
            if (auto player = mPlayer.lock()) {
                player->Kill();
            }
            mPlayer.reset();

            // remove any surviving enemies
            for (auto& weakEnemy : mEnemies) {
                if (auto enemy = weakEnemy.lock()) {
                    enemy->Kill();
                }
            }

            // clear the enemy list
            mEnemies.clear();
            mEnemiesRemaining = 0;

            // reset the respawn timer
            mPlayerDeathFrame = -1;

            // return to the waiting screen
            mState = State::Waiting;
            mStateStartFrame = -1;
        }
    }

    // handle player deaths during waves and gameplay
    if (mState == State::Waves || mState == State::Gameplay) {
        auto player = mPlayer.lock();

        if (!player || !player->IsAlive()) {

            // count the lost life only once
            if (mPlayerDeathFrame == -1) {
                mLives--;
                mPlayerDeathFrame = context->currentFrame;

                if (mLives <= 0) {
                    mState = State::GameOver;
                    mStateStartFrame = context->currentFrame;
                    return;
                }
            }

            // check if all surviving enemies have returned to formation
            bool allInFormation = true;

            for (auto& weakEnemy : mEnemies) {
                auto enemy = weakEnemy.lock();

                if (enemy && enemy->IsAlive() && !enemy->IsInFormation()) {
                    allInFormation = false;
                    break;
                }
            }

            // wait before respawning the player
            if (context->currentFrame - mPlayerDeathFrame >= 90 && allInFormation) {
                auto newPlayer = std::make_shared<Player>(CMPUT350::Point2D(mWidth / 2, mHeight - 124));

                mPlayer = newPlayer;
                context->mEngineView->AddGameObject(newPlayer);
                mPlayerDeathFrame = -1;
            }
        }
    }

    // start the next level when all enemies are destroyed
    if (mState == State::Gameplay && mEnemiesRemaining == 0) {

        // do not advance while the player is dead or respawning
        auto player = mPlayer.lock();

        if (player && player->IsAlive() && mPlayerDeathFrame == -1) {
            mLevel++;

            // reset the wave counters
            mCurrentWave = 0;
            mEnemiesSpawned = 0;

            // return to startup for the next level
            mState = State::Startup;
            mStateStartFrame = -1;
        }
    }

}



void Galaga::RenderUI(CMPUT350::GameContext* context)
{
    // get the drawing context for the gui
    auto gui = context->GUIContext;

    // show the score and level
    gui->DrawText("Score: " + std::to_string(mScore), 24, {20, 20}, CMPUT350::Colors::white);

    gui->DrawText("Level: " + std::to_string(mLevel), 24, {mWidth - 150.0f, 20}, CMPUT350::Colors::white);

    // show the remaining lives
    gui->DrawText("Lives: " + std::to_string(mLives), 24, {20, mHeight - 40.0f}, CMPUT350::Colors::white);

    // show instructions while waiting for the game
    if (mState == State::Waiting) {
        gui->DrawCenteredText("Credits: " + std::to_string(mCoins), 24, {mWidth / 2.0f, mHeight / 2.0f - 50}, CMPUT350::Colors::white);

        gui->DrawCenteredText("Press 5 to insert coin", 24, {mWidth / 2.0f, mHeight / 2.0f}, CMPUT350::Colors::white);

        gui->DrawCenteredText("Press 1 to start", 24, {mWidth / 2.0f, mHeight / 2.0f + 40}, CMPUT350::Colors::white);

    }

    // show the level when starting
    else if (mState == State::Startup) {
        gui->DrawCenteredText("LEVEL " + std::to_string(mLevel), 36, {mWidth / 2.0f, mHeight / 2.0f}, CMPUT350::Colors::white);
    }

    // show a message when the game ends
else if (mState == State::GameOver) {
    gui->DrawCenteredText("GAME OVER", 48, {mWidth / 2.0f, mHeight / 2.0f}, CMPUT350::Colors::red);
}  

}