#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

namespace CMPUT350 {

class GameContext;

class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void Initialize(GameContext *context);
    virtual void Update(GameContext *context);
    virtual void LateUpdate(GameContext *context);
    virtual void RenderUI(GameContext *context);

    // receive a notification from the notification manager
    virtual void ReceiveNotification(const std::string& key);
    
    virtual bool IsAlive() const;
    virtual void Kill();

};

}  // namespace CMPUT350

#endif  // GAMEOBJECT_H
