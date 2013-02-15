
#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <cfloat>

#include <boost/format.hpp>

#include <fltk3/fltk3.h>
#include <fltk3/Box.h>

#include "fltk3utils.h"

#include "console.h"
#include "opts_ui.h"
#include "ui_dims.h"

#include "filestore.h"
#include "image/bigimage.h"
#include "image/targa_io.h"

using namespace std;
using boost::format;


class UI_Stack {
    std::stack<flu::Group *> panes;
  public:
    UI_Stack() {}
    
    void PushPane(flu::Group * pane) {
        if(!panes.empty())
            panes.top()->hide();
        panes.push(pane);
        pane->show();
    }
    void PopPane() {
        if(!panes.empty()) {
            panes.top()->hide();
            panes.pop();
        }
        if(!panes.empty())
            panes.top()->show();
    }
};



class MainWindow: public flu::Window {
    bool isFullscreen = false;
    UI_Stack uiStack;
    ConsoleView * consoleView;
    OptionsUI * optionsUI;
  public:
    MainWindow(int wx, int wy, int ww, int wh, const char * label = nullptr);
};

MainWindow::MainWindow(int winx, int winy, int winw, int winh, const char * label):
    flu::Window(winx, winy, winw, winh, label)
{
    // flu::on_shortcut([]() -> int {cout << "shortcut!" << endl; return 1;});
    
    // callback([this] {
    //     if(fltk3::event() == fltk3::SHORTCUT && fltk3::event_key() == fltk3::EscapeKey) {
    //         uiStack.PopPane();
    //         return;
    //     }
    //     exit(0);
    // });
    
    int hmargin = 30;
    int ww = w() - hmargin*2;
    int wh = 20;
    int ym = 10;
    int wx = hmargin;
    int wy = 150;
    
    wh = 90;
    flu::Widget * box = new flu::Widget(wx+16, wy, ww-32, wh, "Testing...");
    box->box(fltk3::UP_BOX);
    box->labelfont(fltk3::BOLD + fltk3::ITALIC);
    box->labelsize(36);
    box->labeltype(fltk3::SHADOW_LABEL);
    box->on_enter([]() -> int {cout << "Enter!" << endl; return 1;});
    box->on_leave([]() -> int {cout << "Exit!" << endl; return 1;});
    wy += wh + ym;
    
    flu::Button * consoleBtn = new flu::Button(wx, wy, ww, kButtonH, "Show console");
    wy += kButtonH + ym;
    consoleBtn->callback([this] {uiStack.PushPane(consoleView);});
    
    flu::Button * optionsBtn = new flu::Button(wx, wy, ww, kButtonH, "Options");
    wy += kButtonH + ym;
    optionsBtn->callback([this] {uiStack.PushPane(optionsUI);});
    
    flu::CheckButton * fullscreenCB = new flu::CheckButton(wx, wy, ww, kButtonH, "Fullscreen");
    wy += kButtonH + ym;
    // fullscreenCB->callback([this] {
    //     if(isFullscreen)
    //         fullscreen_off();
    //     else
    //         fullscreen();
    //     isFullscreen = !isFullscreen;
    // });
    
    consoleView = new ConsoleView(0, 360, 512, 360);
    consoleView->hide();
    
    optionsUI = new OptionsUI(0, 360, 512, 360);
    optionsUI->hide();
    
    end();
    // resizable(this);
}


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
    // img.EachTile(&contexts[0], [](Context & ctx, BasicTile & ti){
    //     ti.EachPixel([&ctx](uint32_t & pix) {
    //         if(ctx.id < 4)
    //             pix = (ctx.id + 1)*63*0x00010000;
    //         else if(ctx.id < 8)
    //             pix = (ctx.id-3)*63*0x00000100;
    //         else if(ctx.id < 12)
    //             pix = (ctx.id-7)*63*0x00000001;
    //         else
    //             pix = (ctx.id-11)*63*0x00010101;
    //         pix |= 0xFF000000;
    //     });
    // });
    
    // Color by job order: 64 shades of red, green, and blue in repeating sequence
    std::atomic_uint_least32_t jobCtr(0);
    img.EachTile(&contexts[0], [&](Context & ctx, BasicTile & ti){
        uint32_t i = jobCtr++;
        uint32_t c = ((i*4 & 0xFF) << (((i/64) % 3)*8)) | 0xFF000000;
        ti.EachPixel([&](uint32_t & pix) {pix = c;});
    });
    
    
    uint32_t * pixels = new uint32_t[img.Width()*img.Height()];
    img.GetPixels<PixelTypeRGBA32>(pixels);
    
    TargaFileInfo tfile(img.Width(), img.Height(), 32);
    tfile.Write("test.tga", (uint8_t*)pixels);
    
    img.PrintInfo();
    
}

// #include <boost/interprocess/file_mapping.hpp>
// #include <boost/interprocess/mapped_region.hpp>
// using namespace boost::interprocess;

int main(int argc, char * argv[])
{
    // file_mapping m_file("calculon-0.1.tar.gz", read_write);
    // mapped_region region(m_file, read_write, 0, 1024);
    // 
    // cerr << format("address: %X\n")% region.get_address();
    // cerr << format("size: %u\n")% region.get_size();
    
    try {
        // TestFileStore();
        TestBigImage();
        // flu::initialize();
    }
    catch(std::exception & err) {
        cerr << "exception caught: " << err.what() << endl;
    }
    
    
    // MainWindow * window = new MainWindow(0, 0, 512, 720);
    // 
    // window->show();
    
    // return fltk3::run();
    return EXIT_SUCCESS;
}
