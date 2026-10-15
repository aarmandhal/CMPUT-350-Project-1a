#include "GameEngine.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "DrawContext.h"
#include "GameContext.h"
#include "CollisionObject.h"
#include "GraphicsObject.h"
#include <unordered_map>
#include <string>
#include "NotificationManager.h"

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

// register a message for when a key is pressed
void GameEngine::InstallKeyDownNotification(int key, const std::string& notification) {
    mKeyDownNotifications[key].push_back(notification);
}

// register a message for when a key is released
void GameEngine::InstallKeyUpNotification(int key, const std::string& notification) {
    mKeyUpNotifications[key].push_back(notification);
}

// return an iterator to the first game object
GameEngine::const_iterator GameEngine::begin() const {
    return mGameObjects.begin();
}

// return an iterator past the last game object
GameEngine::const_iterator GameEngine::end() const {
    return mGameObjects.end();
}

// return a constant iterator to the first game object
GameEngine::const_iterator GameEngine::cbegin() const {
    return mGameObjects.cbegin();
}

// return a constant iterator past the last game object
GameEngine::const_iterator GameEngine::cend() const {
    return mGameObjects.cend();
}


/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 */


void GameEngine::Run() {

    DrawContext screenContext(mWindow, mFont); // gives the game objects drawing access
    DrawContext guiContext(mWindow, mFont);

    GameContext context;

    context.mEngineView = this; // give rhe context access to this engine
    context.ScreenContext = &screenContext; // give the context access to this drawing helper
    context.GUIContext = &guiContext;

    // give game objects access to the notification manager
    context.mNotificationManager = &mNotificationManager;

    context.currentFrame = 0;

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
        
        std::vector<std::shared_ptr<GameObject>> newObjects;
        newObjects.swap(mNewGameObjects);

        for (const auto& gameObject : newObjects) {
            mGameObjects.push_back(gameObject);

            // set the current object before initializing it
            context.CurrObject = gameObject;

            gameObject->Initialize(&context);
        }

        // 2. Process events

        // Note: use -> because you're accessing a shared_ptr, not the object itself (use '.' in that case)
        // ask the window for the next event and store the result in event
        while (const std::optional<sf::Event> event = mWindow->pollEvent()) {
            // checks if the event received is a closed event (closing window)
            if (event->is<sf::Event::Closed>()) {
                mWindow->close();
            }

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                // find notifications registered for the pressed key
                int key = static_cast<int>(keyPressed->scancode);
                auto it = mKeyDownNotifications.find(key);

                if (it != mKeyDownNotifications.end()) {
                    // send each registered notification
                    for (const auto& message : it->second) {
                        mNotificationManager.Notify(message);
                    }
                }
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                // find notifications registered for the released key
                int key = static_cast<int>(keyReleased->scancode);
                auto it = mKeyUpNotifications.find(key);

                if (it != mKeyUpNotifications.end()) {
                    // send each registered notification
                    for (const auto& message : it->second) {
                        mNotificationManager.Notify(message);
                    }
                }
            }

        }

        // 3. Update game objects
        // upate every game object in mGameObjects
        for(const auto& gameObject : mGameObjects) {
            context.CurrObject = gameObject;
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
                Rect boundsA = objA->GetBounds();
                Rect boundsB = objB->GetBounds();
                
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
            context.CurrObject = gameObject;
            gameObject -> LateUpdate(&context);
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (const auto& gameObject : mGameObjects) {
            // check if this gameObject is a GraphicsObject since only they should be rendered
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(gameObject);

            if (graphicsObj != nullptr) {
                context.CurrObject = gameObject;

                // set the objects position and rotation
                screenContext.SetContextOffset(graphicsObj->GetLocation());
                screenContext.SetContextRotation(graphicsObj->GetRotation());


                // draw background
                graphicsObj -> RenderBackground(&context);
            }
        }

        // 7. Render foreground
        for(const auto& gameObject : mGameObjects) {
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(gameObject);

            if (graphicsObj != nullptr) {
                context.CurrObject = gameObject;

                // set the objects position and rotation
                screenContext.SetContextOffset(graphicsObj->GetLocation());
                screenContext.SetContextRotation(graphicsObj->GetRotation());

                //draw the foreground
                graphicsObj -> RenderForeground(&context);
            }
        }


        // render GUI
        for (const auto& gameObject : mGameObjects) {
            context.CurrObject = gameObject;
            // GUI is drawn without object transformations
            guiContext.SetContextOffset({0, 0});
            guiContext.SetContextRotation(0.0f);

            gameObject->RenderUI(&context);
        }

        context.CurrObject.reset();
        
        // Actually render to window
        mWindow->display();

        // increases the frame counter once per completed frame
        context.currentFrame++;
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
