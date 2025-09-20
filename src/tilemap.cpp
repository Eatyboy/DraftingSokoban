#include <tilemap.h>
#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <algorithm>
#include <iterator>

Tilemap::Tilemap() {
    tileSize = Int2::zero;
}

Tilemap::Tilemap(const char* filename) {
    tmx::Map map;
    if (!map.load(filename)) {
        std::string msg = "Error: Failed to load tilemap from \"" + std::string(filename) + "\"\n";
        throw std::runtime_error(msg);
    }

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
                    gameObjects.boxes.push_back(Box(static_cast<uint32_t>(objects.size())));
                    break;
                };
                
                objects.push_back(ObjectData{
                    position,
                    tileSize,
                    Vector2{ 0, 0 },
                    objType,
                    tileID,
                    object.getRotation(),
                    object.visible()
                });
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
                Vector2 offset = Vector2{
                    (float)tileLayer.getOffset().x,
                    (float)tileLayer.getOffset().y
                };
                this->layers.push_back(ChunkLayer{chunks, offset});
            }
        }
    }

    const auto& tilesets = map.getTilesets();

    if (tilesets.empty()) return;

    uint32_t maxGID = tilesets.back().getLastGID();
    size_t newSize = static_cast<size_t>(maxGID) + 1;
    tileLookup.resize(newSize, TileInfo{ nullptr, nullptr });

    for (const auto& tileset : tilesets) {
        uint32_t first = tileset.getFirstGID();
        uint32_t count = tileset.getTileCount();

        std::string texPath = tileset.getImagePath();
        Texture2D texture = LoadTexture(texPath.c_str());

        if (texture.id == NULL) {
            continue;
        }

        textures[texPath] = texture;

        for (uint32_t i = 0; i < count; ++i) {
            const tmx::Tileset::Tile* tile = tileset.getTile(i);
            if (tile == nullptr) continue;
            tileLookup[(size_t)first + i - 1] = TileInfo{
                &tileset,
                tile,
                first + i,
                Rectangle{ 
                    (float)tile->imagePosition.x, 
                    (float)tile->imagePosition.y,
                    (float)tile->imageSize.x,
                    (float)tile->imageSize.y,
                },
                &textures[texPath],
                tile->properties
            };
        }
    }
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

ObjectData& Tilemap::GetObject(uint32_t ID) {
    return objects.at(ID);
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

Vector2 Tilemap::TilemapToWorldPos(Int2 tilemapPos, int layer) const {
    return (tilemapPos * tileSize).toVector2() + layers[layer].offset;
}

Int2 Tilemap::WorldToTilemapPos(Vector2 worldPos, int layer) const {
    return Int2(worldPos - layers[layer].offset) / tileSize;
}

void Tilemap::Render(int layer) const {
    for (Chunk chunk : layers[layer].chunks) {
        for (int i = 0; i < chunk.tiles.size(); ++i) {
            int chunkWidth = chunk.size.x;
            Int2 tilePos = chunk.position + Int2(i % chunkWidth, i / chunkWidth);

            const TileInfo* tileInfo = GetTileInfo(chunk.tiles[i].ID);
            if (tileInfo && tileInfo->GID != 0) DrawTile(*tileInfo, tilePos, layer);
        }
    }
    for (const ObjectData &object : objects) {
        DrawObject(object, layer);
    }
}

void Tilemap::DrawTile(TileInfo tileInfo, Int2 pos, int layer, Vector2 offset) const {
    Int2 tileSize = Int2(tileInfo.texCoords.width, tileInfo.texCoords.height);
    Vector2 halfTileOffset = Vector2{ -0.5f, -0.5f } * tileSize.toVector2();
    Vector2 worldOffset = layers[0].offset + halfTileOffset + offset;
    Vector2 worldPos = (pos * tileSize).toVector2() + worldOffset;
    DrawTextureRec(*tileInfo.texture, tileInfo.texCoords, worldPos, WHITE);
}

void Tilemap::DrawTile(uint32_t GID, Int2 pos, int layer, Vector2 offset) const {
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
        gameObjects.boxes.push_back(gameObject);
    }
    else return;

    objects.push_back(objectData);
}

void Tilemap::AddGameObject(ObjectData objectData) {
    switch (objectData.type) {
    case ObjectType::Box:
        gameObjects.boxes.push_back(Box(static_cast<uint32_t>(objects.size())));
        break;
    default: return;
    }

    objects.push_back(objectData);
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
