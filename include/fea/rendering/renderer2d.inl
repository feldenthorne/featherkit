template <typename Drawable>
void Renderer2D::render(const Drawable& drawable)
{
    render(drawable, mDefaultShader);
}

template <typename Drawable>
void Renderer2D::render(const Drawable& drawable, const RenderTarget& target)
{
    render(drawable, target, mDefaultShader);
}

template <typename Drawable>
void Renderer2D::render(const Drawable& drawable, Shader& shader)
{
    glBindVertexArray(mVertexArray);
    shader.activate();

    //can be cached
    shader.setUniform("camPosition", Uniform(VEC2, mCurrentViewport.getCamera().getPosition()));
    shader.setUniform("camZoom", Uniform(VEC2, mCurrentViewport.getCamera().getZoom()));
    glm::mat2x2 camRot = mCurrentViewport.getCamera().getRotationMatrix();
    shader.setUniform("camRotation", Uniform(MAT2X2, camRot));
    glm::vec2 halfViewSize = glm::vec2((float)mCurrentViewport.getSize().x / 2.0f, (float)mCurrentViewport.getSize().y / 2.0f);
    shader.setUniform("halfViewSize", Uniform(VEC2, halfViewSize));
    shader.setUniform("projection", Uniform(MAT4X4, mProjection));

    GLuint defaultTextureId = mDefaultTexture.getId();

    for(const auto& renderOperation : drawable.getRenderInfo())
    {
        shader.setUniform("texture", Uniform(TEXTURE, Uniform::TextureData{defaultTextureId, 0})); //may be overriden
        setBlendModeGl(mCurrentBlendMode); //can be cached

        for(const auto& uniformIter : renderOperation.mUniforms)
        {
            shader.setUniform(uniformIter.first, uniformIter.second);
        }
        
        for(const auto& vertexAttributeIter : renderOperation.mVertexAttributes)
        {
            shader.setVertexAttribute(vertexAttributeIter.first, vertexAttributeIter.second.mAttributeFloatAmount, vertexAttributeIter.second.mData.data(), static_cast<int32_t>(vertexAttributeIter.second.mData.size()));
        }

        if(renderOperation.mIndices.empty())
        {
            glDrawArrays(renderOperation.mDrawMode, 0, static_cast<GLsizei>(renderOperation.mElementAmount));
        }
        else
        {
            glDrawElements(renderOperation.mDrawMode, static_cast<GLsizei>(renderOperation.mElementAmount), GL_UNSIGNED_SHORT, renderOperation.mIndices.data());
        }
    }

    setBlendMode(ALPHA);  //needed?
    shader.deactivate();
    glBindVertexArray(0);
}

template <typename Drawable>
void Renderer2D::render(const Drawable& drawable, const RenderTarget& target, Shader& shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, target.getId());
    render(drawable, shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
