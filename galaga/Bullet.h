#ifndef BULLET_H
#define BULLET_H

#include <memory>
#include <vector>

#include "CollisionObject.h"
#include "GameContext.h"
#include "GameObject.h"

class Bullet : public CMPUT350::CollisionObject {
public:
    // firer is who shot this bullet, so it can be ignored in CollisionEnter and a
    // bullet never kills its own shooter. Defaults to an empty weak_ptr for callers
    // (or tests) that don't track a firer.
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player,
           std::weak_ptr<CMPUT350::GameObject> firer = std::weak_ptr<CMPUT350::GameObject>());
    bool IsPlayerBullet();

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill();

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::ShapeUnion>& GetShapes() override;

private:
    // Rebuilds mShapes (a local-space Line) from the current prev/current positions.
    // Must be called any time mPrevLocation/mCurrentLocation change.
    void RebuildShapes();

    // Location of the bullet at the previous frame
    CMPUT350::Point2D mPrevLocation;
    // Location of the bullet at the current frame
    CMPUT350::Point2D mCurrentLocation;
    // Heading of the bullet
    CMPUT350::Point2D mHeading;
    // True if the bullet is fired by the player
    bool mIsPlayer;
    // True if the bullet is alive
    bool mAlive;
    // Bounding box of the bullet
    CMPUT350::Rect mBounds;
    // Local-space collision shapes, rebuilt every Update since the bullet moves
    std::vector<CMPUT350::ShapeUnion> mShapes;
    // Who fired this bullet, so it can be excluded from collisions against its own shooter
    std::weak_ptr<CMPUT350::GameObject> mFirer;
};
#endif  // BULLET_H