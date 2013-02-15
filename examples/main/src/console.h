
#ifndef CONSOLE_H
#define CONSOLE_H

#include "fltk3utils.h"
#include <boost/format.hpp>

class ConsoleView: public flu::Group {
    flu::TextDisplay * display;
    flu::MultilineInput * input;
    
  public:
    ConsoleView(int wx, int wy, int ww, int wh);
    
    virtual void TextEntered(const char * str);
    
    void Write(const char * str);
};


#endif // CONSOLE_H
