
#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <cfloat>

#include <boost/format.hpp>

#include "filestore.h"
#include "image/bigimage.h"
#include "image/targa_io.h"

using namespace std;
using boost::format;


void TestFileStore()
{
    using filestore::FileStore;
    std::array<uint8_t, 1024*64> scratchdata;
    
    for(int i = 0; i < scratchdata.size(); ++i)
        scratchdata[i] = i;
    
    FileStore fs("filestore/data");
    
    std::array<FileStore::id_t, 8> objects;
    for(auto & o : objects)
        o = fs.New();
    
    fs.Log();
    
    cerr << format("********************************************\n");
    cerr << format("\nWriting objects\n");
    for(auto & o : objects)
        fs.Set(o, &scratchdata[0], 16);
    
    fs.Log();
    
    cerr << format("********************************************\n");
    cerr << format("\nModifying objects\n");
    for(auto & o : objects)
        fs.Set(o, &scratchdata[1], 16);
    
    fs.Log();
    
    cerr << format("********************************************\n");
    cerr << format("\nRewriting objects\n");
    for(auto & o : objects)
        fs.Set(o, &scratchdata[2], 32);
    
    fs.Log();
    fs.GC();
    fs.GC();
    fs.Log();
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
    
    struct Context {int id;};
    std::array<Context, kNThreads> contexts;
    int id = 0;
    for(auto & ctx : contexts) {
        ctx.id = id++;
    }
    
    // Color by thread
    img.EachTile(&contexts[0], [](Context & ctx, BasicTile & ti){
        ti.EachPixel([&ctx](uint32_t & pix) {
            if(ctx.id < 4)
                pix = (ctx.id + 1)*63*0x00010000;
            else if(ctx.id < 8)
                pix = (ctx.id-3)*63*0x00000100;
            else if(ctx.id < 12)
                pix = (ctx.id-7)*63*0x00000001;
            else
                pix = (ctx.id-11)*63*0x00010101;
            pix |= 0xFF000000;
        });
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
        // TestFileStore();
        TestBigImage();
    }
    catch(std::exception & err) {
        cerr << "exception caught: " << err.what() << endl;
    }
    return EXIT_SUCCESS;
}
