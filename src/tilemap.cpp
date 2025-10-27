#include <tilemap.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <renderer.h>
#include <glad.h>
#include <Debug.h>

constexpr int MAX_TILES = 100000;

struct TileInstance {
    Vec2 worldPos;
    uint32_t tileIndex;
};

bool Tilemap::LoadTilemap(const char* filename, Shader* shader) {
    tmx::Map map;
    if (!map.load(filename)) {
        std::string msg = "Error: Failed to load tilemap from \"" + std::string(filename) + "\"\n";
        return false;
    }

    float quadVertices[] = {
        1.0f, 0.0f,     1.0f, 0.0f, // top right
        1.0f, 1.0f,     1.0f, 1.0f, // bottom right
        0.0f, 1.0f,     0.0f, 1.0f, // bottom left
        0.0f, 0.0f,     0.0f, 0.0f  // top left
    };

    unsigned int quadIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &tileVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);
    glGenBuffers(1, &tileVBO);

    glBindVertexArray(tileVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_TILES * sizeof(TileInstance), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TileInstance), (void*)0);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(TileInstance), (void*)(offsetof(TileInstance, tileIndex)));
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    this->shader = shader;

    tileSize = Int2((int)map.getTileSize().x, (int)map.getTileSize().y);

    const auto& layers = map.getLayers();
    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Object) {
            const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();
            const auto& mapObjects = objectLayer.getObjects();
            for (const auto& object : mapObjects) {
                uint32_t tileID = object.getTileID();
                if (tileID == 0) continue;

                Int2 position = Int2(object.getPosition()) / tileSize + Int2::down;

                ObjectType objType = stringToObjectType(object.getClass());
                if (objType == ObjectType::Unknown) continue;

                switch (objType) {
                case ObjectType::Box:
                    gameObjects.boxes[position] = Box(static_cast<uint32_t>(objects.size() + 1));
                    break;
                };
                
                objects[position] = ObjectData{
                    position,
                    tileSize,
                    Vec2{ 0, 0 },
                    objType,
                    tileID,
                    object.getRotation(),
                    object.visible()
                };
            }
        }
        else if (layer->getType() == tmx::Layer::Type::Tile) {
            const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();
            if (tileLayer.getClass() == "CollisionMap") {
                for (const auto& chunk : tileLayer.getChunks()) {
                    const auto& tiles = chunk.tiles;
                    for (int cy = 0; cy < chunk.size.y; ++cy) {
                        for (int cx = 0; cx < chunk.size.x; ++cx) {
                            const auto& tile = tiles[(size_t)cx + (size_t)cy * chunk.size.x];
                            if (tile.ID != 0) {
                                int worldX = chunk.position.x + cx;
                                int worldY = chunk.position.y + cy;
                                collisionMap.insert(Int2{ worldX, worldY });
                            }
                        }
                    }
                }
            }
            else {
                std::vector<Chunk> chunks;
                for (auto& chunk : tileLayer.getChunks()) {
                    chunks.push_back(Chunk{
                        Int2(chunk.position.x, chunk.position.y),
                        Int2(chunk.size.x, chunk.size.y),
                        chunk.tiles
                    });
                }
                Vec2 offset = Vec2{
                    (float)tileLayer.getOffset().x,

                    (float)tileLayer.getOffset().y
                };
                this->layers.push_back(ChunkLayer{chunks, offset});
            }
        }
    }

    const auto& tilesets = map.getTilesets();

    if (tilesets.empty()) return false;

    uint32_t maxGID = tilesets.back().getLastGID();
    size_t newSize = static_cast<size_t>(maxGID) + 1;
    tileLookup.resize(newSize, {});

    for (const auto& tileset : tilesets) {
        uint32_t first = tileset.getFirstGID();
        uint32_t count = tileset.getTileCount();

        std::string texPath = tileset.getImagePath();
        Texture texture = LoadTexture(texPath.c_str());

        shader->setInt("texture1", 0);

        tilesetLookup.push_back(TilesetLookup{ tileset, first, first + count, texture });

        for (uint32_t i = 0; i < count; ++i) {
            const tmx::Tileset::Tile* tile = tileset.getTile(i);
            if (tile == nullptr) continue;
            tileLookup[(size_t)first + i - 1] = TileInfo{
                tile,
                tilesetLookup.size() - 1,
                first + i,
                tile->properties
            };
        }
    }

    return true;
}

std::optional<Chunk> Tilemap::GetChunk(Int2 pos, int layer) const {
    for (Chunk chunk : layers[layer].chunks) {
        if (pos / chunk.size == chunk.position) {
            return chunk;
        }
    }
    return std::nullopt;
}

std::optional<tmx::TileLayer::Tile> Tilemap::GetTile(int posX, int posY, int layer) const {
    return GetTile(Int2(posX, posY), layer);
}

std::optional<tmx::TileLayer::Tile> Tilemap::GetTile(Int2 pos, int layer) const {
    std::optional<Chunk> chunk = GetChunk(pos, layer);
    if (!chunk.has_value()) return std::nullopt;

    Int2 tileChunkPos = pos - chunk->position;
    int idx = tileChunkPos.x + tileChunkPos.y * chunk->size.x;
    return chunk->tiles[idx];
}

const TileInfo* Tilemap::GetTileInfo(uint32_t GID) const {
    if (GID == 0) return nullptr;
    if (GID >= tileLookup.size()) return nullptr;
    if (!tileLookup[GID].tile) return nullptr;

    return &tileLookup[GID];
}

bool Tilemap::IsSolid(Int2 pos) const {
    return collisionMap.count(pos) != 0;
}

bool Tilemap::CanPlaceLevel(const Level& level, Int2 position) {
    size_t layerCount = level.layers.size();
    if (layerCount != this->layers.size()) return false;
    if (position % level.layers.at(0).chunks.at(0).size != Int2::zero) return false;

    for (size_t i = 0; i < layerCount; i++) {
        for (const Chunk& levelChunk : level.layers[i].chunks) {
            for (const Chunk& worldChunk : layers[i].chunks) {
                if (levelChunk.position == worldChunk.position) return false;
            }
        }
    }
    return true;
}

Vec2 Tilemap::TilemapToWorldPos(Int2 tilemapPos, int layer) const {
    return (tilemapPos * tileSize) + layers[layer].offset;
}

Int2 Tilemap::WorldToTilemapPos(Vec2 worldPos, int layer) const {
    return Int2(worldPos - layers[layer].offset) / tileSize;
}

void Tilemap::Render(int layer) const {

    std::vector<TileInstance> tiles;

    TilesetLookup tileset = tilesetLookup.front();

    for (Chunk chunk : layers[layer].chunks) {
        for (int i = 0; i < chunk.tiles.size(); ++i) {
            int chunkWidth = chunk.size.x;
            Int2 tilePos = chunk.position + Int2(i % chunkWidth, i / chunkWidth);

            const TileInfo* tileInfo = GetTileInfo(chunk.tiles[i].ID);
            if (!tileInfo || tileInfo->GID == 0) continue;

            Vec2 halfTileOffset = Vec2{ -0.5f, -0.5f } * (Vec2)tileSize;
            Vec2 worldOffset = layers[layer].offset + halfTileOffset;
            Vec2 worldPos = tilePos * tileSize;

            tiles.push_back(TileInstance{ worldPos, tileInfo->GID - 2 });
        }
    }

    glBindVertexArray(tileVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tileset.texture.id);

    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, tiles.size() * sizeof(TileInstance), tiles.data());

    Vec2 imageSize = tileset.tileset.getImageSize();
    Vec2 uvStep = (Vec2)tileSize / imageSize;

    shader->use();
    shader->setVec2("uvStep", uvStep);
    shader->setInt("tilesetCols", static_cast<int>(tileset.tileset.getColumnCount()));
    shader->setMat4("projection", projection);
    shader->setInt("tileSize", tileSize.x);


    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)tiles.size());

    glBindVertexArray(0);

    for (const auto &pair : objects) {
        DrawObject(pair.second, layer);
    }
}

void Tilemap::DrawTile(TileInfo tileInfo, Int2 pos, int layer, Vec2 offset) const {
    Vec2 worldOffset = layers[0].offset + offset;
    Vec2 worldPos = (Vec2)(pos * tileSize) + worldOffset;

    const TilesetLookup& tileset = tilesetLookup.front();
    int tileIndex = tileInfo.GID - 2;
    int tilesetCols = tileset.tileset.getColumnCount();
    int col = tileIndex % tilesetCols;
    int row = tileIndex / tilesetCols;
    Vec2 baseUV = Vec2(col, row) * (Vec2)tileSize;
    Vec2 textureSize = Vec2(tileset.texture.width, tileset.texture.height);
    DrawTexturedRect(worldPos, tileSize, tileset.texture, baseUV / textureSize, (Vec2)tileSize / textureSize, WHITE);
}

void Tilemap::DrawTile(uint32_t GID, Int2 pos, int layer, Vec2 offset) const {
    const TileInfo* tileInfo = GetTileInfo(GID);
    DrawTile(*tileInfo, pos, layer, offset);
}

void Tilemap::DrawObject(ObjectData object, int layer) const {
    if (!object.visible) return;

    DrawTile(object.tileGID, object.position, layer, object.offset);
}

template<typename ObjT>
void Tilemap::AddGameObject(ObjT gameObject, ObjectData objectData) {
    if constexpr (std::is_same_v<ObjT, Box>) {
        gameObject.ID = objects.size();
        gameObjects.boxes[objectData.position] = gameObject;
    }
    else return;

    objects[objectData.position] = objectData;
}

void Tilemap::AddGameObject(ObjectData objectData) {
    switch (objectData.type) {
    case ObjectType::Box:
        gameObjects.boxes[objectData.position] = Box(static_cast<uint32_t>(objects.size() + 1));
        break;
    default: return;
    }

    objects[objectData.position] = objectData;
}

void Tilemap::AddLevel(const Level& level, Int2 position) {
    size_t layerCount = level.layers.size();
    if (layerCount != this->layers.size()) return;

    for (size_t i = 0; i < layerCount; i++) {
        std::transform(
            level.layers[i].chunks.begin(),
            level.layers[i].chunks.end(),
            std::back_inserter(this->layers[i].chunks),
            [position](const Chunk& levelChunk) {
                Chunk newChunk = levelChunk;
                newChunk.position += position;
                return newChunk;
            }
        );
    }

    std::transform(
        level.collisionMap.begin(),
        level.collisionMap.end(),
        std::inserter(this->collisionMap, this->collisionMap.end()),
        [position](const Int2& solid) {
            return solid + position;
        }
    );

    for (const ObjectData& object : level.objects) {
        ObjectData newObject = object;
        newObject.position += position;
        AddGameObject(newObject);
    }
}
