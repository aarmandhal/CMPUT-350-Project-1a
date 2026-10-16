#include "Enemy.h"

#include "Bullet.h"

// Constants
// Width of enemy
const int ENEMY_WIDTH = 18;
// Height of enemy
const int ENEMY_HEIGHT = 18;
// False to hide boxes, True to show boxes
const bool DEBUG_BOXES = false;
// Width of debug line
const int DEBUG_LINE = 1;

// Members

Enemy::Enemy(CMPUT350::Point2D loc) {
    // set location
    this->mLocation = loc;
    // set alive to true
    this->mAlive = true;
    // set bounding box
    CMPUT350::Vector2D halfSize(ENEMY_WIDTH / 2.0f, ENEMY_HEIGHT / 2.0f);
    this->mBounds = CMPUT350::Rect(loc - halfSize, ENEMY_WIDTH, ENEMY_HEIGHT);

    // Local-space shape, centered on local origin
    CMPUT350::Rect localRect = CMPUT350::Rect(
        CMPUT350::Point2D(-ENEMY_WIDTH / 2.0f, -ENEMY_HEIGHT / 2.0f),
        ENEMY_WIDTH, ENEMY_HEIGHT);
    this->mLocalBounds = localRect;

    CMPUT350::ShapeUnion shapeUnion;
    shapeUnion.type = CMPUT350::ShapeType::Rect;
    shapeUnion.rect = localRect;
    this->mShapes.push_back(shapeUnion);
}

const std::vector<CMPUT350::ShapeUnion>& Enemy::GetShapes() { return this->mShapes; }

// Enemy's own position — the engine uses this to offset the draw context,
// so RenderForeground below draws relative to this, not in absolute coordinates
CMPUT350::Point2D Enemy::GetLocation() const { return this->mLocation; }

// Enemies don't rotate
float Enemy::GetRotation() const { return 0.0f; }

// Empty as constructor already did all the setup the object needs
void Enemy::Initialize(CMPUT350::GameContext* context) {}

// Empty as Enemies are stationary
void Enemy::Update(CMPUT350::GameContext* context) {}

// Empty as nothing depends on the result of this frame's collisions
void Enemy::LateUpdate(CMPUT350::GameContext* context) {}

// Empty as enemy only draws in the foreground pass only
void Enemy::RenderBackground(CMPUT350::GameContext* context) {}

// Draw enemy in the foreground pass
void Enemy::RenderForeground(CMPUT350::GameContext* context) {
    // Don't draw if enemy is dead
    if (!this->mAlive) {
        return;
    }
    // Draw enemy as a green square, in local space — the engine has already offset
    // the draw context by GetLocation(), so mBounds (world space) would double-offset it
    context->ScreenContext->DrawRect(mLocalBounds, CMPUT350::Colors::green);
    // Draw debug box if enabled
    if (DEBUG_BOXES) {
        context->ScreenContext->FrameRect(mLocalBounds, DEBUG_LINE, CMPUT350::Colors::yellow);
    }
}

// Process collision with another object
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) {
    // Don't process collisions if enemy is dead
    if (!this->mAlive) {
        return;
    }
    // Try to cast the object to a bullet
    std::shared_ptr<Bullet> b = std::dynamic_pointer_cast<Bullet>(obj);
    // If it's not a bullet, return
    if (b == nullptr) {
        return;
    }
    // If it's a player bullet, kill the enemy
    if (b->IsPlayerBullet()) {
        this->Kill();
    }
}

// Kill the enemy
void Enemy::Kill() { this->mAlive = false; }

// Check if enemy is alive
bool Enemy::IsAlive() const { return this->mAlive; }

// Get bounding box of enemy
const CMPUT350::Rect& Enemy::GetBounds() { return this->mBounds; }