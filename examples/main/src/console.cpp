
#include "console.h"
#include <boost/format.hpp>

using namespace std;
using boost::format;


ConsoleView::ConsoleView(int wx, int wy, int ww, int wh):
    flu::Group(wx, wy, ww, wh)
{
    int hmargin = 30;
    int vmargin = 20;
    int widgetWidth = w() - hmargin*2;
    int widgetHeight = 20;
    int x = hmargin;
    int y = 20;
    
    widgetHeight = h() - 20 - vmargin*2;
    display = new flu::TextDisplay(x, y, widgetWidth, widgetHeight);
    display->buffer(new fltk3::TextBuffer());
    display->textfont(fltk3::COURIER);
    display->textsize(12);
    y += widgetHeight;
    
    widgetHeight = 20;
    input = new flu::MultilineInput(x, y, widgetWidth, widgetHeight);
    input->callback([this] {
        Write(input->value());
        TextEntered(input->value());
        input->value("");
    });
    input->when(fltk3::WHEN_ENTER_KEY_ALWAYS);
    input->textfont(fltk3::COURIER);
    input->textsize(12);
    
    end();
    resizable(this);
    
    on_show([this]()->int {
        input->take_focus();
        return fltk3::Group::handle(fltk3::SHOW);
    });
}

void ConsoleView::TextEntered(const char * str)
{
    cout << (format("Console input: %s\n")% str);
}

void ConsoleView::Write(const char * str)
{
    if(strlen(str) > 0)
    {
        display->buffer()->append("\n");
        display->buffer()->append(str);
        size_t buflen = display->buffer()->length();
        // Just try to scroll to last character
        display->scroll(display->count_lines(0, buflen, true), 0);
    }
}
