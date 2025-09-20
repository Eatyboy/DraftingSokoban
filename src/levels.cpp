#include <levels.h>

std::unique_ptr<Level> LoadLevel(const char* filename) {
    tmx::Map map;
    
    if (!map.load(filename)) {
        std::string msg = "Error: Failed to load tilemap from \"" + std::string(filename) + "\"\n";
        return nullptr;
    }

    auto newLevel = std::make_unique<Level>();

    Int2 tileSize(map.getTileSize());

    const auto& layers = map.getLayers();
    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Object) {
            const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();
            const auto& mapObjects = objectLayer.getObjects();
            for (const auto& object : mapObjects) {
                uint32_t tileID = object.getTileID();
                if (tileID == 0) continue;

                Int2 position = Int2(object.getPosition()) / tileSize + Int2::up;

                ObjectType objType = stringToObjectType(object.getClass());
                if (objType == ObjectType::Unknown) continue;

                newLevel->objects.push_back(ObjectData{
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
                                newLevel->collisionMap.insert(Int2{ worldX, worldY });
                            }
                        }
                    }
                }
            }
            else {
                Int2 minCorner{ std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };
                Int2 maxCorner{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };

                std::vector<Chunk> chunks;
                for (auto& chunk : tileLayer.getChunks()) {
                    Int2 topLeft(chunk.position);
                    Int2 size(chunk.size);
                    Int2 botRight = topLeft + size;

                    minCorner.x = std::min(minCorner.x, topLeft.x);
                    minCorner.y = std::min(minCorner.y, topLeft.y);
                    maxCorner.x = std::max(maxCorner.x, botRight.x);
                    maxCorner.y = std::max(maxCorner.y, botRight.y);

                    chunks.push_back(Chunk{ topLeft, size, chunk.tiles });
                }

                newLevel->size = maxCorner - minCorner;

                Vector2 offset = Vector2{
                    (float)tileLayer.getOffset().x,
                    (float)tileLayer.getOffset().y
                };
                newLevel->layers.push_back(ChunkLayer{chunks, offset});
            }
        }
    }

    return newLevel;
}
