#ifndef PLAYER_H
#define PLAYER_H

#include "CollisionObject.h"
#include <string>

// forward decleration of the class Bullet
class Bullet;

class Player : public CMPUT350::CollisionObject
{
public:
    Player(CMPUT350::Point2D loc);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& message) override;    
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    // return the players position and rotation
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;

private:
    // store the players current location
    CMPUT350::Point2D mLocation;

    // weak pointer because Player only wants to 
    // keep track of whether its bullets still exist
    std::weak_ptr<Bullet> mBullet1;
    std::weak_ptr<Bullet> mBullet2;

    // keep access to the game context for shooting
    CMPUT350::GameContext* mContext = nullptr;

    // keep a weak reference to this player for bullets
    std::weak_ptr<CMPUT350::GameObject> mSelf;

    // track which movement keys are held
    bool mMovingLeft = false;
    bool mMovingRight = false;
    bool mFiring = false;

    // track whether the player is alive
    bool mAlive = true;

};

#endif
