#pragma once

#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/Object.hpp>
#include <raylib.h>
#include <int2.h>

struct TileInfo {
    const tmx::Tileset* tileset = nullptr;
    const tmx::Tileset::Tile* tile = nullptr;

    uint32_t GID = 0;
    Rectangle texCoords;
    const Texture2D* texture = nullptr;

    std::vector<tmx::Property> props;
};

struct Chunk {
    Int2 position;
    Int2 size;
    std::vector<tmx::TileLayer::Tile> tiles;
};

struct ChunkLayer {
    std::vector<Chunk> chunks;
    Vector2 offset;
};
