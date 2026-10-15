
#include "NotificationManager.h"
#include <algorithm>

namespace CMPUT350 {

// register an object to receive a notification
void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key) {
    // add the object to the listeners for this key
    mListeners[key].push_back(object);
}

// unregister an object from a notification
void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key) {
    // find the listeners for this key
    auto it = mListeners.find(key);

    if (it == mListeners.end()) {
        return;
    }

    auto& listeners = it->second;

    // remove the object and any expired listeners
    listeners.erase(
        std::remove_if(listeners.begin(), listeners.end(),
            [&object](const std::weak_ptr<GameObject>& listener) {
                return listener.expired() ||
                    (!listener.owner_before(object) && !object.owner_before(listener));
            }),
        listeners.end()
    );
}

// send a notification to registered objects
void NotificationManager::Notify(const std::string& message) {
    // find the listeners for this message
    auto it = mListeners.find(message);

    if (it == mListeners.end()) {
        return;
    }

    // copy the list so callbacks can change registrations safely
    auto listeners = it->second;

    // notify objects that are still alive
    for (const auto& listener : listeners) {
        if (auto gameObject = listener.lock()) {
            if (gameObject->IsAlive()) {
                gameObject->ReceiveNotification(message);
            }
        }
    }

    // remove expired listeners from the original list
    auto current = mListeners.find(message);

    if (current != mListeners.end()) {
        auto& registered = current->second;

        registered.erase(
            std::remove_if(registered.begin(), registered.end(),
                [](const std::weak_ptr<GameObject>& listener) {
                    return listener.expired();
                }),
            registered.end()
        );
    }
}

}  // namespace CMPUT350
