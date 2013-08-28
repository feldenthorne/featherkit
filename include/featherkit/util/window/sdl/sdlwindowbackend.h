#pragma once
#define NO_SDL_GLEXT
#include <featherkit/window/windowbackend.h>
#include <glm/glm.hpp>
#include <SDL.h>
#include <SDL/SDL_opengl.h>
#include <memory>

namespace fea
{
    namespace util
    {
        class SDLWindowBackend : public WindowBackend
        {
            public:
                SDLWindowBackend();

                void create(VideoMode mode, const std::string& title, uint32_t style = Style::Default, const ContextSettings& settings = ContextSettings()) override;
                void close() override;
                bool isOpen() const override;
                const ContextSettings getSettings() const override;
                Vec2I getPosition() const override;
                void setPosition(int32_t x, int32_t y) override;
                Vec2I getSize() const override;
                void setSize(int32_t w, int32_t h) override;
                void setTitle(const std::string& title) override;
                void setIcon(uint32_t width, uint32_t height, const uint8_t* pixels) override;
                void setVisible(bool visible) override;
                void setVSyncEnabled(bool enabled) override;
                void setMouseCursorVisible(bool visible) override;
                void setFramerateLimit(uint32_t limit) override;

                bool setRenderingActive(bool active = true) const override;
                void swapBuffers() override;
                ~SDLWindowBackend();
            private:
                SDL_Surface* window;
        };
    }
}
