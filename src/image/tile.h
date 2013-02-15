
#ifndef TILE_H
#define TILE_H

#include <array>

namespace bigimage {

// *****************************************************************************
// Tile, TileInfo
// *****************************************************************************

// Good tile widths are such that a line from the tile is a multiple of the cache line
// size. On current hardware, this is often 64 bytes.
// Tile dimensions in pixels
const int32_t kTileWidth = 64;
const int32_t kTileHeight = 64;
const int32_t kTilePixels = kTileWidth*kTileHeight;

// This structure may go away, it serves no real purpose as tiles can always be
// handled through TileInfo structs.
template<typename imageT>
struct Tile {
    typedef imageT image_t;
    typedef typename imageT::pixel_val_t pixel_val_t;
    
    std::array<pixel_val_t, kTilePixels> pixels;
};

template<typename imageT>
struct TileInfo {
    typedef imageT image_t;
    typedef typename imageT::pixel_val_t pixel_val_t;
    
    std::array<pixel_val_t, kTilePixels> * pixels;
    int32_t x, y;
    uint32_t references;
    
    TileInfo() {}
    TileInfo(int32_t _x, int32_t _y, Tile<imageT> & t):
        pixels(&t.pixels), x(_x), y(_y), references(0) {}
    
    
    // auto EachPixel(const std::function<pixel_val_t(pixel_val_t&)> & fn)
    template<typename fnT>
    auto EachPixel(const fnT & fn)
        -> void
    {
        for(pixel_val_t & p : *pixels)
            fn(p);
    }
    
    // auto EachPixelXY(const std::function<pixel_val_t(int32_t, int32_t)> & fn)
    template<typename fnT>
    auto EachPixelXY(const fnT & fn)
        -> void
    {
        int32_t i = 0;
        for(int32_t py = 0; py < kTileHeight; ++py)
        for(int32_t px = 0; px < kTileWidth; ++px)
            fn(x + px, y + py, (*pixels)[i++]);
    }
};


} // namespace bigimage
#endif // TILE_H
