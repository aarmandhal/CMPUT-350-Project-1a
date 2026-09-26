#include "GameEngine.h"
#include <iostream>
#include <vector>
#include "DrawContext.h"
#include "GameContext.h"
#include "CollisionObject.h"
#include "GraphicsObject.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    // Sample font loading code
        mFont = std::make_shared<sf::Font>(); // load font

    	if (!mFont->openFromMemory(&_font, _font_len))
    	{
    		fprintf(stderr, "WARNING: Font did not load.\n");
    	}

        mWindow = std::make_shared<sf::RenderWindow> (sf::VideoMode(sf::Vector2u(width, height)), name); // load window
        mWindow->setFramerateLimit(30); // set fps to 30
}

GameEngine::~GameEngine() {
    // Cleanup resources
    mWindow->close(); // destructor cleans up window
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mNewGameObjects.push_back(gameObject); // add objects (gameObject) to the game (runs twice)
}

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 */


void GameEngine::Run() {

    DrawContext drawContext(mWindow, mFont); // gives the game objects drawing access

    GameContext context;

    context.mEngineView = this; // pointer to the current GameEngine object
    context.ScreenContext = &drawContext; // give me the address of drawContex

    while (mWindow->isOpen())  // window is open
    {
        // 0. Remove any objects that are now dead
        mGameObjects.erase(
            std::remove_if(
                // go from the beggining to the end of the vector
                mGameObjects.begin(),
                mGameObjects.end(),
                // take the current shared pointer we're checking and call it gameObject
                [](const std::shared_ptr<GameObject>& gameObject)
                {
                    return !gameObject->IsAlive(); // return True if the object
                }
            ),
            mGameObjects.end()
        );

        // 1. Activate and initialize any objects added during the last frame
        for (const auto& gameObject : mNewGameObjects) { // for every shared pointer in mNewGameObjects
            mGameObjects.push_back(gameObject); // add that pointer to the active vector:
            gameObject->Initialize(&context);
        }

        mNewGameObjects.clear(); // remove everything from the pending vector

        // 2. Process events
        // Note: use -> because you're accessing a shared_ptr, not the object itself (use '.' in that case)
        while (const std::optional<sf::Event> event = mWindow->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mWindow->close();
            }
        }

        // 3. Update game objects
        // upate every game object in mGameObjects
        for(const auto& gameObject : mGameObjects) {
            gameObject->Update(&context);
        }

        // 4. Process collision events
        // loop through every game object
        for (size_t a = 0; a < mGameObjects.size(); a++) {
            // start at the object after a so we don't check the same pair twice
            for (size_t b = a + 1; b < mGameObjects.size(); b++) {
            
                // check if objects are both collision objects
                auto objA = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[a]);
                auto objB = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[b]);

                // if either of them aren't collision objects, skip
                if (objA == nullptr || objB == nullptr) {
                    continue;
                }

                // get the bounding rectangle for each collision object
                // every object must have GetBounds
                const Rect& boundsA = objA->GetBounds();
                const Rect& boundsB = objB->GetBounds();

                // check whether the two bounding rectangles overlap
                bool overlap =
                    boundsA.topLeft.x < boundsB.topLeft.x + boundsB.width && // is As left before Bs right
                    boundsA.topLeft.x + boundsA.width > boundsB.topLeft.x && // is As right before Bs left
                    boundsA.topLeft.y < boundsB.topLeft.y + boundsB.height && // is As top above Bs bottom
                    boundsA.topLeft.y + boundsA.height > boundsB.topLeft.y; // is As bottom below Bs top

                // tell both objects that they collided
                if (overlap)
                {
                    objA->CollisionEnter(objB);
                    objB->CollisionEnter(objA);
                }

            }
        }

        // 5. Late updates
        // give each active object a chance to update after collision processing. Logic that should happen after collisions
        for(const auto& gameObject : mGameObjects) {
            gameObject -> LateUpdate(&context);
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (const auto& gameObject : mGameObjects) {
            // check if this gameObject is a GraphicsObject since only they should be rendered
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(gameObject);

            if (graphicsObj != nullptr) {
                graphicsObj -> RenderBackground(&context); // lets that object draw anything it considers background contents
            }
        }

        // 7. Render foreground
        for(const auto& gameObject : mGameObjects) {
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(gameObject);

            if (graphicsObj != nullptr) {
                graphicsObj -> RenderForeground(&context);
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
