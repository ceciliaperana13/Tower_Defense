#include "Map.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// Parses a flat CSV string (may contain newlines) into the layer array
static void parseCSVData(const std::string& data,
                         std::array<int, Map::COLS * Map::ROWS>& layer) {
    int index = 0;
    std::stringstream ss(data);
    std::string token;
    while (std::getline(ss, token, ',') && index < Map::COLS * Map::ROWS) {
        // Strip whitespace and newlines
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        if (!token.empty())
            layer[index++] = std::stoi(token);
    }
}

// Extracts the two CSV data blocks from the TMX file
static bool parseTMX(const std::string& path,
                     std::array<int, Map::COLS * Map::ROWS>& bg,
                     std::array<int, Map::COLS * Map::ROWS>& fg) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Map] Cannot open TMX: " << path << "\n";
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    // Find each <data encoding="csv"> ... </data> block
    const std::string openTag  = "<data encoding=\"csv\">";
    const std::string closeTag = "</data>";

    std::vector<std::string> blocks;
    size_t pos = 0;
    while (blocks.size() < 2) {
        size_t start = content.find(openTag, pos);
        if (start == std::string::npos) break;
        start += openTag.size();
        size_t end = content.find(closeTag, start);
        if (end == std::string::npos) break;
        blocks.push_back(content.substr(start, end - start));
        pos = end + closeTag.size();
    }

    if (blocks.size() < 2) {
        std::cerr << "[Map] TMX: expected 2 layers, found " << blocks.size() << "\n";
        return false;
    }

    parseCSVData(blocks[0], bg);
    parseCSVData(blocks[1], fg);
    return true;
}

Map::Map() {
    loadTilesets();
    loadLayers();
}

void Map::loadTilesets() {
    auto load = [](sf::Texture& tex, const std::string& path) {
        if (!tex.loadFromFile(path))
            std::cerr << "[Map] Texture not found: " << path << "\n";
    };
    load(m_floorTexture,   "../assets/tilemap/TilesetFloor.png");
    load(m_natureTexture,  "../assets/tilemap/TilesetNature.png");
    load(m_castleTexture,  "../assets/sprites/big_buildings/castle.png");
    load(m_orcCaveTexture, "../assets/sprites/big_buildings/orc_cave.png");
}

void Map::loadLayers() {
    m_bgLayer.fill(0);
    m_fgLayer.fill(0);
    parseTMX("../assets/tilemap/tilemap.tmx", m_bgLayer, m_fgLayer);
}

void Map::draw(sf::RenderTarget& target, sf::Vector2f offset, float scale) const {
    drawLayer(target, m_bgLayer, offset, scale);
    drawLayer(target, m_fgLayer, offset, scale);
}

void Map::drawLayer(sf::RenderTarget& target, const Layer& layer,
                    sf::Vector2f offset, float scale) const {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            int gid = layer[row * COLS + col];
            if (gid == 0) continue;
            float px = offset.x + col * TILE_SIZE * scale;
            float py = offset.y + row * TILE_SIZE * scale;
            drawTile(target, gid, px, py, scale);
        }
    }
}

void Map::drawTile(sf::RenderTarget& target, int gid,
                   float px, float py, float scale) const {
    sf::RectangleShape shape;
    shape.setFillColor(sf::Color::White);

    // Tiled anchors big sprites at their bottom-left tile, so offset upward
    // by (spriteHeight - tileSize) to align with the visual top
    constexpr float BIG_SIZE   = 64.f;
    constexpr float ANCHOR_OFF = BIG_SIZE - TILE_SIZE; // 48px in logical coords

    if (gid == GID_ORC_CAVE) {
        shape.setSize({ BIG_SIZE * scale, BIG_SIZE * scale });
        shape.setPosition({ px, py - ANCHOR_OFF * scale });
        shape.setTexture(&m_orcCaveTexture);
        shape.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 }));
    }
    else if (gid == GID_CASTLE) {
        shape.setSize({ BIG_SIZE * scale, BIG_SIZE * scale });
        shape.setPosition({ px, py - ANCHOR_OFF * scale });
        shape.setTexture(&m_castleTexture);
        shape.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 }));
    }
    else if (gid >= GID_NATURE_FIRST) {
        int localId = gid - GID_NATURE_FIRST;
        int srcX = (localId % NATURE_COLS) * TILE_SIZE;
        int srcY = (localId / NATURE_COLS) * TILE_SIZE;
        shape.setSize({ TILE_SIZE * scale, TILE_SIZE * scale });
        shape.setPosition({ px, py });
        shape.setTexture(&m_natureTexture);
        shape.setTextureRect(sf::IntRect({ srcX, srcY }, { TILE_SIZE, TILE_SIZE }));
    }
    else {
        int localId = gid - GID_FLOOR_FIRST;
        int srcX = (localId % FLOOR_COLS) * TILE_SIZE;
        int srcY = (localId / FLOOR_COLS) * TILE_SIZE;
        shape.setSize({ TILE_SIZE * scale, TILE_SIZE * scale });
        shape.setPosition({ px, py });
        shape.setTexture(&m_floorTexture);
        shape.setTextureRect(sf::IntRect({ srcX, srcY }, { TILE_SIZE, TILE_SIZE }));
    }

    target.draw(shape);
}