#include "InputManager.h"
#include "Input_Lite.h"
#include <functional>

namespace SL {
namespace Input_Lite {

    InputManager::~InputManager()
    {
        KeepRunning = false;
        Ready = true;
        Conditional.notify_all();
    }

    bool InputManager::PushEvent(const KeyEvent &e)
    {
        if (e.Key == KeyCodes::INVALID || !OnKeyEvent) {
            return false;
        } // key not regonized
        auto tmp = [&, e] { OnKeyEvent(e); };
        std::lock_guard<std::mutex> lk(Mutex);
        Events.emplace_back(tmp);
        Ready = true;
        return true;
    }
    bool InputManager::PushEvent(const MouseButtonEvent &e)
    {
        if (!OnMouseButtonEvent) {
            return false;
        } // key not regonized
        auto tmp = [&, e] { OnMouseButtonEvent(e); };
        std::lock_guard<std::mutex> lk(Mutex);
        Events.emplace_back(tmp);
        Ready = true;
        return true;
    }

    bool InputManager::PushEvent(const MouseScrollEvent &pos)
    {
        if (!OnMouseScroll) {
            return false;
        } // key not regonized
        auto tmp = [&, pos] { OnMouseScroll(pos); };
        std::lock_guard<std::mutex> lk(Mutex);
        Events.emplace_back(tmp);
        Ready = true;
        return true;
    }
    bool InputManager::PushEvent(const MousePositionOffsetEvent &pos)
    {
        if (!OnMousePositionOffset) {
            return false;
        } // key not regonized
        auto tmp = [&, pos] { OnMousePositionOffset(pos); };
        std::lock_guard<std::mutex> lk(Mutex);
        Events.emplace_back(tmp);
        Ready = true;
        return true;
    }
    bool InputManager::PushEvent(const MousePositionAbsoluteEvent &pos)
    {
        if (!OnMousePositionAbsolute) {
            return false;
        } // key not regonized
        auto tmp = [&, pos] { OnMousePositionAbsolute(pos); };
        std::lock_guard<std::mutex> lk(Mutex);
        Events.emplace_back(tmp);
        Ready = true;
        return true;
    }
    void InputManager::Run()
    {
        while (KeepRunning) {
            std::unique_lock<std::mutex> lk(Mutex);
            Conditional.wait(lk, [&] { return Ready; });
            if (KeepRunning) { // check again.. thread could have been work up for destruction
                for (auto &f : Events) {
                    f();
                }
                Events.clear();
            }
        }
    }

} // namespace Input_Lite
} // namespace SL