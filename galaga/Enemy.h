#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"

class Enemy : public CMPUT350::CollisionObject {
public:
    Enemy(CMPUT350::Point2D loc);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    // return the enemy's position and rotation
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::ShapeUnion>& GetShapes() override;

private:
    // True if alive, false if dead
    bool mAlive;
    // Bounding box of the enemy (world space, used for the broad-phase collision check)
    CMPUT350::Rect mBounds;
    // Local-space bounding rect, centered on the origin — used for drawing relative to
    // GetLocation(), which the engine now uses to offset the draw context
    CMPUT350::Rect mLocalBounds;
    // Location of the enemy
    CMPUT350::Point2D mLocation;
    // Local-space collision shapes, built once in the constructor
    std::vector<CMPUT350::ShapeUnion> mShapes;
};

#endif