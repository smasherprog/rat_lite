#include "InputManager.h"
#include "Input_Lite.h"
#include <assert.h>

namespace SL {
namespace Input_Lite {

    class InputConfiguration : public IInputConfiguration {
        std::shared_ptr<InputManager> Impl;

      public:
        InputConfiguration(const std::shared_ptr<InputManager> &impl) : Impl(impl) {}
        virtual ~InputConfiguration() {}

        virtual std::shared_ptr<IInputConfiguration> onEvent(const std::function<void(const KeyEvent &)> &cb) override
        {
            assert(!Impl->OnKeyEvent);
            Impl->OnKeyEvent = cb;
            return std::make_shared<InputConfiguration>(Impl);
        }
        virtual std::shared_ptr<IInputConfiguration> onEvent(const std::function<void(const MouseButtonEvent &)> &cb) override
        {
            assert(!Impl->OnMouseButtonEvent);
            Impl->OnMouseButtonEvent = cb;
            return std::make_shared<InputConfiguration>(Impl);
        }
        virtual std::shared_ptr<IInputConfiguration> onEvent(const std::function<void(const MouseScrollEvent &)> &cb) override
        {
            assert(!Impl->OnMouseScroll);
            Impl->OnMouseScroll = cb;
            return std::make_shared<InputConfiguration>(Impl);
        }
        virtual std::shared_ptr<IInputConfiguration> onEvent(const std::function<void(const MousePositionOffsetEvent &)> &cb) override
        {
            assert(!Impl->OnMousePositionOffset);
            Impl->OnMousePositionOffset = cb;
            return std::make_shared<InputConfiguration>(Impl);
        }

        virtual std::shared_ptr<IInputConfiguration> onEvent(const std::function<void(const MousePositionAbsoluteEvent &)> &cb) override
        {
            assert(!Impl->OnMousePositionAbsolute);
            Impl->OnMousePositionAbsolute = cb;
            return std::make_shared<InputConfiguration>(Impl);
        }
        virtual std::shared_ptr<IInputManager> Build() override { return Impl; }
    };
    std::shared_ptr<IInputConfiguration> CreateInputConfiguration() { return std::make_shared<InputConfiguration>(std::make_shared<InputManager>()); }
} // namespace Input_Lite
} // namespace SL