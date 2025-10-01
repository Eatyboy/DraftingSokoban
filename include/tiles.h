#pragma once

#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/Object.hpp>
#include <utils.h>
#include <glad.h>
#include <GLFW/glfw3.h>

struct TileInfo {
    const tmx::Tileset::Tile* tile = nullptr;
    size_t tilesetIndex = 0;
    uint32_t GID = 0;

    std::vector<tmx::Property> props;
};

struct Chunk {
    Int2 position;
    Int2 size;
    std::vector<tmx::TileLayer::Tile> tiles;
};

struct ChunkLayer {
    std::vector<Chunk> chunks;
    Vec2 offset;
};
