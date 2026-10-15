#include "Galaga.h"
#include "GameContext.h"
#include "EngineView.h"
#include "NotificationManager.h"
#include "../engine/Keyboard.h"
#include "Player.h"
#include "Stars.h"
#include "DrawContext.h"

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

        // move to the startup state
        mState = State::Startup;

        // reset the timer for the startup state
        mStateStartFrame = -1;
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
        if (mPlayer.expired()) {
            auto player = std::make_shared<Player>(
                CMPUT350::Point2D(mWidth / 2, mHeight - 124)
            );

            mPlayer = player;
            context->mEngineView->AddGameObject(player);
        }

        // wait about three seconds before starting waves
        if (context->currentFrame - mStateStartFrame >= 90) {
            mEnemiesRemaining = 0;
            mState = State::Waves;
            mStateStartFrame = context->currentFrame;
        }
    }

    // handle player lives during gameplay
    else if (mState == State::Gameplay) {
        // check whether the player has been destroyed
        if (mPlayer.expired()) {
            // lose one life
            mLives--;

            // enter game over when no lives remain
            if (mLives <= 0) {
                mState = State::GameOver;
                mStateStartFrame = context->currentFrame;
            }
            else {
                // create another player
                auto player = std::make_shared<Player>(
                    CMPUT350::Point2D(mWidth / 2, mHeight - 124)
                );

                mPlayer = player;
                context->mEngineView->AddGameObject(player);
            }
        }
    }

    // show game over before returning to the waiting screen
    else if (mState == State::GameOver) {
        // wait three seconds
        if (context->currentFrame - mStateStartFrame >= 90) {
            mState = State::Waiting;
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