#include <iostream>
#include <random>

#include "Enemy.h"
#include "GameEngine.h"
#include "Player.h"
#include "Stars.h"

class DummyBall : public CMPUT350::CollisionObject {
public:
    DummyBall(CMPUT350::Point2D loc, int radius) : CollisionObject(), mLoc(loc), mRadius(radius) {}

    void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override {}

    void CollisionEnter(const std::shared_ptr<CollisionObject> &obj,
                        const CMPUT350::Point2D &collisionPoint) override {
        std::cout << "DummyBall hit something at " << collisionPoint << std::endl;
    }

    std::vector<CMPUT350::ShapeUnion> GetShapes() override {
        CMPUT350::ShapeUnion shape;
        shape.type = CMPUT350::ShapeType::Circle;
        shape.circle = CMPUT350::Circle(mLoc, mRadius);
        return {shape};
    }

    const CMPUT350::Rect &GetBounds() override {
        static CMPUT350::Rect bounds({0, 0}, 0, 0);
        bounds = CMPUT350::Rect(mLoc - mRadius, 2 * mRadius, 2 * mRadius);
        return bounds;
    }

    bool HandleKeyEvent(CMPUT350::GameContext *context, char key) override {
        const float step = 8.0f;
        if (key == 'w') mLoc.y -= step;
        if (key == 's') mLoc.y += step;
        if (key == 'a') mLoc.x -= step;
        if (key == 'd') mLoc.x += step;
        return true;
    }

    bool IsAlive() const override { return true; }

    void RenderForeground(CMPUT350::GameContext *context) override {
        context->ScreenContext->DrawCircle(mLoc, mRadius, CMPUT350::Colors::red);
    }

private:
    CMPUT350::Point2D mLoc;
    int mRadius;
};

// --- Rect dummy (stationary) ---
class DummyRect : public CMPUT350::CollisionObject {
public:
    DummyRect(CMPUT350::Rect bounds) : CollisionObject(), mBounds(bounds) {}

    void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override {}

    void CollisionEnter(const std::shared_ptr<CollisionObject> &obj,
                        const CMPUT350::Point2D &collisionPoint) override {
        std::cout << "DummyRect hit something at " << collisionPoint << std::endl;
    }

    std::vector<CMPUT350::ShapeUnion> GetShapes() override {
        CMPUT350::ShapeUnion shape;
        shape.type = CMPUT350::ShapeType::Rect;
        shape.rect = mBounds;
        return {shape};
    }

    const CMPUT350::Rect &GetBounds() override { return mBounds; }

    bool IsAlive() const override { return true; }

    void RenderForeground(CMPUT350::GameContext *context) override {
        context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::green);
    }

private:
    CMPUT350::Rect mBounds;
};

// --- Line dummy (stationary) ---
class DummyLine : public CMPUT350::CollisionObject {
public:
    DummyLine(CMPUT350::Point2D p1, CMPUT350::Point2D p2) : CollisionObject(), mLine(p1, p2) {}

    void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override {}

    void CollisionEnter(const std::shared_ptr<CollisionObject> &obj,
                        const CMPUT350::Point2D &collisionPoint) override {
        std::cout << "DummyLine hit something at " << collisionPoint << std::endl;
    }

    std::vector<CMPUT350::ShapeUnion> GetShapes() override {
        CMPUT350::ShapeUnion shape;
        shape.type = CMPUT350::ShapeType::Line;
        shape.line = mLine;
        return {shape};
    }

    const CMPUT350::Rect &GetBounds() override {
        static CMPUT350::Rect bounds({0, 0}, 0, 0);
        bounds = CMPUT350::Rect(mLine.p1, mLine.p2);
        return bounds;
    }

    bool IsAlive() const override { return true; }

    void RenderForeground(CMPUT350::GameContext *context) override {
        context->ScreenContext->DrawLine(mLine.p1, mLine.p2, 3.0f, CMPUT350::Colors::cyan);
    }

private:
    CMPUT350::Line mLine;
};

class Ball : public CMPUT350::CollisionObject {
public:
    Ball(int radius)
        : CollisionObject(),
          mRadius(radius),
          mNumCollisions(0),
          mLoc(radius, radius),
          mSpeed(3, 3),
          mSpeedNext(3, 3),
          mFrameCollisions(0) {
        std::uniform_int_distribution<int> xRand(mRadius, 1024 - mRadius);
        std::uniform_int_distribution<int> yRand(mRadius, 768 - mRadius);
        std::uniform_int_distribution<int> speed(1, 5);

        // Initialize ball with a random position and speed
        mLoc.x = xRand(gen);
        mLoc.y = yRand(gen);
        mSpeed.x = speed(gen);  // Random speed between 1 and 5
        mSpeed.y = speed(gen);  // Random speed between 1 and 5
    }

    void Update(CMPUT350::GameContext *context) override {
        // Update ball position or state
        mLoc += mSpeedNext;
        mSpeed = mSpeedNext;
        if (mLoc.x > 1024 - mRadius || mLoc.x < mRadius) {
            mSpeed.x = -mSpeed.x;
            if (mSpeed.x > 0) mSpeed.x++;
        }
        if (mLoc.y > 768 - mRadius || mLoc.y < mRadius) {
            mSpeed.y = -mSpeed.y;
            if (mSpeed.y > 0) mSpeed.y++;
        }
        if (mNumCollisions % 10 == 8) {
            std::uniform_int_distribution<int> size(50, 150);
            context->mEngineView->AddGameObject(std::make_shared<Ball>(size(gen)));
            mNumCollisions++;
        }
        mSpeedNext = mSpeed;
    }

    void LateUpdate(CMPUT350::GameContext *context) override {
        if (mFrameCollisions > 0) mNumCollisions++;
        mFrameCollisions = 0;
    }

    void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override {
        std::shared_ptr<Ball> ball = std::dynamic_pointer_cast<Ball>(obj);
        // Only collisions with other Balls
        if (ball != nullptr) {
            CMPUT350::Point2D b1 = mLoc;
            CMPUT350::Point2D b2 = ball->mLoc;
            if (b1.Distance(b2) <= ball->mRadius + mRadius)  // Are the balls within radius
            {
                mFrameCollisions++;
                CMPUT350::Point2D normal = b1 - b2;
                normal /= normal.Distance(CMPUT350::Point2D(0, 0));
                mCollisionPoint = normal * (-mRadius);
                float m1 = 3.14f * mRadius * mRadius;              // mass of this ball
                float m2 = 3.14f * ball->mRadius * ball->mRadius;  // mass of other ball
                CMPUT350::Point2D v1prime =
                    mSpeed - (2 * m2 / (m1 + m2)) * ((normal * (mSpeed - ball->mSpeed)) * normal);
                mSpeedNext = v1prime;
            }
        }
    }

    const CMPUT350::Rect &GetBounds() override {
        static CMPUT350::Rect bounds({0, 0}, 0, 0);  // Initialize with dummy values
        bounds = CMPUT350::Rect(mLoc - mRadius, 2 * mRadius, 2 * mRadius);
        return bounds;
    }

    std::vector<CMPUT350::ShapeUnion> GetShapes() override {
        CMPUT350::ShapeUnion shape;
        shape.type = CMPUT350::ShapeType::Circle;
        shape.circle = CMPUT350::Circle(mLoc, mRadius);
        return {shape};
    }

    bool IsAlive() const override { return mNumCollisions < 10; }

    void RenderForeground(CMPUT350::GameContext *context) override {
        context->ScreenContext->DrawCircle(mLoc, mRadius, CMPUT350::Colors::red);
        context->ScreenContext->DrawCenteredText("Circle" + std::to_string(mNumCollisions),
                                                 mRadius / 6.0f, mLoc, CMPUT350::Colors::white);
        context->ScreenContext->DrawLine(mLoc, mLoc + mCollisionPoint, mRadius * 0.1f,
                                         CMPUT350::Colors::blue);
    }

private:
    static std::random_device rd;
    static std::mt19937 gen;

    int mRadius, mNumCollisions, mFrameCollisions;
    CMPUT350::Point2D mLoc, mSpeed, mSpeedNext, mCollisionPoint;
};

// Shared randoms across all balls
// These are the actual objeects associated with the class. Normally these
// would be in a C++ file
std::random_device Ball::rd;
std::mt19937 Ball::gen(rd());

int main() {
    bool mBallSsample = true;

    if (mBallSsample) {
        CMPUT350::GameEngine engine(1024, 768, "Ball Simulation");
        engine.AddGameObject(std::make_shared<Ball>(100));
        engine.AddGameObject(std::make_shared<Ball>(50));
        engine.AddGameObject(std::make_shared<DummyBall>(CMPUT350::Point2D(200, 400), 40));
        engine.AddGameObject(
            std::make_shared<DummyRect>(CMPUT350::Rect(CMPUT350::Point2D(500, 300), 150, 100)));
        engine.AddGameObject(
            std::make_shared<DummyLine>(CMPUT350::Point2D(600, 100), CMPUT350::Point2D(800, 500)));
        engine.Run();
    } else {
        CMPUT350::GameEngine engine(768, 1024, "Galaga");
        auto player = std::make_shared<Player>(CMPUT350::Point2D(768 / 2, 900));
        engine.AddGameObject(player);
        engine.AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, 768, 1024)));
        // create 4 rows with 10 enemies in each row
        const int rows = 4;
        const int cols = 10;

        // width of the Galaga window
        const float screenWidth = 768.0f;

        // space each enemy evenly across a row
        const float horizontalSpacing = screenWidth / cols;

        // starting height of the first row
        const float startY = 100.0f;

        // space between each enemy row
        const float verticalSpacing = 50.0f;

        // create each row
        for (int row = 0; row < rows; row++) {
            // create 10 enemies across the current row
            for (int col = 0; col < cols; col++) {
                // calculate this enemy's horizontal position
                float enemyX = horizontalSpacing / 2 + col * horizontalSpacing;

                // calculate this enemy's vertical position
                float enemyY = startY + row * verticalSpacing;

                // create a new enemy at the calculated position
                auto enemy = std::make_shared<Enemy>(CMPUT350::Point2D(enemyX, enemyY));

                // add the enemy to the game engine
                engine.AddGameObject(enemy);
            }
        }
        engine.Run();
    }
    return 0;
}
