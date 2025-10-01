#pragma once

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include <utils.h>
#include <tiles.h>
#include <gameObjects.h>
#include <levels.h>
#include <stb_image/stb_image.h>
#include <shader.h>

class Tilemap {
private:
    struct TilesetLookup {
        const tmx::Tileset tileset;
        uint32_t firstGID;
        uint32_t lastGID;
        Texture texture;
    };

    unsigned int tileVAO = 0;
    unsigned int quadVBO = 0;
    unsigned int quadEBO = 0;
    unsigned int tileVBO = 0;

    Shader& shader;

    std::vector<ChunkLayer> layers;
    std::vector<TileInfo> tileLookup;
    std::vector<TilesetLookup> tilesetLookup;
    std::unordered_set<Int2, Int2::Hash> collisionMap;
    std::vector<ObjectData> objects;
public:
    Int2 tileSize;
    GameObjects gameObjects;

    Tilemap(Shader& shader) : tileSize(), shader(shader) {}

    bool LoadTilemap(const char* filename);

    std::optional<tmx::TileLayer::Tile> GetTile(int posX, int posY, int layer) const;
    std::optional<tmx::TileLayer::Tile> GetTile(Int2 pos, int layer) const;
    std::optional<Chunk> GetChunk(Int2 pos, int layer) const;
    const TileInfo* GetTileInfo(uint32_t GID) const;
    ObjectData& GetObject(uint32_t ID);
    bool IsSolid(Int2 pos) const;
    bool CanPlaceLevel(const Level& level, Int2 position);

    Vec2 TilemapToWorldPos(Int2 tilemapPos, int layer = 0) const;
    Int2 WorldToTilemapPos(Vec2 worldPos, int layer = 0) const;

    void DrawTile(TileInfo tile, Int2 pos, int layer, Vec2 offset = { 0, 0 }) const;
    void DrawTile(uint32_t GID, Int2 pos, int layer, Vec2 offset = { 0, 0 }) const;
    void DrawObject(ObjectData object, int layer) const;
    void Render(int layer) const;

    template<typename ObjT>
    void AddGameObject(ObjT gameObject, ObjectData objectData);
    void AddGameObject(ObjectData objectData);
    void AddLevel(const Level& level, Int2 position);
};
