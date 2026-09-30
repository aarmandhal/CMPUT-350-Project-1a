#include <cassert>
#include "Player.h"
#include "Bullet.h"

Player::Player(CMPUT350::Point2D loc)
{
    mLocation = loc;
}

void Player::Initialize(CMPUT350::GameContext* context)
{
}

void Player::Update(CMPUT350::GameContext* context)
{
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    const float moveAmount = 20;
    const float halfSize = 20;

    int screenWidth = context->ScreenContext->GetWindowWidth();

    if (key == 'a' || key == 'A')
    {
        mLocation.x -= moveAmount; // move left

        // for player to stay on screen
        if (mLocation.x < halfSize)
            mLocation.x = halfSize;

        return true;
    }

    else if (key == 'd' || key == 'D') {
        mLocation.x += moveAmount; // move right

        // for player to stay on screen
        if (mLocation.x > screenWidth - halfSize) {
            mLocation.x = screenWidth - halfSize;
        };

        return true;
    }

    // Note: a weak_ptr is not destroyed when its Bullet is destroyed.
    // it just becomes expired which means that bullet slot is free to be used again
    else if (key == ' ') {
        // bullets movement direction. x movement is 0, y is -10
        CMPUT350::Point2D heading(0, -10);

        if (mBullet1.expired()) {
            // create a new bullet object
            auto bullet = std::make_shared<Bullet>(
                mLocation, // stat the bullet where the player is
                heading, // move straigh up (y=-10)
                true);

            mBullet1 = bullet;
            context->mEngineView->AddGameObject(bullet);
        }

        else if (mBullet2.expired()) {
            auto bullet = std::make_shared<Bullet>(mLocation, heading, true);

            mBullet2 = bullet;
            context->mEngineView->AddGameObject(bullet);
        }

        return true;
    }

    return false;

}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    const float halfSize = 20.0f;

    // same x as the center but 20 pixels upward
    CMPUT350::Point2D top(mLocation.x, mLocation.y - halfSize);

    // 20 pixels left and 20 pixels down
    CMPUT350::Point2D bottomLeft(mLocation.x - halfSize, mLocation.y + halfSize);

    // 20 pixels right and 20 pixels down
    CMPUT350::Point2D bottomRight(mLocation.x + halfSize, mLocation.y + halfSize);

    // connect the three points with lines
    context->ScreenContext->DrawLine(top, bottomLeft, 3, CMPUT350::Colors::green);
    context->ScreenContext->DrawLine(bottomLeft, bottomRight, 3, CMPUT350::Colors::green);
    context->ScreenContext->DrawLine(bottomRight, top, 3, CMPUT350::Colors::green);

}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
}

void Player::Kill()
{
}

bool Player::IsAlive() const
{
    return true;
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
