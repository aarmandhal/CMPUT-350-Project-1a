#include "Bullet.h"

#include "Enemy.h"
#include "Player.h"

// Constants
// Width of bullet
const int LINE_WIDTH = 2;
// True to show boxes, false to hide boxes
const bool DEBUG_BOXES = false;
// Width of debug line
const int DEBUG_LINE = 1;

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player) {
    // Set previous location to current location
    this->mPrevLocation = location;
    // Set current location
    this->mCurrentLocation = location;
    // Set heading
    this->mHeading = heading;
    // Set if bullet is fired by player
    this->mIsPlayer = player;
    // Bullet starts alive
    this->mAlive = true;
    // Set bounding box
    this->mBounds = CMPUT350::Rect(this->mPrevLocation, this->mCurrentLocation);
}

// Return if bullet is fired by player
bool Bullet::IsPlayerBullet() { return this->mIsPlayer; }

// Empty as constructor already did all the setup the object needs
void Bullet::Initialize(CMPUT350::GameContext* context) {}

// Update bullet location
void Bullet::Update(CMPUT350::GameContext* context) {
    // Don't update if bullet is dead
    if (!this->mAlive) {
        return;
    }
    // Update previous location
    this->mPrevLocation = this->mCurrentLocation;
    // Update current location
    this->mCurrentLocation += this->mHeading;
    // Update bounding box
    this->mBounds = CMPUT350::Rect(this->mPrevLocation, this->mCurrentLocation);
}

// Check if bullet is outside of screen
void Bullet::LateUpdate(CMPUT350::GameContext* context) {
    // Don't update if bullet is dead
    if (!this->mAlive) {
        return;
    }
    // Get screen width
    float width = context->ScreenContext->GetWindowWidth();
    // Get screen height
    float height = context->ScreenContext->GetWindowHeight();
    // Kill bullet if it is outside of screen
    if (this->mCurrentLocation.x < 0 || this->mCurrentLocation.x > width ||
        this->mCurrentLocation.y < 0 || this->mCurrentLocation.y > height) {
        // Remove bullet from scene
        this->Kill();
    }
}

// Empty as bullets ignore keyboard input
bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key) { return false; }

// Empty as bullets only draw in the foreground pass
void Bullet::RenderBackground(CMPUT350::GameContext* context) {}

// Draw bullets
void Bullet::RenderForeground(CMPUT350::GameContext* context) {
    // Don't draw if bullet is dead
    if (!this->mAlive) {
        return;
    }
    // Set color based on if bullet is fired by player
    CMPUT350::RGBColor color = this->mIsPlayer ? CMPUT350::Colors::red : CMPUT350::Colors::blue;
    // Draw bullet as a line
    context->ScreenContext->DrawLine(this->mPrevLocation, this->mCurrentLocation, LINE_WIDTH,
                                     color);
    // Draw debug box if enabled
    if (DEBUG_BOXES) {
        context->ScreenContext->FrameRect(this->mBounds, DEBUG_LINE, CMPUT350::Colors::yellow);
    }
}

// Process collision with another object
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) {
    // Don't process collisions if bullet is dead
    if (!this->mAlive) {
        return;
    }
    // If bullet is fired by player
    if (this->mIsPlayer) {
        // If bullet collides with enemy
        if (std::dynamic_pointer_cast<Enemy>(obj).get() != nullptr) {
            // Kill bullet
            this->Kill();
        }
        // If bullet is fired by enemy
    } else {
        // If bullet collides with player
        if (std::dynamic_pointer_cast<Player>(obj).get() != nullptr) {
            // Kill bullet
            this->Kill();
        }
    }
}

// Kill the bullet
void Bullet::Kill() { this->mAlive = false; }

// Check if bullet is alive
bool Bullet::IsAlive() const {
    // Return if bullet is alive
    return this->mAlive;
}

// Get bounding box of bullet
const CMPUT350::Rect& Bullet::GetBounds() { return this->mBounds; }
