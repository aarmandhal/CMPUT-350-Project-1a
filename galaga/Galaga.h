#ifndef GALAGA_H
#define GALAGA_H

#include <memory>
#include <string>
#include "GameObject.h"
#include <vector>

class Player;
class Enemy;

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

    // track the current wave and enemies spawned in it
    int mCurrentWave = 0;
    int mEnemiesSpawned = 0;

    // track when the current state started
    int mStateStartFrame = 0;

    // track when the player was destroyed
    int mPlayerDeathFrame = -1;

    // track when the last enemy attack started
    int mLastAttackFrame = -1;
    

    // track the player without keeping it alive
    std::weak_ptr<Player> mPlayer;

    // track enemies without keeping them alive
    std::vector<std::weak_ptr<Enemy>> mEnemies;
};

#endif