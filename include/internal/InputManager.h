#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "Input_Lite.h"

namespace SL {
namespace Input_Lite {

    class InputManager final : public IInputManager {
        std::thread Thread;
        std::mutex Mutex;
        std::condition_variable Conditional;
        std::vector<std::function<void()>> Events;
        bool Ready = false;
        bool KeepRunning = false;

        void Run();

      public:
        virtual ~InputManager();

        virtual bool PushEvent(const KeyEvent &e) override;
        virtual bool PushEvent(const MouseButtonEvent &e) override;

        virtual bool PushEvent(const MouseScrollEvent &pos) override;
        virtual bool PushEvent(const MousePositionOffsetEvent &pos) override;
        virtual bool PushEvent(const MousePositionAbsoluteEvent &pos) override;

        std::function<void(const KeyEvent &)> OnKeyEvent;
        std::function<void(const MouseButtonEvent &)> OnMouseButtonEvent;
        std::function<void(const MouseScrollEvent &)> OnMouseScroll;
        std::function<void(const MousePositionOffsetEvent &)> OnMousePositionOffset;
        std::function<void(const MousePositionAbsoluteEvent &)> OnMousePositionAbsolute;
        void start() { Thread = std::thread(&SL::Input_Lite::InputManager::Run, this); }
    };

} // namespace Input_Lite
} // namespace SL
