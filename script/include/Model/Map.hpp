#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include <vector>

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

    // Exclusion radius around big buildings (in tiles, from anchor tile)
    static constexpr int   BUILDING_EXCLUSION = 3;

    // Tile placement status (used for the map overlay)
    enum class TileStatus { Free, Path, Decor, Building, Tower };

    Map();

    void update(float dt) {}
    void draw(sf::RenderTarget& target, sf::Vector2f offset, float scale) const;

    // Waypoints for enemy path, pre-scaled to the given render scale
    std::vector<sf::Vector2f> getWaypoints(float scale) const;

    // Tile/cell coordinate helpers
    sf::Vector2i worldToTile(sf::Vector2f worldPos, float scale) const;
    sf::Vector2i cellOrigin(sf::Vector2f worldPos, float scale) const;
    sf::Vector2f snapToCell(sf::Vector2f worldPos, float scale) const;

    // Returns true if the full 2x2 cell is free for tower placement
    bool canPlaceCell(sf::Vector2f worldPos, float scale,
                      const std::vector<sf::Vector2f>& occupiedCenters) const;

    // Returns the placement status of every tile (for rendering the overlay)
    std::array<TileStatus, COLS * ROWS>
        buildPlacementOverlay(const std::vector<sf::Vector2f>& occupiedCenters,
                              float scale) const;

private:
    using Layer = std::array<int, COLS * ROWS>;

    void loadTilesets();
    void loadLayers();
    void drawLayer(sf::RenderTarget& target, const Layer& layer,
                   sf::Vector2f offset, float scale) const;
    void drawTile(sf::RenderTarget& target, int gid,
                  float px, float py, float scale) const;

    bool isPathTile(int col, int row) const;
    bool isForegroundBlocked(int col, int row) const;
    bool isBuildingExclusion(int col, int row) const;
    bool isInBounds(int col, int row) const;

    // Returns true if the single tile (col,row) is blocked for any reason
    bool isTileBlocked(int col, int row) const;

    sf::Texture m_floorTexture;
    sf::Texture m_natureTexture;
    sf::Texture m_castleTexture;
    sf::Texture m_orcCaveTexture;

    Layer m_bgLayer;
    Layer m_fgLayer;
};