#include <fea/rendering/tilemap.hpp>
#include <fea/assert.hpp>

namespace fea
{
    const int32_t tileMapChunkSize = 32;

    TileDefinition::TileDefinition(const glm::ivec2& texPos, TileId next, int32_t ticks):
        tileTexPosition(texPos),
        nextTile(next),
        ticksUntilChange(ticks)
    {
    }

    TileMap::TileChunk::TileChunk():
        tileCount(0),
        tilesSet(tileMapChunkSize*tileMapChunkSize, false)
    {
        tiles.resize(tileMapChunkSize*tileMapChunkSize);
    }

    void TileMap::TileChunk::setTile(const glm::ivec2& position, TileId id)
    {
        size_t index = position.x + position.y * tileMapChunkSize;
        tiles[index].id = id;
        tiles[index].color = fea::Color::White;

        if(!tilesSet[index])
        {
            tilesSet[index] = true;
            tileCount++;
        };
    }

    void TileMap::TileChunk::setTileColor(const glm::ivec2& position, const fea::Color& color)
    {
        size_t index = position.x + position.y * tileMapChunkSize;

        //ensure tile is set
        tiles[index].color = color;
    }

    const TileMap::Tile& TileMap::TileChunk::getTile(const glm::ivec2& position) const
    {
        size_t index = position.x + position.y * tileMapChunkSize;

        //ensure tile is set
        return tiles[index];
    }

    TileMap::Tile& TileMap::TileChunk::getTile(const glm::ivec2& position)
    {
        size_t index = position.x + position.y * tileMapChunkSize;

        //ensure tile is set
        return tiles[index];
    }

    void TileMap::TileChunk::unsetTile(const glm::ivec2& position)
    {
        size_t index = position.x + position.y * tileMapChunkSize;

        if(tilesSet[index])
        {
            tilesSet[index] = false;
            tileCount--;
        };
    }

    TileMap::TileMap(const glm::ivec2& tileSize, const glm::ivec2& tileTextureSize):
        mTileSize(tileSize),
        mTileTextureSize(tileTextureSize)
    {
    }

    void TileMap::setTexture(const Texture& texture)
    {
        mTexture = &texture;
        mChunkCache.clear();
    }

    const Texture& TileMap::getTexture() const
    {
        return *mTexture;
    }

    void TileMap::addTileDefinition(TileId id, const TileDefinition& tileDef)
    {
        mTileDefinitions.emplace(id, tileDef);
    }

    void TileMap::setTile(const glm::ivec2& pos, TileId id)
    {
        const glm::ivec2 chunkCoord = tileToChunk(pos);
        const glm::ivec2 tileInChunkCoord = tileToTileInChunk(pos);

        auto& chunk = mChunks[chunkCoord];

        chunk.setTile(tileInChunkCoord, id);

        const auto& definition = mTileDefinitions.at(id);
        if(definition.nextTile != -1)
        {
            auto& tile = mAnimatedTiles[pos];
            tile = &chunk.getTile(tileInChunkCoord);
            tile->ticksUntilChange = definition.ticksUntilChange;
        }
        else
        {
            mAnimatedTiles.erase(pos);
        }

        setDirty(chunkCoord);
    }

    void TileMap::unsetTile(const glm::ivec2& pos)
    {
        const glm::ivec2 chunkCoord = tileToChunk(pos);
        const glm::ivec2 tileInChunkCoord = tileToTileInChunk(pos);

        auto& chunk = mChunks[chunkCoord];

        chunk.unsetTile(tileInChunkCoord);

        if(chunk.tileCount == 0)
        {
            mChunks.erase(chunkCoord);
        }

        auto animatedIterator = mAnimatedTiles.find(pos);
        mAnimatedTiles.erase(animatedIterator);
        setDirty(chunkCoord);
    }

    void TileMap::fillRegion(glm::ivec2 startCorner, glm::ivec2 endCorner, TileId id)
    {
        if(startCorner.x > endCorner.x)
            std::swap(startCorner.x, endCorner.x);
        if(startCorner.y > endCorner.y)
            std::swap(startCorner.y, endCorner.y);
            
        for(int32_t x = startCorner.x; x <= endCorner.x; ++x)
        {
            for(int32_t y = startCorner.y; y <= endCorner.y; ++y)
            {
                setTile({x, y}, id);
            }
        }
    }

    void TileMap::clear()
    {
        mChunks.clear();
        mAnimatedTiles.clear();
        mChunkCache.clear();
    }

    void TileMap::setTileColor(const glm::ivec2& pos, const fea::Color& color)
    {
        //assert that tile exists
        const glm::ivec2 chunkCoord = tileToChunk(pos);
        const glm::ivec2 tileInChunkCoord = tileToTileInChunk(pos);

        auto& chunk = mChunks[chunkCoord];

        chunk.setTileColor(tileInChunkCoord, color);
        setDirty(chunkCoord);
    }

    const TileMap::Tile& TileMap::getTile(const glm::ivec2& pos) const
    {
        //assert that tile exists
        const glm::ivec2 chunkCoord = tileToChunk(pos);
        const glm::ivec2 tileInChunkCoord = tileToTileInChunk(pos);

        auto& chunk = mChunks.at(chunkCoord);

        return chunk.getTile(tileInChunkCoord);
    }
    
    glm::ivec2 TileMap::worldToTileCoordinates(const glm::vec2& coordinate) const
    {
        float sin = glm::sin(-getRotation());
        float cos = glm::cos(-getRotation());

        glm::mat2x2 rot = glm::mat2x2(cos, sin, -sin, cos);
        glm::vec2 scale = getScale();

        glm::vec2 transformedCoordinate = ((glm::inverse(rot) * (coordinate - mPosition)) + getOrigin()) / scale;

        return static_cast<glm::ivec2>(transformedCoordinate) / mTileSize;
    }

    const glm::ivec2& TileMap::getTileSize() const
    {
        return mTileSize;
    }

    const glm::ivec2& TileMap::getTileTextureSize() const
    {
        return mTileTextureSize;
    }

    void TileMap::tick()
    {
        std::unordered_map<glm::ivec2, TileId> tilesToSet;

        for(auto tileIter : mAnimatedTiles)
        {
            if(--tileIter.second->ticksUntilChange == 0)
            {
                TileId nextTile = mTileDefinitions.at(tileIter.second->id).nextTile;
                tilesToSet.emplace(tileIter.first, nextTile);
            }
        }

        for(auto tileIter : tilesToSet)
        {
            setTile(tileIter.first, tileIter.second);
        }
    }
            
    std::vector<RenderEntity> TileMap::getRenderInfo() const
    {
        std::vector<RenderEntity> renderInfo;

        for(auto chunk : mChunks)
        {
            renderInfo.push_back(renderInfoFromChunk(chunk.first, chunk.second));
        }

        return renderInfo;
    }

    glm::ivec2 TileMap::tileToChunk(const glm::ivec2& pos) const
    {
        return glm::ivec2(pos.x >= 0 ? pos.x / tileMapChunkSize : (pos.x+1) / tileMapChunkSize - 1, pos.y >= 0 ? pos.y / tileMapChunkSize : (pos.y+1) / tileMapChunkSize - 1); //takes <0 into account
    }
    
    glm::ivec2 TileMap::tileToTileInChunk(const glm::ivec2& pos) const
    {
        return glm::ivec2(pos.x >= 0 ? pos.x % tileMapChunkSize : pos.x % tileMapChunkSize + tileMapChunkSize, pos.y >= 0 ? pos.y % tileMapChunkSize : pos.y % tileMapChunkSize + tileMapChunkSize); //takes <0 into account
    }

    RenderEntity TileMap::renderInfoFromChunk(const glm::ivec2& chunkPos, const TileChunk& chunk) const
    {
        glm::vec2 worldPos = static_cast<glm::vec2>(chunkPos * tileMapChunkSize * mTileSize);

        size_t index = 0;

        std::vector<float> vertices;
        std::vector<float> texCoords;
        std::vector<float> vertexColors;
        glm::vec2 tilePos;


        if(isDirty(chunkPos))
        {
            for(int32_t y = 0; y < tileMapChunkSize; ++y)
            {
                tilePos.y = worldPos.y + y * mTileSize.y;
                for(int32_t x = 0; x < tileMapChunkSize; ++x)
                {
                    if(chunk.tilesSet[index])
                    {
                        TileId tileId = chunk.tiles[index].id;
                        const fea::Color& tileColor = chunk.tiles[index].color;

                        FEA_ASSERT(mTileDefinitions.count(tileId) != 0, "Set tile does not exist. This is a bug in feather kit");
                        tilePos.x = worldPos.x + x * mTileSize.x;

                        vertices.push_back(tilePos.x              ); vertices.push_back(tilePos.y              );
                        vertices.push_back(tilePos.x              ); vertices.push_back(tilePos.y + mTileSize.y);
                        vertices.push_back(tilePos.x + mTileSize.x); vertices.push_back(tilePos.y              );
                        vertices.push_back(tilePos.x + mTileSize.x); vertices.push_back(tilePos.y              );
                        vertices.push_back(tilePos.x              ); vertices.push_back(tilePos.y + mTileSize.y);
                        vertices.push_back(tilePos.x + mTileSize.x); vertices.push_back(tilePos.y + mTileSize.y);

                        glm::vec2 startTexCoords;
                        glm::vec2 endTexCoords = {1.0f, 1.0f};


                        if(mTexture)
                        {
                            startTexCoords = static_cast<glm::vec2>(mTileDefinitions.at(tileId).tileTexPosition * mTileTextureSize) / static_cast<glm::vec2>(mTexture->getSize());
                            endTexCoords = startTexCoords + static_cast<glm::vec2>(mTileTextureSize) / static_cast<glm::vec2>(mTexture->getSize());
                        }

                        texCoords.push_back(startTexCoords.x); texCoords.push_back(startTexCoords.y);
                        texCoords.push_back(startTexCoords.x); texCoords.push_back(endTexCoords.y);
                        texCoords.push_back(endTexCoords.x); texCoords.push_back(startTexCoords.y);
                        texCoords.push_back(endTexCoords.x); texCoords.push_back(startTexCoords.y);
                        texCoords.push_back(startTexCoords.x); texCoords.push_back(endTexCoords.y);
                        texCoords.push_back(endTexCoords.x); texCoords.push_back(endTexCoords.y);

                        float r = tileColor.rAsFloat();
                        float g = tileColor.gAsFloat();
                        float b = tileColor.bAsFloat();
                        float a = tileColor.aAsFloat();

                        vertexColors.push_back(r); vertexColors.push_back(g); vertexColors.push_back(b); vertexColors.push_back(a);
                        vertexColors.push_back(r); vertexColors.push_back(g); vertexColors.push_back(b); vertexColors.push_back(a);
                        vertexColors.push_back(r); vertexColors.push_back(g); vertexColors.push_back(b); vertexColors.push_back(a);
                        vertexColors.push_back(r); vertexColors.push_back(g); vertexColors.push_back(b); vertexColors.push_back(a);
                        vertexColors.push_back(r); vertexColors.push_back(g); vertexColors.push_back(b); vertexColors.push_back(a);
                        vertexColors.push_back(r); vertexColors.push_back(g); vertexColors.push_back(b); vertexColors.push_back(a);
                    }

                    ++index;
                }
            }

            mChunkCache[chunkPos] = { vertices, texCoords, vertexColors };
            std::cout << chunkPos.x << " " << chunkPos.y << "\n";
            std::cout << "cached a thing\n";
        }
        else
        {
            const auto& cacheEntry = mChunkCache[chunkPos];

            vertices = cacheEntry.vertices;
            texCoords = cacheEntry.texCoords;
            vertexColors = cacheEntry.colors;

            std::cout << chunkPos.x << " " << chunkPos.y << "\n";
            std::cout << "retreived a cached thing\n";
        }

        RenderEntity renderEntity;
        renderEntity.mDrawMode = GL_TRIANGLES;
        renderEntity.mElementAmount = vertices.size() / 2;

        if(vertices.size() > 0)
        {
            renderEntity.mVertexAttributes.push_back(VertexAttribute("vertex", 2, std::move(vertices)));
            renderEntity.mVertexAttributes.push_back(VertexAttribute("texCoords", 2, std::move(texCoords)));
            renderEntity.mVertexAttributes.push_back(VertexAttribute("colors", 4, std::move(vertexColors)));
        }

        if(mTexture != nullptr)
        {
            renderEntity.mUniforms.push_back(Uniform("texture", TEXTURE, getTexture().getId()));
        }

        renderEntity.mUniforms.push_back(Uniform("position", VEC2, mPosition));
        renderEntity.mUniforms.push_back(Uniform("origin", VEC2, mOrigin));
        renderEntity.mUniforms.push_back(Uniform("rotation", FLOAT, mRotation));
        renderEntity.mUniforms.push_back(Uniform("scaling", VEC2, mScaling));
        renderEntity.mUniforms.push_back(Uniform("parallax", VEC2, mParallax));
        renderEntity.mUniforms.push_back(Uniform("constraints", VEC4, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
        glm::vec3 colorInfo = glm::vec3(mColor.rAsFloat(), mColor.gAsFloat(), mColor.bAsFloat());
        renderEntity.mUniforms.push_back(Uniform("color", VEC3, colorInfo));
        float opacity = mColor.aAsFloat();
        renderEntity.mUniforms.push_back(Uniform("opacity", FLOAT, opacity));

        for(const auto& uniform : mExtraUniforms)
            renderEntity.mUniforms.push_back(uniform.second);

        return renderEntity;
    }

    void TileMap::setDirty(const glm::ivec2& chunk) const
    {
        mChunkCache.erase(chunk);
    }

    bool TileMap::isDirty(const glm::ivec2& chunk) const
    {
        return mChunkCache.count(chunk) == 0;
    }
}
