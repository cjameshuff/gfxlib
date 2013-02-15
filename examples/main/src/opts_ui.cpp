
#include "opts_ui.h"
#include "ui_dims.h"


OptionsUI::OptionsUI(int wx, int wy, int ww, int wh):
    flu::Group(wx, wy, ww, wh)
{
    int hmargin = 30;
    int vmargin = 20;
    int widgetWidth = w() - hmargin*2;
    int widgetHeight = 60;
    int x = hmargin;
    int y = 20;
    
    widgetHeight = 60;
    fltk3::Widget * box = new flu::Widget(x, y, widgetWidth, widgetHeight, "Options");
    box->box(fltk3::UP_BOX);
    box->labelfont(fltk3::BOLD + fltk3::ITALIC);
    box->labelsize(36);
    box->labeltype(fltk3::SHADOW_LABEL);
    y += widgetHeight;
    
    
    flu::CheckButton * fullscreenCB = new flu::CheckButton(wx, wy, ww, kButtonH, "Fullscreen");
    wy += kButtonH + vmargin;
    // fullscreenCB->callback([this] {
    //     if(isFullscreen)
    //         fullscreen_off();
    //     else
    //         fullscreen();
    //     isFullscreen = !isFullscreen;
    // });
    
    end();
    resizable(this);
}


