
// *****************************************************************************
// Images are divided up into uniformly sized tiles for division of processing
// between threads. Each tile contains the pixel data for its portion of the
// image as a contiguous block, allowing efficient use of caches.
// 
// Tile managers allow use of different strategies for data layout to preserve
// locality, reduce copying, etc. Hooks may eventually be added to allow
// preloading, limit memory usage, etc, currently simple on-demand paging is
// implemented via memory mapping.
//
// Image types are defined with a tile manager and a pixel type, which describes
// not only the datatype of the pixel but also information such as numeric limits,
// encoding and conversions, etc.

// Implementation notes:
// Const correctness is currently ignored for the tile traversal functions due
// to the large amount of code duplication it would cause.


#ifndef BIGIMAGE_H
#define BIGIMAGE_H

#include <cstdio>

#include <iostream>
#include <string>

#include <functional>
#include <utility>
#include <thread>
#include <atomic>
#include <mutex>

#include <boost/format.hpp>

#include "targa_io.h"

#include "pixeltype.h"
#include "tile.h"
#include "tilemanager.h"
#include "imageproc.h"
#include "rect.h"

namespace bigimage {
using boost::format;
using std::cerr;
using std::endl;

const int kNThreads = 16;


// *****************************************************************************
// Image types
// *****************************************************************************

template<typename pixT, typename TM>
struct ImageType: public pixT {
    typedef pixT pixel_t;
    typedef TM TileManager;
    
    // const int32_t kTileWidth = 64;
    // const int32_t kTileHeight = 64;
    // const int32_t kTilePixels = kTileWidth*kTileHeight;
};

typedef ImageType<PixelTypeU32, TileBlockManager> ImageTypeU32;
typedef ImageType<PixelTypeRGBA32, TileBlockManager> ImageTypeRGBA32;
typedef ImageType<PixelTypeRGBAf, TileBlockManager> ImageTypeRGBAf;


// *****************************************************************************
// BigImage
// *****************************************************************************

// Working image size size must be a multiple of the tile size. Tile managers
// may place additional constraints on image size.
// Tile managers may lay out pixel data in non-linear fashion, but tile info is
// linearly arranged and contains references to the pixel data.
template<typename imageT>
class BigImage {
  public:
    typedef imageT image_t;
    typedef typename imageT::pixel_t pixel_t;
    typedef typename imageT::pixel_val_t pixel_val_t;
    typedef TileInfo<pixel_t> TileInfo;
    typedef Tile<pixel_t> Tile;
    
  protected:
    typename image_t::TileManager tileManager;
    Tile * tiles;
    std::vector<TileInfo> tinfo;
    std::vector<TileInfo *> torder;
    int32_t width, height;
    int32_t xtiles, ytiles;
    
    
  public:
    BigImage(int32_t w, int32_t h, const std::string & backingFilePath);
    ~BigImage();
    
    int32_t Width() const {return width;}
    int32_t Height() const {return height;}
    std::tuple<int32_t, int32_t> Size() const {return std::make_tuple(width, height);}
    
    // Get vector of linear-ordered tiles.
    // Intended for efficient look-up of tiles by location.
    std::vector<TileInfo> & GetTiles() {return tinfo;}
    const std::vector<TileInfo> & GetTiles() const {return tinfo;}
    
    // Get vector of memory-ordered pointers to tiles.
    // Intended for efficient traversal of all tiles.
    std::vector<TileInfo *> & GetNaturalOrdering() {return torder;}
    const std::vector<TileInfo *> & GetNaturalOrdering() const {return torder;}
    
    // Get tile from pixel coordinates
    TileInfo & GetTile(int32_t x, int32_t y);
    
    // Get pixel.
    pixel_val_t & GetPixel(int32_t x, int32_t y);
    
    // Get pixel in tile.
    // Coordinates may be relative to either image or tile origin.
    pixel_val_t & GetPixel(TileInfo & tile, int32_t x, int32_t y);
    
    // Iterate over all tiles, calling function of form:
    // void(TileInfo &)
    // void(ctxT &, TileInfo &)
    template<typename fnT>
    void EachTile(const fnT & fn);
    
    template<typename ctxT, typename fnT>
    void EachTile(ctxT * threadContexts, const fnT & fn);
    
    // Iterate over tiles that intersect given rect, calling function of form:
    // void(TileInfo &)
    // void(ctxT &, TileInfo &)
    template<typename fnT>
    void EachTile(const Rect & rect, const fnT & fn);
    
    template<typename ctxT, typename fnT>
    void EachTile(ctxT * threadContexts, const Rect & rect, const fnT & fn);
    
    // Iterate over each pixel, calling function of form void(pixel_val_t & pix)
    template<typename fnT>
    void EachPixel(const fnT & fn);
    
    // Iterate over each pixel, calling function of form void(int32_t x, int32_t y, pixel_val_t & pix)
    // Coordinates are in image space
    template<typename fnT>
    void EachPixelXY(const fnT & fn);
    
    // Get pixels as linear pixel data.
    // Pixels array must be allocated by caller.
    // TODO: flip vertical/horizontal, lambda filter
    template<typename dpixelT>
    void GetPixels(typename dpixelT::pixel_val_t * pixels);
    
    template<typename dpixelT>
    void GetPixels(const Rect & rect, typename dpixelT::pixel_val_t * pixels);
    
    // Set pixels from linear pixel data.
    // TODO: flip vertical/horizontal, lambda filter
    template<typename spixelT>
    void SetPixels(const typename spixelT::pixel_val_t * pixels);
    
    template<typename spixelT>
    void SetPixels(const Rect & rect, const typename spixelT::pixel_val_t * pixels);
    
    void PrintInfo() const;
};


// *****************************************************************************
// BigImage implementation
// *****************************************************************************
template<typename imageT>
BigImage<imageT>::BigImage(int32_t w, int32_t h, const std::string & backingFilePath):
    tileManager(backingFilePath),
    tiles(nullptr),
    width(0), height(0),
    xtiles(0), ytiles(0)
{
    width = w;
    height = h;
    xtiles = (w + kTileWidth - 1)/kTileWidth;
    ytiles = (h + kTileHeight - 1)/kTileHeight;
    tiles = tileManager.AllocMain(*this);
}

template<typename imageT>
BigImage<imageT>::~BigImage()
{
    tileManager.FreeMain(tiles);
}

template<typename imageT>
auto BigImage<imageT>::GetTile(int32_t x, int32_t y)
    -> TileInfo &
{
    int32_t tx = x/kTileWidth;
    int32_t ty = y/kTileHeight;
    return tinfo[ty*xtiles + tx];
}

template<typename imageT>
auto BigImage<imageT>::GetPixel(int32_t x, int32_t y)
    -> pixel_val_t &
{
    return GetPixel(GetTile(x, y), x, y);
}

template<typename imageT>
auto BigImage<imageT>::GetPixel(TileInfo & tile, int32_t x, int32_t y)
    -> pixel_val_t &
{
    int32_t px = x % kTileWidth;
    int32_t py = y % kTileHeight;
    return tile.pixels[py*kTileWidth + px];
}


template<typename imageT>
template<typename fnT>
auto BigImage<imageT>::EachTile(const fnT & fn)
    -> void
{
    uint8_t dummyContexts[kNThreads];
    EachTile(dummyContexts, std::function<void(uint8_t &, TileInfo &)>([fn](uint8_t & ctx, TileInfo & ti){fn(ti);}));
}

template<typename imageT>
template<typename fnT>
auto BigImage<imageT>::EachTile(const Rect & rect, const fnT & fn)
    -> void
{
    uint8_t dummyContexts[kNThreads];
    EachTile(dummyContexts, rect, std::function<void(uint8_t &, TileInfo &)>([fn](uint8_t & ctx, TileInfo & ti){fn(ti);}));
}


template<typename imageT>
template<typename ctxT, typename fnT>
auto BigImage<imageT>::EachTile(ctxT * threadContexts, const fnT & fn)
    -> void
{
#if(0)
    for(TileInfo & ti : tinfo)
        fn(threadContexts[0], ti);
#else
    // Each thread gets a lock on the job counter, checks for availability of work,
    // increments job counter, and releases the lock to do the job.
    std::vector<std::thread> threads;
    size_t nextTile = 0;
    std::mutex tileCtrMtx;
    size_t ntiles = xtiles*ytiles;
    
    for(int tid = 0; tid < kNThreads; ++tid)
    {
        threads.emplace_back(std::thread([&](int threadID){
            tileCtrMtx.lock();
            while(nextTile < ntiles)
            {
                TileInfo & ti = *torder[nextTile++];
                tileCtrMtx.unlock();
                fn(threadContexts[threadID], ti);
                tileCtrMtx.lock();
            }
            tileCtrMtx.unlock();
        }, tid));
    }
    for(auto & t : threads)
        t.join();
#endif
}


template<typename imageT>
template<typename ctxT, typename fnT>
auto BigImage<imageT>::EachTile(ctxT * threadContexts, const Rect & rect, const fnT & fn)
    -> void
{
#if(0)
    for(TileInfo & ti : tinfo)
        if(rect.Overlaps(ti.x, ti.y, kTileWidth, kTileHeight))
            fn(threadContexts[0], ti);
#else
    // Each thread gets a lock on the job counter, checks for availability of work,
    // increments job counter, and releases the lock to do the job.
    std::vector<std::thread> threads;
    size_t nextTile = 0;
    std::mutex tileCtrMtx;
    size_t ntiles = xtiles*ytiles;
    
    for(int tid = 0; tid < kNThreads; ++tid)
    {
        threads.emplace_back(std::thread([&](int threadID){
            tileCtrMtx.lock();
            
            // skip tiles not overlapping rect
            while(nextTile < ntiles && !rect.Overlaps(torder[nextTile]->x, torder[nextTile]->y, kTileWidth, kTileHeight))
                ++nextTile;
            
            while(nextTile < ntiles)
            {
                TileInfo & ti = *torder[nextTile++];
                tileCtrMtx.unlock();
                fn(threadContexts[threadID], ti);
                tileCtrMtx.lock();
            }
            tileCtrMtx.unlock();
        }, tid));
    }
    for(auto & t : threads)
        t.join();
#endif
}


template<typename imageT>
template<typename fnT>
auto BigImage<imageT>::EachPixel(const fnT & fn)
    -> void
{
    EachTile([fn](TileInfo & ti){
        for(pixel_val_t & p : *ti.pixels)
            fn(p);
    });
}

template<typename imageT>
template<typename fnT>
auto BigImage<imageT>::EachPixelXY(const fnT & fn)
    -> void
{
    EachTile([fn](TileInfo & ti){
        int32_t i = 0;
        for(int32_t y = 0; y < kTileHeight; ++y)
        for(int32_t x = 0; x < kTileWidth; ++x)
            fn(ti.x + x, ti.y + y, (*ti.pixels)[i++]);
    });
}

template<typename imageT>
template<typename dpixelT>
auto BigImage<imageT>::GetPixels(const Rect & rect, typename dpixelT::pixel_val_t * pixels) -> void
{
    // For each line of each tile, copy line segments from tile to pixel array
    EachTile(rect, [&](TileInfo & ti){
        Rect tr = rect.Intersect(ti.x, ti.y, kTileWidth, kTileHeight);
        int32_t tx = tr.x - ti.x, ty = tr.y - ti.y;// source rect coordinates relative to tile
        int32_t dx = tr.x - rect.x, dy = tr.y - rect.y;// source rect coordinates relative to destination rect
        for(int32_t y = 0; y < tr.h; ++y)
            CopyPixels<dpixelT, typename imageT::pixel_t>(
                pixels + (dy + y)*rect.w + dx,
                &((*ti.pixels)[(ty + y)*kTileWidth + tx]), tr.w);
    });
}

template<typename imageT>
template<typename dpixelT>
auto BigImage<imageT>::GetPixels(typename dpixelT::pixel_val_t * pixels) -> void
{
    GetPixels<dpixelT>(Rect(0, 0, width, height), pixels);
}

template<typename imageT>
template<typename dpixelT>
auto BigImage<imageT>::SetPixels(const Rect & rect, const typename dpixelT::pixel_val_t * pixels) -> void
{
    // For each line of each tile, copy line segments from source pixel array
    EachTile(rect, [&](TileInfo & ti){
        Rect tr = rect.Intersect(ti.x, ti.y, kTileWidth, kTileHeight);
        int32_t tx = tr.x - ti.x, ty = tr.y - ti.y;// source rect coordinates relative to tile
        int32_t dx = tr.x - rect.x, dy = tr.y - rect.y;// source rect coordinates relative to destination rect
        for(int32_t y = 0; y < tr.h; ++y)
            CopyPixels<typename imageT::pixel_t, dpixelT>(
                &((*ti.pixels)[(ty + y)*kTileWidth + tx],
                pixels + (dy + y)*rect.w + dx), tr.w);
    });
}

template<typename imageT>
template<typename dpixelT>
auto BigImage<imageT>::SetPixels(const typename dpixelT::pixel_val_t * pixels) -> void
{
    SetPixels<dpixelT>(Rect(0, 0, width, height), pixels);
}

template<typename imageT>
auto BigImage<imageT>::PrintInfo() const -> void
{
    cerr << "================================\n";
    cerr << format("size: %dx%d\n")% width % height;
    cerr << format("tiles: %dx%d\n")% xtiles % ytiles;
}

} // namespace bigimage
#endif // BIGIMAGE_H
