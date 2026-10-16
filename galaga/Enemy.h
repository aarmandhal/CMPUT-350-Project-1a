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

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::ShapeUnion>& GetShapes() override;

private:
    // True if alive, false if dead
    bool mAlive;
    // Bounding box of the enemy
    CMPUT350::Rect mBounds;
    // Location of the enemy
    CMPUT350::Point2D mLocation;
    // Local-space collision shapes, built once in the constructor
    std::vector<CMPUT350::Shape> mShapes;
};

#endif
