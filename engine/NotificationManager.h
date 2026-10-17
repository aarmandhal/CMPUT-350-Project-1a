
#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "GameObject.h"

namespace CMPUT350 {

class NotificationManager {
public:
    // register an object to receive a notification
    void Register(std::weak_ptr<GameObject> object, const std::string& key);

    // stop an object from receiving a notification
    void Unregister(std::weak_ptr<GameObject> object, const std::string& key);

    // send a notification to all registered objects
    void Notify(const std::string& message);

private:
    // store the listeners for each notification
    std::unordered_map<std::string, std::vector<std::weak_ptr<GameObject>>> mListeners;
};

}  // namespace CMPUT350

#endif  // NOTIFICATIONMANAGER_H
