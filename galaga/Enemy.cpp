#include "Enemy.h"

#include <cmath>
#include <cstdlib>

#include "Bullet.h"

#include "NotificationManager.h"

// Constants
// Width of enemy
const int ENEMY_WIDTH = 18;
// Height of enemy
const int ENEMY_HEIGHT = 18;
// False to hide boxes, True to show boxes
const bool DEBUG_BOXES = false;
// Width of debug line
const int DEBUG_LINE = 1;
const float PI = 3.14159265f;
const float ENEMY_RETURN_SPEED = 1.5f;
const CMPUT350::Point2D ENEMY_HALF_SIZE(ENEMY_WIDTH / 2.0f, ENEMY_HEIGHT / 2.0f);
// Broad-phase bounds are padded to the half-diagonal so a rotated enemy never pokes outside them
const float ENEMY_BOUNDS_RADIUS = std::ceil(ENEMY_HALF_SIZE.Distance(CMPUT350::Point2D(0, 0)));
// Members

Enemy::Enemy(CMPUT350::Point2D loc, CMPUT350::RGBColor color, int health, int pointValue) {
    // Set location
    this->mLocation = loc;
    this->mFormationLocation = loc;
    // Set alive to true
    this->mAlive = true;
    this->mHealth = health;
    this->mPointVal = pointValue;
    this->mColor = color;
    // Set bounding box
    this->mBounds = CMPUT350::Rect(loc, ENEMY_BOUNDS_RADIUS);

    // Local-space shape, centered on local origin
    CMPUT350::Rect localRect = CMPUT350::Rect(
        CMPUT350::Point2D(-ENEMY_WIDTH / 2.0f, -ENEMY_HEIGHT / 2.0f), ENEMY_WIDTH, ENEMY_HEIGHT);
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

// Rotation follows the attack path; 0 while in formation
float Enemy::GetRotation() const { return this->mRotation; }

// Empty as constructor already did all the setup the object needs
void Enemy::Initialize(CMPUT350::GameContext* context) {
    this->mContext = context;
    this->mSelf = context->CurrObject;
    this->mNextFireFrame = context->currentFrame + CMPUT350::RandomInt(90, 240);
}

void Enemy::StartAttack() {
    if (this->mAttackState != AttackState::Formation ||
        this->mAttackPath.controlPoints.size() < 4) {
        return;
    }

    this->mAttackState = AttackState::Attacking;
    this->mPathT = 0.0f;

    // schedule a shot shortly after starting an attack
    if (this->mContext != nullptr) {
        this->mNextFireFrame =
            this->mContext->currentFrame + CMPUT350::RandomInt(10, 25);
    }
}

// Empty as Enemies are stationary
void Enemy::Update(CMPUT350::GameContext* context) {
    if (!this->mAlive) {
        return;
    }
    if (this->mAttackState == AttackState::Attacking) {
        this->mPathT += this->mPathSpeed;
        int maxT = (this->mAttackPath.controlPoints.size() - 1) / 3;
        if (this->mPathT >= maxT) {
            this->mAttackState = AttackState::Returning;
        } else {
            this->mLocation = this->mAttackPath.GetPoint(this->mPathT);
            CMPUT350::Point2D slope = this->mAttackPath.GetSlope(this->mPathT);
            this->mRotation = std::atan2(slope.y, slope.x) + (PI / 2.0f);
            RebuildBoundsAt(this->mLocation);
        }
    } else if (this->mAttackState == AttackState::Returning) {
        CMPUT350::Point2D dir = this->mFormationLocation - this->mLocation;
        if (dir.Distance(CMPUT350::Point2D(0, 0)) < ENEMY_RETURN_SPEED) {
            this->mLocation = this->mFormationLocation;
            this->mAttackState = AttackState::Formation;
            this->mRotation = 0.0f;
            RebuildBoundsAt(this->mLocation);
        } else {
            dir.Normalize();
            this->mLocation += dir * ENEMY_RETURN_SPEED;
            RebuildBoundsAt(this->mLocation);
        }
    }
    // only shoot while attacking
    if (this->mAttackState == AttackState::Attacking &&
        context->currentFrame >= this->mNextFireFrame) {

        FireAt(context);
        this->mNextFireFrame = context->currentFrame + CMPUT350::RandomInt(30, 60);
    }
}

void Enemy::FireAt(CMPUT350::GameContext* context) {
    CMPUT350::Point2D heading;
    heading = CMPUT350::Point2D(0.0f, 6.0f);
    std::shared_ptr<Bullet> bullet =
        std::make_shared<Bullet>(this->mLocation, heading, false, this->mSelf);
    context->mEngineView->AddGameObject(bullet);
}

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
    context->ScreenContext->DrawRect(mLocalBounds, this->mColor);
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
        this->mHealth -= 1;

        // change color when the enemy has one health remaining
        if (this->mHealth == 1) {
            this->mColor = CMPUT350::Colors::red;
        }

        if (this->mHealth <= 0) {
            this->Kill();
            if (this->mContext != nullptr) {
                this->mContext->mNotificationManager->Notify("enemy_destroyed");
            }
        }
    }
}

// Kill the enemy
void Enemy::Kill() { this->mAlive = false; }

// Check if enemy is alive
bool Enemy::IsAlive() const { return this->mAlive; }

// Get bounding box of enemy
const CMPUT350::Rect& Enemy::GetBounds() { return this->mBounds; }

void Enemy::RebuildBoundsAt(CMPUT350::Point2D loc) {
    this->mBounds = CMPUT350::Rect(loc, ENEMY_BOUNDS_RADIUS);
}