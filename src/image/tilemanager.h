
#ifndef TILEMANAGER_H
#define TILEMANAGER_H

#include "filestore.h"
#include "tile.h"

namespace bigimage {

// Block dimensions in tiles
const int32_t kBlockWidth = 8;
const int32_t kBlockHeight = 8;
const int32_t kBlockTiles = kBlockWidth*kBlockHeight;

// *****************************************************************************
// Tile managers
// *****************************************************************************

// TODO: TileLinearManager, TileQuadtreeManager

class TileBlockManager {
    std::string backingFilePath;
    filestore::MappedFile * backingFile;
    
  public:
    TileBlockManager(const std::string & bfPath): backingFilePath(bfPath) {}
    ~TileBlockManager() {}
    
    // Allocate main image tiles and initialize tinfo entries
    template<typename image_t>
    auto AllocMain(image_t & image) -> typename image_t::Tile *
    {
        int32_t width, height;
        std::tie(width, height) = image.Size();
        int32_t xtiles = (width + kTileWidth - 1)/kTileWidth;
        int32_t ytiles = (height + kTileHeight - 1)/kTileHeight;
        
        typename image_t::Tile * tiles;
        if(backingFilePath != "") {
            backingFile = new filestore::MappedFile(backingFilePath.c_str(), xtiles*ytiles*sizeof(typename image_t::Tile));
            tiles = static_cast<typename image_t::Tile*>(backingFile->region.get_address());
        }
        else {
            tiles = new typename image_t::Tile[xtiles*ytiles];
        }
        std::vector<typename image_t::TileInfo> & tinfo = image.GetTiles();
        std::vector<typename image_t::TileInfo *> & torder = image.GetNaturalOrdering();
        tinfo.resize(xtiles*ytiles);
        torder.resize(xtiles*ytiles);
        
        const int32_t kBlockRowTiles = xtiles*kBlockHeight;
        // tx and ty are global tile coordinates
        for(int tx = 0; tx < xtiles; ++tx)
        for(int ty = 0; ty < ytiles; ++ty)
        {
            // Map to tiled and blocked pixel data
            // Which block row are we in?
            int32_t by = ty/kBlockHeight;
            int32_t bx = tx/kBlockWidth;// block coordinates
            int32_t btx = tx % kBlockWidth;// block-relative tile coordinates
            int32_t bty = ty % kBlockHeight;
            int32_t tidx = (by*kBlockRowTiles + bx*kBlockTiles) + (bty*kBlockWidth + btx);
            typename image_t::Tile & tile = tiles[tidx];
            tinfo[ty*xtiles + tx] = typename image_t::TileInfo(tx*kTileWidth, ty*kTileHeight, tile);
            torder[tidx] = &tinfo[ty*xtiles + tx];
        }
        return tiles;
    }
    
    template<typename Tile>
    void FreeMain(Tile * tiles) {
        if(backingFile) {
            delete backingFile;
            backingFile = nullptr;
        }
        else {
            delete[] tiles;
        }
    }
    
    template<typename image_t>
    auto AllocTmp() -> typename image_t::Tile * {return new typename image_t::Tile;}
    
    template<typename image_t>
    void FreeTmp(typename image_t::Tile * tile) {delete tile;}
    
    template<typename image_t>
    void MovePixels(typename image_t::TileInfo & dst, typename image_t::TileInfo & src) {
        memcpy(dst.pixels, src.pixels, sizeof(typename image_t::Tile));
    }
};

} // namespace bigimage
#endif // TILEMANAGER_H
