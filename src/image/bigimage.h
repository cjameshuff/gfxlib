// Images are organized in tiles and blocks.
// Tiles are rectangular sets of pixels, sized to make efficient use of CPU caches.
// Blocks are sets of tiles that are sized for efficient use of disk I/O.

// With 64x64 tiles and 8x8 blocks:
// 4096 pixels/tile, 16384 B at 32 bpp.
// 232144 pixels/block, 512x512 pixels, 1 MB at 32 bpp
// Image dimensions multiple of 64.


// Workflow for applying some per-pixel transform
// load block, hand tiles to processing threads while loading the next block
// On completion of processing, queue block for writing


// Layout of tiles in memory for quadtree:
// 0 1 4 5
// 2 3 6 7
// 8 9 C D
// A B E F


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
// #include <chrono>

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

// Minimum image size is 1 block, and size must be a multiple of the tile size.
// Tile pixel data is laid out in non-linear fashion, but tile info is linearly
// arranged and contains references to the pixel data.
//
// The space-linear tinfo vector is used for spatial addressing of tiles, while
// the torder vector provides a memory-linear ordering for efficient traversal
// of tiles.
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
    
    std::vector<TileInfo> & GetTiles() {return tinfo;}
    const std::vector<TileInfo> & GetTiles() const {return tinfo;}
    
    std::vector<TileInfo *> & GetNaturalOrdering() {return torder;}
    const std::vector<TileInfo *> & GetNaturalOrdering() const {return torder;}
    
    TileInfo & GetTile(int32_t w, int32_t h);
    
    pixel_val_t & GetPixel(int32_t w, int32_t h);
    pixel_val_t & GetPixel(TileInfo & tile, int32_t w, int32_t h);
    
    template<typename fnT>
    void EachTile(const fnT & fn);
    
    template<typename fnT>
    void EachTile(const Rect & rect, const fnT & fn);
    
    template<typename ctxT, typename fnT>
    void EachTile(ctxT * threadContexts, const fnT & fn);
    
    template<typename ctxT, typename fnT>
    void EachTile(ctxT * threadContexts, const Rect & rect, const fnT & fn);
    
    // template<typename ctxT>
    // void EachTile(ctxT * threadContexts, const std::function<void(ctxT &, TileInfo &)> & fn);
    // 
    // template<typename ctxT>
    // void EachTile(ctxT * threadContexts, const Rect & rect, const std::function<void(ctxT &, TileInfo &)> & fn);
    
    template<typename fnT>
    void EachPixel(const fnT & fn);
    template<typename fnT>
    void EachPixelXY(const fnT & fn);
    
    template<typename dpixelT>
    void GetPixels(const Rect & rect, typename dpixelT::pixel_val_t * pixels);
    
    template<typename dpixelT>
    void GetPixels(typename dpixelT::pixel_val_t * pixels);
    
    template<typename spixelT>
    void SetPixels(const Rect & rect, const typename spixelT::pixel_val_t * pixels);
    
    template<typename spixelT>
    void SetPixels(const typename spixelT::pixel_val_t * pixels);
    
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
    return tinfo[(ty*xtiles + tx)];
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
// auto BigImage<imageT>::EachTile(const std::function<void(TileInfo &)> & fn)
auto BigImage<imageT>::EachTile(const fnT & fn)
    -> void
{
    uint8_t dummyContexts[kNThreads];
    EachTile(dummyContexts, std::function<void(uint8_t &, TileInfo &)>([fn](uint8_t & ctx, TileInfo & ti){fn(ti);}));
}

template<typename imageT>
template<typename fnT>
// auto BigImage<imageT>::EachTile(const Rect & rect, const std::function<void(TileInfo &)> & fn)
auto BigImage<imageT>::EachTile(const Rect & rect, const fnT & fn)
    -> void
{
    uint8_t dummyContexts[kNThreads];
    EachTile(dummyContexts, rect, std::function<void(uint8_t &, TileInfo &)>([fn](uint8_t & ctx, TileInfo & ti){fn(ti);}));
}


template<typename imageT>
// template<typename ctxT>
// auto BigImage<imageT>::EachTile(ctxT * threadContexts, const std::function<void(ctxT &, TileInfo &)> & fn)
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
// template<typename ctxT>
// auto BigImage<imageT>::EachTile(ctxT * threadContexts, const Rect & rect, const std::function<void(ctxT &, TileInfo &)> & fn)
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
    const int32_t yblocks = (ytiles + kBlockHeight - 1)/kBlockHeight;
    const int32_t xblocks = (xtiles + kBlockWidth - 1)/kBlockWidth;
    cerr << format("size: %dx%d\n")% width % height;
    cerr << format("tiles: %dx%d\n")% xtiles % ytiles;
    cerr << format("blocks: %dx%d\n")% xblocks % yblocks;
}

} // namespace bigimage
#endif // BIGIMAGE_H
