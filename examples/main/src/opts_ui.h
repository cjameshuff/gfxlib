
#ifndef OPTS_UI_H
#define OPTS_UI_H

#include "fltk3utils.h"
#include <boost/format.hpp>

struct Options {
    bool isFullscreen = false;
};

class OptionsUI: public flu::Group {
  protected:
    
  public:
    OptionsUI(int wx, int wy, int ww, int wh);
};

#endif // OPTS_UI_H
