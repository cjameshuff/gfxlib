
#ifndef RECT_H
#define RECT_H

// *****************************************************************************
// Rect
// *****************************************************************************

struct Rect {
    int32_t x, y, w, h;
    
    Rect() {}
    Rect(int32_t _x, int32_t _y, int32_t _w, int32_t _h): x(_x), y(_y), w(_w), h(_h) {}
    Rect(const Rect & rhs): x(rhs.x), y(rhs.y), w(rhs.w), h(rhs.h) {}
    
    bool Overlaps(int32_t _x, int32_t _y) const {
        return (_x >= x) && (_x < x + w) &&
               (_y >= y) && (_y < y + h);
    }
    bool Overlaps(int32_t _x, int32_t _y, int32_t _w, int32_t _h) const {return Overlaps(Rect(_x, _y, _w, _h));}
    bool Overlaps(const Rect & r) const {
        return !(r.x >= x + w || r.x + r.w <= x ||
                 r.y >= y + h || r.y + r.h <= y);
    }
    
    Rect Intersect(int32_t _x, int32_t _y, int32_t _w, int32_t _h) const {return Intersect(Rect(_x, _y, _w, _h));}
    Rect Intersect(const Rect & r) const {
        if(Overlaps(r)) {
            int32_t _x = std::max(x, r.x), _y = std::max(y, r.y);
            int32_t _w = std::min(x + w, r.x + r.w) - _x, _h = std::min(y + h, r.y + r.h) - _y;
            return Rect(_x, _y, _w, _h);
        }
        else {
            return Rect(0, 0, 0, 0);
        }
    }
    bool Intersect(Rect & result, const Rect & r) const {
        if(Overlaps(r)) {
            int32_t _x = std::max(x, r.x), _y = std::max(y, r.y);
            int32_t _w = std::min(x + w, r.x + r.w) - _x, _h = std::min(y + h, r.y + r.h) - _y;
            result = Rect(_x, _y, _w, _h);
            return true;
        }
        else {
            return false;
        }
    }
};
#endif // RECT_H
