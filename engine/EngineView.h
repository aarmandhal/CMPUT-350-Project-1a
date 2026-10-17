#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>
#include <vector>
#include <string> 

#include "GameObject.h"

namespace CMPUT350 {

class GameObject;

class EngineView {
    // used to store game objects
    using tGameObject = std::vector<std::shared_ptr<GameObject>>;

public:
    // add a new object to the game engine
    virtual void AddGameObject(std::shared_ptr<GameObject> gameObject) = 0;

    // notifications for key presses and releases
    virtual void InstallKeyDownNotification(int key, const std::string& message) = 0;
    virtual void InstallKeyUpNotification(int key, const std::string& message) = 0;

    // iterator for accessing game objects
    using const_iterator = tGameObject::const_iterator;

    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;

};

}  // namespace CMPUT350

#endif
