#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string>

// Tilemap: 33x22 tiles, tile size 16px
// Two layers: background (TilesetFloor) and foreground (TilesetNature + buildings)
class Map {
public:
    static constexpr int   COLS      = 33;
    static constexpr int   ROWS      = 22;
    static constexpr int   TILE_SIZE = 16;

    // firstgid values from tilemap.tmx
    static constexpr int   GID_FLOOR_FIRST   = 1;
    static constexpr int   GID_ORC_CAVE      = 573;
    static constexpr int   GID_CASTLE        = 574;
    static constexpr int   GID_NATURE_FIRST  = 575;

    static constexpr int   FLOOR_COLS   = 22;
    static constexpr int   NATURE_COLS  = 24;

    Map();

    void update(float dt) {}
    void draw(sf::RenderTarget& target, sf::Vector2f offset, float scale) const;

private:
    using Layer = std::array<int, COLS * ROWS>;

    void loadTilesets();
    void loadLayers();
    void drawLayer(sf::RenderTarget& target, const Layer& layer,
                   sf::Vector2f offset, float scale) const;

    void drawTile(sf::RenderTarget& target, int gid,
                  float px, float py, float scale) const;

    sf::Texture m_floorTexture;
    sf::Texture m_natureTexture;
    sf::Texture m_castleTexture;
    sf::Texture m_orcCaveTexture;

    Layer m_bgLayer;
    Layer m_fgLayer;
};