#ifndef BULLET_H
#define BULLET_H

#include <vector>

#include "CollisionObject.h"
#include "GameContext.h"

class Bullet : public CMPUT350::CollisionObject {
public:
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player);
    bool IsPlayerBullet();

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool HandleKeyEvent(CMPUT350::GameContext* context, char key) override;
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
};
#endif  // BULLET_H