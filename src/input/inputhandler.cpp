#include <featherkit/input/inputbackend.h>
#include <featherkit/input/inputhandler.h>

namespace fea
{
    InputHandler::InputHandler(InputBackend* backend) : mInputBackend(backend)
    {
    }

    void InputHandler::processEvents(bool keepLast)
    {
        std::queue<Event> newEvents = mInputBackend->fetchEvents();

        if(keepLast)
        {
            while(newEvents.size() > 0)
                mEventQueue.push(newEvents.front());
                newEvents.pop();
        }
        else
        {
            mEventQueue = newEvents;
        }
    }
    
    bool InputHandler::pollEvent(Event& event)
    {
        if(mEventQueue.size() > 0)
        {
            event = mEventQueue.front();
            mEventQueue.pop();
            return true;
        }
        else 
        {
            return false;
        }
    }
    
    const std::queue<Event>& InputHandler::getEventQueue() const
    {
        return mEventQueue;
    }
    
    bool InputHandler::isKeyPressed(Keyboard::Code code) const
    {
        return mInputBackend->isKeyPressed(code);
    }
    
    bool InputHandler::isMouseButtonPressed(Mouse::Button b) const
    {
        return mInputBackend->isMouseButtonPressed(b);
    }
    
    glm::ivec2 InputHandler::getMouseGlobalPosition() const
    {
        return mInputBackend->getMouseGlobalPosition();
    }

    glm::ivec2 InputHandler::getMouseWindowPosition() const
    {
        return mInputBackend->getMouseWindowPosition();
    }
    
    void InputHandler::setMouseGlobalPosition(int32_t x, int32_t y)
    {
        mInputBackend->setMouseGlobalPosition(x, y);
    }
    
    void InputHandler::setMouseWindowPosition(int32_t x, int32_t y)
    {
        mInputBackend->setMouseWindowPosition(x, y);
    }

    bool InputHandler::isGamepadConnected(uint32_t id) const
    {
        return mInputBackend->isGamepadConnected(id);
    }

    uint32_t InputHandler::getGamepadButtonCount(uint32_t id) const
    {
        return mInputBackend->getGamepadButtonCount(id);
    }

    bool InputHandler::isGamepadButtonPressed(uint32_t id, uint32_t button) const
    {
        return mInputBackend->isGamepadButtonPressed(id, button);
    }

    bool InputHandler::gamepadHasAxis(uint32_t id, Gamepad::Axis axis) const
    {
        return mInputBackend->gamepadHasAxis(id, axis);
    }

    float InputHandler::getGamepadAxisPosition(uint32_t id, Gamepad::Axis axis) const
    {
        return mInputBackend->getGamepadAxisPosition(id, axis);
    }

    void InputHandler::setGamepadThreshold(float threshold)
    {
        mInputBackend->setGamepadThreshold(threshold);
    }

    void InputHandler::setKeyRepeatEnabled(bool enabled)
    {
        mInputBackend->setKeyRepeatEnabled(enabled);
    }
}
