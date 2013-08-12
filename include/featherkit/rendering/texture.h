#pragma once
#include <stdint.h>

namespace fea
{
    class Renderer2DBackend;

    class Texture
    {
        public:
            Texture();
            Texture(Renderer2DBackend& backend, int32_t i);
            Texture(Texture&& other);
            Texture(const Texture& other) = delete;
            Texture& operator=(Texture&& other);
            int32_t getId() const;
            ~Texture();
        private:
            Renderer2DBackend* creator;
            int32_t id;
    };
    /** @addtogroup Render2D
     *@{
     *  @class Texture
     *@}
     ***
     *  @class Texture
     *  @brief Container class for a texture resource that can be used by Drawable2D instances.
     *
     *  Textures are tied to the rendering system and cannot be created properly on its own. Textures created using the standard constructor will not be valid. To properly create a Texture, use the Renderer2D::createTexture function. Because Textures represents internal textures of the rendering system, they cannot be copied. 
     *
     *  A Texture instance has to be kept alive as long as there are drawables using it, otherwise they will try to render with an invalid Texture.
     ***
     *  @fn Texture::Texture()
     *  @brief Construct an uninitialised Texture.
     *
     *  Textures created this way need to be set to a valid Texture returned by the Renderer2D::createTexture function.
     ***
     *  @fn Texture::Texture(Renderer2DBackend& backend, int32_t i)
     *  @brief Construct a Texture.
     *
     *  This constructor is used internally by the Renderer2D when the Renderer2D::createTexture function is called and is not meant to be used directly.
     *  @param backend Backend to store a reference to.
     *  @param i Id of the new Texture.
     ***
     *  @fn Texture::Texture(Texture&& other)
     *  @brief Move constructor.
     *  @param other Other instance.
     ***
     *  @fn Texture::Texture(const Texture& other) = delete
     *  @brief Deleted copy constructor.
     *  @param other Instance to copy.
     ***
     *  @fn Texture& Texture::operator=(Texture&& other)
     *  @brief Move assignment operator.
     *  @param other Other instance.
     ***
     *  @fn int32_t Texture::getId() const
     *  @brief Get the ID of the internal texture.
     ***
     *  @fn Texture::~Texture()
     *  @brief Destruct a texture.
     **/
}
