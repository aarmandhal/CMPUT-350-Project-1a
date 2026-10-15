#ifndef GALAGA_H
#define GALAGA_H

#include <memory>
#include <string>
#include "GameObject.h"

class Player;

class Galaga : public CMPUT350::GameObject
{
public:
    Galaga(int width, int height);

    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void RenderUI(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& message) override;

private:
    // the five required game states
    enum class State {
        Waiting,
        Startup,
        Waves,
        Gameplay,
        GameOver
    };

    // store the current game state
    State mState = State::Waiting;

    // store the window dimensions
    int mWidth;
    int mHeight;

    // track game progress
    int mScore = 0;
    int mLives = 3;
    int mLevel = 1;
    int mCoins = 0;
    int mEnemiesRemaining = 0;

    // track when the current state started
    int mStateStartFrame = 0;

    // track the player without keeping it alive
    std::weak_ptr<Player> mPlayer;
};

#endif