#include <cassert>
#include "Player.h"
#include "Bullet.h"
#include "GameContext.h"
#include "EngineView.h"
#include "NotificationManager.h"

Player::Player(CMPUT350::Point2D loc)
{
    mLocation = loc;
}

void Player::Initialize(CMPUT350::GameContext* context)
{
    // save the context for creating bullets
    mContext = context;

    // get access to the notification manager
    auto manager = context->mNotificationManager;

    // register this player to receive the messages
    manager->Register(context->CurrObject, "left_down");
    manager->Register(context->CurrObject, "left_up");
    manager->Register(context->CurrObject, "right_down");
    manager->Register(context->CurrObject, "right_up");
    manager->Register(context->CurrObject, "fire_down");
    manager->Register(context->CurrObject, "fire_up");

}

void Player::Update(CMPUT350::GameContext* context)
{
    const float moveAmount = 10;
    const float halfSize = 20;

    int screenWidth = context->ScreenContext->GetWindowWidth();

    // move left while a is held
    if (mMovingLeft) {
        mLocation.x -= moveAmount;
    }

    // move right while d is held
    if (mMovingRight) {
        mLocation.x += moveAmount;
    }

    // keep the player inside the left side of the screen
    if (mLocation.x < halfSize) {
        mLocation.x = halfSize;
    }

    // keep the player inside the right side of the screen
    if (mLocation.x > screenWidth - halfSize) {
        mLocation.x = screenWidth - halfSize;
    }

}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}


void Player::ReceiveNotification(const std::string& message)
{
    // start moving left when a is pressed
    if (message == "left_down") {
        mMovingLeft = true;
    }

    // stop moving left when a is released
    else if (message == "left_up") {
        mMovingLeft = false;
    }

    // start moving right when d is pressed
    else if (message == "right_down") {
        mMovingRight = true;
    }

    // stop moving right when d is released
    else if (message == "right_up") {
        mMovingRight = false;
    }

    // allow shooting again when space is released
    else if (message == "fire_up") {
        mFiring = false;
    }

    // shoot once when space is pressed
    else if (message == "fire_down" && !mFiring) {
        mFiring = true;

        // bullets move straight upward
        CMPUT350::Point2D heading(0, -10);

        // create a bullet if the first slot is free
        if (mBullet1.expired()) {
            auto bullet = std::make_shared<Bullet>(
                mLocation, heading, true);

            mBullet1 = bullet;
            mContext->mEngineView->AddGameObject(bullet);
        }

        // otherwise use the second slot
        else if (mBullet2.expired()) {
            auto bullet = std::make_shared<Bullet>(
                mLocation, heading, true);

            mBullet2 = bullet;
            mContext->mEngineView->AddGameObject(bullet);
        }
    }
}

// return the players current location
CMPUT350::Point2D Player::GetLocation() const
{
    return mLocation;
}

// return the players rotation
float Player::GetRotation() const
{
    return 0.0f;
}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    const float halfSize = 20.0f;

    // triangle points relative to the players center
    CMPUT350::Point2D top(0, -halfSize);
    CMPUT350::Point2D bottomLeft(-halfSize, halfSize);
    CMPUT350::Point2D bottomRight(halfSize, halfSize);

    // draw the player triangle
    context->ScreenContext->DrawLine(top, bottomLeft, 3, CMPUT350::Colors::green);
    context->ScreenContext->DrawLine(bottomLeft, bottomRight, 3, CMPUT350::Colors::green);
    context->ScreenContext->DrawLine(bottomRight, top, 3, CMPUT350::Colors::green);

}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
}

void Player::Kill()
{
    // mark the player as dead
    mAlive = false;
}

bool Player::IsAlive() const
{
    // return whether player is alive or not
    return mAlive;
}

const CMPUT350::Rect& Player::GetBounds()
{
    const float halfSize = 20.0f;

    // static so the returned reference stays valid
    static CMPUT350::Rect playerBound;

    // create a 40x40 collision box around the player
    playerBound = CMPUT350::Rect(
        // get the top left corner of the players invisible box, width, height
        CMPUT350::Point2D(mLocation.x - halfSize, mLocation.y - halfSize), 40, 40);

    return playerBound;
}
