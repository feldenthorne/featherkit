#include <framework/rendering/viewport.h>

namespace windbreeze
{
    void Viewport::setSize(const glm::uvec2& s)
    {
        size = s;
    }

    glm::uvec2& Viewport::getSize()
    {
        return size;
    }

    void Viewport::setCamera(const Camera& cam)
    {
        camera = cam;
    }
    
    Camera& Viewport::getCamera()
    {
        return camera;
    }
    
    glm::vec2 Viewport::transformPoint(const glm::vec2 point)
    {
        glm::vec2 halfViewSize = ((glm::vec2) size) * 0.5f;
        return glm::inverse(camera.getTransformation()) * (camera.getZoom() * (point - camera.getPosition())) + halfViewSize;
    }
    
    glm::vec2 Viewport::untransformPoint(const glm::vec2 point)
    {
        glm::vec2 halfViewSize = ((glm::vec2) size) * 0.5f;
        return (1.0f/camera.getZoom()) * (camera.getTransformation() * (point - halfViewSize)) + camera.getPosition();
    }
}