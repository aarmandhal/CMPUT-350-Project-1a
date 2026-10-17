#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"

class Enemy : public CMPUT350::CollisionObject {
public:
    Enemy(CMPUT350::Point2D loc, CMPUT350::RGBColor color, int health, int pointValue);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;
    void FireAt(CMPUT350::GameContext* context);
    void StartAttack();

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    // return the enemy's position and rotation and point value
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;
    int GetPointValue() const { return this->mPointVal; }
    bool IsInFormation() const { return this->mAttackState == AttackState::Formation; }

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
    void RebuildBoundsAt(CMPUT350::Point2D loc);

protected:
    int mHealth;
    int mPointVal;
    CMPUT350::RGBColor mColor = CMPUT350::Colors::green;
    CMPUT350::GameContext* mContext = nullptr;
    std::weak_ptr<CMPUT350::GameObject> mSelf;
    int mNextFireFrame = 0;

    enum class AttackState { Formation, Attacking, Returning };
    CMPUT350::Bezier mAttackPath;
    AttackState mAttackState = AttackState::Formation;
    CMPUT350::Point2D mFormationLocation;
    float mPathT = 0.0f;
    float mPathSpeed = 0.03f;
    float mRotation = 0.0f;
};

#endif