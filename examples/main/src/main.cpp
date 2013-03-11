
#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <cfloat>

#include <boost/format.hpp>
#include <random>
#include <functional>

#include "filestore.h"
#include "image/bigimage.h"
#include "image/targa_io.h"

using namespace std;
using boost::format;

using filestore::FileStore;
using filestore::loc_t;

std::mt19937 rng;

void SetAll(FileStore & fs, uint8_t val)
{
    uint8_t * mem = (uint8_t *)fs.Data();
    size_t size = fs.DataSize();
}

void SetFreeMem(FileStore & fs, uint8_t val)
{
    
}

void ConsistencyCheck(FileStore & fs)
{
    fs.Reset();
    size_t requested = 0;// bytes requested
    size_t allocated = 0;// bytes used for allocations
    size_t reserved = 0;// total bytes used
    size_t freebytes = 0;// bytes available for allocations
    std::vector<loc_t> allocs;
    
    std::uniform_int_distribution<int> ui64(1, 256);
    auto rsize = [&]{return ui64(rng)*8;};
    
    cout << format("allocation test\n");
    // Make some allocations
    for(int j = 0; j < 10240; ++j)
    {
        size_t s = rsize();
        loc_t l = fs.Alloc(s);
        size_t bs = filestore::BlockBytes(l);
        
        if(bs < s) {
            cout << format("Allocated block too small! Requested %u, got %u\n") % s % bs;
            return;
        }
        
        allocs.push_back(l);
        requested += s;
        allocated += bs;
    }
    
    // Count free memory, verify sum of free + allocated blocks
    reserved = fs.DataSize();
    freebytes = fs.CountFreeBytes();
    
    if(allocated + freebytes != reserved)
        cout << format("Inconsistent sizes!\n");
    
    cout << format("allocated block/free block overlap test\n");
    // Fill all allocations with 0xFF
    // Zero free memory
    // Check allocations for damage
    
    for(auto & l : allocs)
        memset(fs.Get<uint8_t>(l), 0xFF, filestore::BlockBytes(l));
    
    fs.ZeroFreeMem();
    
    for(auto & l : allocs) {
        uint8_t * bytes = fs.Get<uint8_t>(l);
        for(size_t j = 0, n = filestore::BlockBytes(l); j < n; ++j)
        {
            if(bytes[j] != 0xFF)
            {
                cout << format("Overlap detected between allocated and free blocks\n");
                return;
            }
        }
    }
    
    cout << format("allocated block/allocated block overlap test\n");
    // Decrement each byte of all allocations
    // Check for allocations not containing 0xFE
    
    for(auto & l : allocs) {
        uint8_t * bytes = fs.Get<uint8_t>(l);
        for(size_t j = 0, n = filestore::BlockBytes(l); j < n; ++j)
            --(bytes[j]);
    }
    
    for(auto & l : allocs) {
        uint8_t * bytes = fs.Get<uint8_t>(l);
        for(size_t j = 0, n = filestore::BlockBytes(l); j < n; ++j)
        {
            if(bytes[j] != 0xFE)
            {
                cout << format("Overlap detected between allocated blocks\n");
                return;
            }
        }
    }
    
    cout << format("Free memory recount\n");
    // Recount free memory, verify no damage to freelist structures
    size_t freebytes2 = fs.CountFreeBytes();
    if(freebytes != freebytes2)
        cout << format("Freelist damage detected!\n");
    
    cout << format("requested: %s\n")% filestore::SizeToS(requested);
    cout << format("allocated: %s\n")% filestore::SizeToS(allocated);
    cout << format("    waste: %s (%0.2f %%)\n")% filestore::SizeToS(allocated - requested) % ((100.0*(allocated - requested))/allocated);
    cout << format(" reserved: %s\n")% filestore::SizeToS(reserved);
    cout << format("freebytes: %s\n")% filestore::SizeToS(freebytes);
}


void TestFileStore()
{
    std::array<uint8_t, 1024*64> scratchdata;
    
    for(int i = 0; i < scratchdata.size(); ++i)
        scratchdata[i] = i;
    
    FileStore fs;
    fs.Create("filestore/");
    ConsistencyCheck(fs);
    
    // std::array<loc_t, 16> allocs;
    // for(auto & l : allocs)
    //     l = fs.Alloc(64);
    
    // for(auto & l : allocs)
    //     cerr << format("%u:%u\n")% filestore::BlockBytes(l) % filestore::FileOffset(l);
    
    // std::array<FileStore::id_t, 8> objects;
    // for(auto & o : objects)
    //     o = fs.New();
    // 
    // fs.Log();
    // 
    // cerr << format("********************************************\n");
    // cerr << format("\nWriting objects\n");
    // for(auto & o : objects)
    //     fs.Set(o, &scratchdata[0], 16);
    // 
    // fs.Log();
    // 
    // cerr << format("********************************************\n");
    // cerr << format("\nModifying objects\n");
    // for(auto & o : objects)
    //     fs.Set(o, &scratchdata[1], 16);
    // 
    // fs.Log();
    // 
    // cerr << format("********************************************\n");
    // cerr << format("\nRewriting objects\n");
    // for(auto & o : objects)
    //     fs.Set(o, &scratchdata[2], 32);
    // 
    // fs.Log();
}

using bigimage::BigImage;
using bigimage::kNThreads;
using bigimage::ImageType;
using bigimage::PixelTypeU32;
using bigimage::PixelTypeRGBA32;
using bigimage::PixelTypeRGBAf;
using bigimage::TileBlockManager;
using bigimage::TileInfo;

typedef BigImage<ImageType<PixelTypeRGBA32, TileBlockManager>> BasicImage;
typedef BasicImage::TileInfo BasicTile;

void WriteImage(BasicImage & img, const std::string & fname)
{
    uint32_t * pixels = new uint32_t[img.Width()*img.Height()];
    
    img.GetPixels<PixelTypeRGBA32>(pixels);
    
    TargaFileInfo tfile(img.Width(), img.Height(), 32);
    tfile.Write(fname, (uint8_t*)pixels);
    
    delete pixels;
}



void TestBigImage()
{
    // bigimage::Workspace work("filestore/data");
    
    // BigImage<ImageTypeRGBA32> img(1024, 1024, "");
    BasicImage img(1024, 1024, "bigimage.work");
    // BasicImage img(1024, 1024, "");
    
    struct Context {int id;};
    std::array<Context, kNThreads> contexts;
    int id = 0;
    for(auto & ctx : contexts) {
        ctx.id = id++;
    }
    
    // Color by thread
    img.EachTile(&contexts[0], [](Context & ctx, BasicTile & ti){
        uint32_t c;
        if(ctx.id < 4)
            c = (ctx.id + 1)*63*0x00010000;
        else if(ctx.id < 8)
            c = (ctx.id-3)*63*0x00000100;
        else if(ctx.id < 12)
            c = (ctx.id-7)*63*0x00000001;
        else
            c = (ctx.id-11)*63*0x00010101;
        c |= 0xFF000000;
        ti.EachPixel([&](uint32_t & pix) {pix = c;});
    });
    WriteImage(img, "threadtiles.tga");
    
    // Color by job order: 64 shades of red, green, and blue in repeating sequence
    std::atomic_uint_least32_t jobCtr(0);
    img.EachTile(&contexts[0], [&](Context & ctx, BasicTile & ti){
        uint32_t i = jobCtr++;
        uint32_t c = ((i*4 & 0xFF) << (((i/64) % 3)*8)) | 0xFF000000;
        ti.EachPixel([&](uint32_t & pix) {pix = c;});
    });
    
    WriteImage(img, "jobordertiles.tga");
    
    img.PrintInfo();
}


int main(int argc, char * argv[])
{
    try {
        TestFileStore();
        // TestBigImage();
    }
    catch(std::exception & err) {
        cerr << "exception caught: " << err.what() << endl;
    }
    return EXIT_SUCCESS;
}
