#include "GameEngine.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "CollisionObject.h"
#include "DrawContext.h"
#include "GameContext.h"
#include "GraphicsObject.h"
#include "MathUtil.h"

using namespace CMPUT350;

const float EPSILON = 0.0001f;

Point2D rotatePoint(Point2D p, float angle) {
    return Point2D(p.x * std::cos(angle) - p.y * std::sin(angle),
                   p.x * std::sin(angle) + p.y * std::cos(angle));
}

std::vector<ShapeUnion> transformShapes(const std::vector<ShapeUnion>& localShapes,
                                        Point2D location, float rotation) {
    std::vector<ShapeUnion> worldShapes;
    for (const auto& shape : localShapes) {
        if (shape.type == ShapeType::CIRCLE) {
            Point2D newCenter = rotatePoint(shape.circle.center, rotation) + location;
            worldShapes.push_back(Circle(newCenter, shape.circle.radius));
        } else if (shape.type == ShapeType::LINE) {
            Point2D newP1 = rotatePoint(shape.line.p1, rotation) + location;
            Point2D newP2 = rotatePoint(shape.line.p2, rotation) + location;
            worldShapes.push_back(Line(newP1, newP2));
        } else if (shape.type == ShapeType::RECT) {
            Point2D corners[4] = {
                shape.rect.topLeft,
                shape.rect.topLeft + Point2D(shape.rect.width, 0),
                shape.rect.topLeft + Point2D(shape.rect.width, shape.rect.height),
                shape.rect.topLeft + Point2D(0, shape.rect.height),
            };
            for (int i = 0; i < 4; i++) {
                corners[i] = rotatePoint(corners[i], rotation) + location;
            }
            for (int i = 0; i < 4; i++) {
                worldShapes.push_back(Line(corners[i], corners[(i + 1) % 4]));
            }
        }
    }
    return worldShapes;
}

bool CircleVsCircle(Circle a, Circle b, Point2D& out) {
    Point2D diff = a.center - b.center;
    float distSq = diff * diff;  // operator* IS the dot product
    float radiusSum = a.radius + b.radius;

    if (distSq > radiusSum * radiusSum) {
        return false;
    }

    float dist = std::sqrt(distSq);
    Point2D normal =
        dist > EPSILON ? diff * (1.0f / dist) : Point2D(1, 0);  // no operator/, use operator*
    out = a.center - normal * a.radius;
    return true;
}

bool LineVsLine(Line a, Line b, Point2D& out) {
    return a.Crosses(b, out);  // reuse Line's own tested method
}

bool LineVsCircle(Line l, Circle c, Point2D& out) {
    Point2D closest = l.ClosestPoint(c.center);  // reuse Line's own tested method
    Point2D diff = closest - c.center;
    if (diff * diff > c.radius * c.radius) {
        return false;
    }
    out = closest;
    return true;
}

bool ShapesIntersect(ShapeUnion a, ShapeUnion b, Point2D& out) {
    if (a.type == ShapeType::Circle && b.type == ShapeType::Circle) {
        return CircleVsCircle(a.circle, b.circle, out);
    } else if (a.type == ShapeType::Line && b.type == ShapeType::Line) {
        return LineVsLine(a.line, b.line, out);
    } else if (a.type == ShapeType::Line && b.type == ShapeType::Circle) {
        return LineVsCircle(a.line, b.circle, out);
    } else if (a.type == ShapeType::Circle && b.type == ShapeType::Line) {
        return LineVsCircle(b.line, a.circle, out);
    }
    return false;
}

/// @brief
namespace CMPUT350 {
#include "FontData.h"

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    // Sample font loading code
    mFont = std::make_shared<sf::Font>();  // load font

    if (!mFont->openFromMemory(&_font, _font_len)) {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }

    mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(width, height)),
                                                 name);  // load window
    mWindow->setFramerateLimit(30);                      // set fps to 30
}

GameEngine::~GameEngine() {
    // Cleanup resources
    mWindow->close();  // destructor cleans up window
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mNewGameObjects.push_back(gameObject);  // add objects (gameObject) to the game (runs twice)
}

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is
 * closed or all objects have been destroyed.
 */

void GameEngine::Run() {
    DrawContext drawContext(mWindow, mFont);  // gives the game objects drawing access

    GameContext context;

    context.mEngineView = this;            // give rhe context access to this engine
    context.ScreenContext = &drawContext;  // give the context access to this drawing helper

    while (mWindow->isOpen())  // window is open
    {
        // 0. Remove any objects that are now dead
        mGameObjects.erase(
            std::remove_if(
                // go from the beggining to the end of the vector
                mGameObjects.begin(), mGameObjects.end(),
                // take the current shared pointer we're checking and call it gameObject
                [](const std::shared_ptr<GameObject>& gameObject) {
                    return !gameObject->IsAlive();  // return True if the object
                }),
            mGameObjects.end());

        // 1. Activate and initialize any objects added during the last frame
        for (const auto& gameObject :
             mNewGameObjects) {                  // for every shared pointer in mNewGameObjects
            mGameObjects.push_back(gameObject);  // add that pointer to the active vector:
            gameObject->Initialize(&context);
        }

        mNewGameObjects.clear();  // remove everything from the pending vector

        // 2. Process events

        // Note: use -> because you're accessing a shared_ptr, not the object itself (use '.' in
        // that case) ask the window for the next event and store the result in event
        while (const std::optional<sf::Event> event = mWindow->pollEvent()) {
            // checks if the event received is a closed event (closing window)
            if (event->is<sf::Event::Closed>()) {
                mWindow->close();
            }

            // checks if current event is a textEntered event
            else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>()) {
                // check if input is lowercase, so < 128
                if (keyPressed->unicode < 128) {
                    // convert number to a char (HandleKeyEvent expects that)
                    char key = static_cast<char>(keyPressed->unicode);

                    std::cout << "Key entered: " << key << std::endl;

                    // notify every gameObject of a key press and send it to them
                    for (const auto& gameObject : mGameObjects) {
                        gameObject->HandleKeyEvent(&context, key);
                    }
                }
            }
        }

        // 3. Update game objects
        // upate every game object in mGameObjects
        for (const auto& gameObject : mGameObjects) {
            gameObject->Update(&context);
        }

        // 4. Process collision events
        // loop through every game object
        for (size_t a = 0; a < mGameObjects.size(); a++) {
            for (size_t b = a + 1; b < mGameObjects.size(); b++) {
                auto objA = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[a]);
                auto objB = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[b]);

                if (objA == nullptr || objB == nullptr) {
                    continue;
                }

                Rect boundsA = objA->GetBounds();
                Rect boundsB = objB->GetBounds();

                bool overlap = boundsA.topLeft.x < boundsB.topLeft.x + boundsB.width &&
                               boundsA.topLeft.x + boundsA.width > boundsB.topLeft.x &&
                               boundsA.topLeft.y < boundsB.topLeft.y + boundsB.height &&
                               boundsA.topLeft.y + boundsA.height > boundsB.topLeft.y;

                if (overlap) {
                    auto shapesA = transformShapes(objA->GetShapes(), objA->GetLocation(),
                                                   objA->GetRotation());
                    auto shapesB = transformShapes(objB->GetShapes(), objB->GetLocation(),
                                                   objB->GetRotation());

                    Point2D collisionPoint;
                    bool hit = false;

                    for (const auto& shapeA : shapesA) {
                        for (const auto& shapeB : shapesB) {
                            if (ShapesIntersect(shapeA, shapeB, collisionPoint)) {
                                hit = true;
                                break;
                            }
                        }
                        if (hit) break;
                    }

                    if (hit) {
                        objA->CollisionEnter(objB);
                        objB->CollisionEnter(objA);
                        objA->CollisionEnter(objB, collisionPoint);
                        objB->CollisionEnter(objA, collisionPoint);
                    }
                }
            }
        }

        // 5. Late updates
        // give each active object a chance to update after collision processing. Logic that
        // should happen after collisions
        for (const auto& gameObject : mGameObjects) {
            gameObject->LateUpdate(&context);
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (const auto& gameObject : mGameObjects) {
            // check if this gameObject is a GraphicsObject since only they should be rendered
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(gameObject);

            if (graphicsObj != nullptr) {
                graphicsObj->RenderBackground(&context);  // lets that object draw anything it
                                                          // considers background contents
            }
        }

        // 7. Render foreground
        for (const auto& gameObject : mGameObjects) {
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(gameObject);

            if (graphicsObj != nullptr) {
                graphicsObj->RenderForeground(&context);
            }
        }

        // Actually render to window
        mWindow->display();
    }
}

// Sample code for processing events

// bool GameEngine::ProcessEvents(GameContext *context)
//{
//	while (const std::optional event = mWindow->pollEvent())
//	{
//		if (event->is<sf::Event::Closed>())
//		{
//		}
//		else if (event->is<sf::Event::Resized>())
//		{
//		}
//		else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
//		{
//			// use keyPressed->unicode to get character
//		}
//	}
// }

}  // namespace CMPUT350