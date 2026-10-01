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
    // TODO: Update code
    // Constructor for enemy
    // Set location
    this->mLocation = loc;
    // Set alive to true
    this->mAlive = true;
    // Set bounding box
    CMPUT350::Point2D halfSize(ENEMY_WIDTH / 2, ENEMY_HEIGHT / 2);
    this->mBounds = CMPUT350::Rect(loc - halfSize, loc + halfSize);
}

// Empty as constructor already did all the setup the object needs
void Enemy::Initialize(CMPUT350::GameContext* context) {}

// Empty as Enemies are stationary
void Enemy::Update(CMPUT350::GameContext* context) {}

// Empty as nothing depends on the result of this frame's collisions
void Enemy::LateUpdate(CMPUT350::GameContext* context) {}

// Enemies ignore keyboard input
bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key) { return false; }

// Empty as enemy only draws in the foreground pass only
void Enemy::RenderBackground(CMPUT350::GameContext* context) {}

// Draw enemy in the foreground pass
void Enemy::RenderForeground(CMPUT350::GameContext* context) {
    // Don't draw if enemy is dead
    if (!this->mAlive) {
        return;
    }
    // Draw enemy as a green square
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::green);
    // Draw debug box if enabled
    if (DEBUG_BOXES) {
        context->ScreenContext->FrameRect(mBounds, DEBUG_LINE, CMPUT350::Colors::yellow);
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
