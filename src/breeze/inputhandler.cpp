#include "inputbackend.h"
#include "inputhandler.h"

namespace windbreeze
{
    void InputHandler::processEvents(bool keepLast)
    {
        std::queue<Event> newEvents = inputBackend.fetchEvents();

        if(keepLast)
        {
            while(newEvents.size() > 0)
                eventQueue.push(newEvents.front());
                newEvents.pop();
        }
        else
        {
            eventQueue = newEvents;
        }
    }
    
    bool InputHandler::pollEvent(Event& event)
    {
        if(eventQueue.size() > 0)
        {
            event = eventQueue.front();
            eventQueue.pop();
            return true;
        }
        else 
        {
            return false;
        }
    }
    
    const std::queue<Event>& InputHandler::getEventQueue() const
    {
        return eventQueue;
    }
    
    bool InputHandler::isKeyPressed(Keyboard::Code code)
    {
        return inputBackend.isKeyPressed(code);
    }
    
    bool InputHandler::isMouseButtonPressed(Mouse::Button b)
    {
        return inputBackend.isMouseButtonPressed(b);
    }
    
    IntVec2 InputHandler::getMouseGlobalPosition()
    {
        return inputBackend.getMouseGlobalPosition();
    }

    IntVec2 InputHandler::getMouseWindowPosition()
    {
        return inputBackend.getMouseWindowPosition();
    }
    
    void InputHandler::setMouseGlobalPosition(int32_t x, int32_t y)
    {
        inputBackend.setMouseGlobalPosition(x, y);
    }
    
    void InputHandler::setMouseWindowPosition(int32_t x, int32_t y)
    {
        inputBackend.setMouseWindowPosition(x, y);
    }

    bool InputHandler::isGamepadConnected(uint32_t id)
    {
        return inputBackend.isGamepadConnected(id);
    }

    uint32_t InputHandler::getGamepadButtonCount(uint32_t id)
    {
        return inputBackend.getGamepadButtonCount(id);
    }

    bool InputHandler::isGamepadButtonPressed(uint32_t id, uint32_t button)
    {
        return inputBackend.isGamepadButtonPressed(id, button);
    }

    bool InputHandler::gamepadHasAxis(uint32_t id, Gamepad::Axis axis)
    {
        return inputBackend.gamepadHasAxis(id, axis);
    }

    float InputHandler::getGamepadAxisPosition(uint32_t id, Gamepad::Axis axis)
    {
        return inputBackend.getGamepadAxisPosition(id, axis);
    }

    void InputHandler::setGamepadThreshold(float threshold)
    {
        inputBackend.setGamepadThreshold(threshold);
    }

    void InputHandler::setKeyRepeatEnabled(bool enabled)
    {
        inputBackend.setKeyRepeatEnabled(enabled);
    }
}
