/*******************************************************************************
# Copyright (c) 2012 Christopher James Huff
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#******************************************************************************/
 
// 2D, 3D and 4D vector classes.
// 
// * and / operators are component-wise multiplication and division. Cross and
// dot products use named functions.
// If one vector is smaller than the other, the missing elements are considered
// to be zero.
// Operations on scalars are applied to each component:
//   vec3 + 1 is equivalent to vec3 + < 1, 1, 1>
// 
// Assignments can extend a smaller vector to a larger one, but do not truncate
// large vectors to smaller ones. (this includes combined operator-assignments)
// Assignments from scalars set all components of a vector to the value of the
// scalar.
// 
// Equality tests are only defined for vectors of identical size.

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include "misc.h"

namespace m3d {

// Forward declarations for 2D/3D/4D interoperability
template<typename T> struct tvec2;
template<typename T> struct tvec3;
template<typename T> struct tvec4;


// Template for 3D vectors
template<typename T>
struct tvec3 {
    typedef T vec_t;
    T x, y, z;
    
    // Default constructor does no initialization.
    tvec3<T>(): x(), y(), z() {}
    
    // Initialize from array.
    template<typename T2>
    explicit tvec3<T>(T2 arr[3]): x(arr[0]), y(arr[1]), z(arr[2]) {}
    
    // Initialize from scalar.
    template<typename T2>
    explicit tvec3<T>(const T2 & scal): x(scal), y(scal), z(scal) {}
    
    // Initialize with component values.
    template<typename T2, typename T3, typename T4>
    tvec3<T>(T2 xx, T3 yy, T4 zz): x(xx), y(yy), z(zz) {}
    
    // Initialize with existing 2D vector, setting z component to 0.
    template<typename T2>
    tvec3<T>(const tvec2<T2> & vec): x(vec.x), y(vec.y), z(0) {}
    
    // Initialize with existing 3D vector.
    template<typename T2>
    tvec3<T>(const tvec3<T2> & vec): x(vec.x), y(vec.y), z(vec.z) {}
    
    // Initialize with existing 3D vector and given length.
    template<typename T2, typename Length_T>
    tvec3<T>(const tvec3<T2> & vec, Length_T len): tvec3<T>(vec) {set_length(len);}
    
    
    template<typename T2>
    tvec3<T> & operator=(const T2 & rhs)
    {x = rhs; y = rhs; z = rhs; return *this;}
    
    template<typename T2>
    tvec3<T> & operator=(const tvec2<T2> & rhs)
    {x = rhs.x; y = rhs.y; z = 0; return *this;}
    
    template<typename T2>
    tvec3<T> & operator=(const tvec3<T2> & rhs)
    {x = rhs.x; y = rhs.y; z = rhs.z; return *this;}
    
    
    T & operator[](int i) {return (&x)[i];}
    
    
    template<typename T2>
    void get(T2 & xx, T2 & yy, T2 & zz) {xx = x; yy = y; zz = z;}
    
    template<typename T2>
    T2 * getv(const T2 arr[3]) const {arr[0] = x; arr[1] = y; arr[2] = z; return arr;}
    
    
    template<typename T2>
    tvec3<T> & set(T2 v) {x = y = z = v; return *this;}
    
    template<typename TX, typename TY, typename TZ>
    tvec3<T> & set(TX xx, TY yy, TZ zz) {x = xx; y = yy; z = zz; return *this;}
    
    template<typename T2>
    tvec3<T> & setv(const T2 arr[3]) {x = arr[0]; y = arr[1]; z = arr[2]; return *this;}
    
    
    // Compute the length.
    T length() const;
    
    // Compute the square of the length.
    T length_sqr() const;
    
    // Set the length without changing the direction
    // Negative "length" is acceptable, the direction of the resulting vector will be reversed.
    template<typename Length_T>
    tvec3<T> & set_length(Length_T len) {*this *= len/length(); return *this;}
    
    template<typename Length_T>
    tvec3<T> & set_length_sqr(Length_T lensq) {*this *= lensq/length_sqr(); return *this;}
    
    // Return a vector with the same direction as this, but with the given length.
    // Negative "length" is acceptable, the direction of the resulting vector will be reversed.
    template<typename Length_T>
    tvec3<T> with_length(Length_T len) const {return (*this)*(len/length());}
    
    template<typename Length_T>
    tvec3<T> with_length_sqr(Length_T lensq) const {return (*this)*(lensq/length_sqr());}
    
    
    template<typename T2>
    tvec3<T> difference(const tvec3<T2> & vec) const
        {return tvec3<T>(abs(x - vec.x), abs(y - vec.y), abs(z - vec.z));}
    
    tvec3<T> & normalize_eq() {return *this /= length();}
    tvec3<T> normalized() const {return *this/length();}
    
    
    // Dot product: cosine of the angle between the vectors, times the lengths of the two vectors.
    template<typename T2>
    T dot(const tvec3<T2> & vec) const {return x*vec.x + y*vec.y + z*vec.z;}
    
    T dot(T rhs_x, T rhs_y, T rhs_z) const {return x*rhs_x + y*rhs_y + z*rhs_z;}
    
    
    // Cross product
    template<typename T2>
    tvec3<T> cross(const tvec3<T2> & vec) const {
        return tvec3<T>(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
    }
    
    tvec3<T> cross(T rhs_x, T rhs_y, T rhs_z) const {
        return tvec3<T>(y*rhs_z - z*rhs_y, z*rhs_x - x*rhs_z, x*rhs_y - y*rhs_x);
    }
    
    // cross(*this, axis)
    tvec3<T> cross_x() const {return tvec3<T>(0, z,-y);}
    tvec3<T> cross_y() const {return tvec3<T>(-z, 0, x);}
    tvec3<T> cross_z() const {return tvec3<T>(y,-x, 0);}
    
    // cross(axis, *this)
    tvec3<T> x_cross() const {return tvec3<T>( 0,-z, y);}
    tvec3<T> y_cross() const {return tvec3<T>( z, 0,-x);}
    tvec3<T> z_cross() const {return tvec3<T>(-y, x, 0);}
    
    
    template<typename T2>
    tvec3<T> min(const tvec3<T2> & vec) const {
        return tvec3<T>(std::min(x, vec.x), std::min(y, vec.y), std::min(z, vec.z));
    }
    
    template<typename T2>
    tvec3<T> max(const tvec3<T2> & vec) const {
        return tvec3<T>(std::max(x, vec.x), std::max(y, vec.y), std::max(z, vec.z));
    }
    
    
    tvec3<T> rotated_x(double amt) const {
        double cosAng = cos(amt), sinAng = sin(amt);
        return tvec3<T>(x, y*cosAng + z*sinAng, -y*sinAng + z*cosAng);
    }
    tvec3<T> rotated_y(double amt) const {
        double cosAng = cos(amt), sinAng = sin(amt);
        return tvec3<T>(x*cosAng - z*sinAng, y, x*sinAng + z*cosAng);
    }
    tvec3<T> rotated_z(double amt) const {
        double cosAng = cos(amt), sinAng = sin(amt);
        return tvec3<T>(x*cosAng + y*sinAng, -x*sinAng + y*cosAng, z);
    }
    tvec3<T> rotated(double xx, double yy, double zz) const {
        return rotated_x(xx).rotated_y(yy).rotated_z(zz);
    }
    tvec3<T> rotated(const tvec3<T> & vec) const {return rotated(vec.x, vec.y, vec.z);}
    
    
    tvec3<T> & rotate_eq_x(double amt) {
        double cosAng = cos(amt); double sinAng = sin(amt); T tmpy = y;
        y = y*cosAng + z*sinAng; z = -tmpy*sinAng + z*cosAng;
        return *this;
    }
    tvec3<T> & rotate_eq_y(double amt) {
        double cosAng = cos(amt); double sinAng = sin(amt); T tmpx = x;
        x = x*cosAng - z*sinAng; z = tmpx*sinAng + z*cosAng;
        return *this;
    }
    tvec3<T> & rotate_eq_z(double amt) {
        double cosAng = cos(amt); double sinAng = sin(amt); T tmpx = x;
        x = x*cosAng + y*sinAng; y = -tmpx*sinAng + y*cosAng;
        return *this;
    }
    tvec3<T> & rotate_eq(double xx, double yy, double zz) {
        rotate_eq_x(xx); rotate_eq_y(yy); rotate_eq_z(zz);
        return *this;
    }
    tvec3<T> & rotate_eq(const tvec3<T> & vec) {return rotate_eq(vec.x, vec.y, vec.z);}
    
    // Returns a vector obtained by rotating this vector around
    // given axis by angle in radians.
    tvec3<T> rotated(const tvec3<T> & axis, double th) const {
        tvec3<T> v(axis.with_length(sin(th/2)));
        T w = cos(th/2);
        T ww = w*w;
        T xx = v.x*v.x, yy = v.y*v.y, zz = v.z*v.z;
        T wx = w*v.x,   wy = w*v.y,   wz = w*v.z;
        T xy = v.x*v.y, xz = v.x*v.z, yz = v.y*v.z;
        return tvec3<T>((ww + xx - yy - zz)*x +         2*(xy - wz)*y +         2*(xz + wy)*z,
                                2*(xy + wz)*x + (ww - xx + yy - zz)*y +         2*(yz - wx)*z,
                                2*(xz - wy)*x +         2*(yz + wx)*y + (ww - xx - yy + zz)*z);
    }
    
    // Rotates the vector around given axis by angle in radians
    tvec3<T> & rotate_eq(const tvec3<T> & axis, double ang) {
        return *this = this->rotated(axis, ang);
    }
    
    // The following comparison operators are defined to give vectors a useful
    // sorting order. They have no other real use.
    bool operator<(const tvec3<T> & rhs) const {
        if(x < rhs.x) return true;
        else if(x == rhs.x)
        {
            if(y < rhs.y) return true;
            else if(y == rhs.y && z < rhs.z)
                return true;
        }
        return false;
    }
    bool operator>(const tvec3<T> & rhs) const {
        if(x > rhs.x) return true;
        else if(x == rhs.x)
        {
            if(y > rhs.y) return true;
            else if(y == rhs.y && z > rhs.z)
                return true;
        }
        return false;
    }
    bool operator<=(const tvec3<T> & rhs) const {return !(*this > rhs);}
    bool operator>=(const tvec3<T> & rhs) const {return !(*this < rhs);}
    
    // Computes the normal of a plane defined by points a, b, and c facing toward point p
    template<typename T2>
    friend tvec3<T2> normal_facing(const tvec3<T2> & a, const tvec3<T2> & b, const tvec3<T2> & c,
                                      const tvec3<T2> & p);
    
    std::string string() const {std::ostringstream strm; strm << *this; return strm.str();}
};

template<typename T2>
T2 vdot(const tvec3<T2> & lhs, const tvec3<T2> & rhs) {return lhs.dot(rhs);}

template<typename T2>
tvec3<T2> vnormalized(const tvec3<T2> & vect) {return vect.normalized();}

template<typename T2>
tvec3<T2> & vnormalize_eq(tvec3<T2> & vect) {return vect.normalize_eq();}

template<typename T2>
T2 vlength(const tvec3<T2> & vect) {return vect.length();}

template<typename T2>
T2 vlength_sqr(const tvec3<T2> & vect) {return vect.length_sqr();}

template<typename T2>
tvec3<T2> vcross(const tvec3<T2> & lhs, const tvec3<T2> & rhs) {
    return lhs.cross(rhs);
}
template<typename T2>
tvec3<T2> x_cross(const tvec3<T2> & vect) {return vect.x_cross();}

template<typename T2>
tvec3<T2> y_cross(const tvec3<T2> & vect) {return vect.y_cross();}

template<typename T2>
tvec3<T2> z_cross(const tvec3<T2> & vect) {return vect.z_cross();}

template<typename T2>
std::ostream & operator<<(std::ostream & strm, const tvec3<T2> & v) {
    return strm << "<" << v.x << ", " << v.y << ", " << v.z << ">";
}

template<typename T2>
std::istream & operator>>(std::istream & istrm, tvec3<T2> & v)
{
    char ch;
    while(istrm && isspace(istrm.peek())) istrm.get(ch);
    if(istrm.peek() == '<')
    {
        istrm.get(ch);
        istrm >> v.x;
        while(istrm && isspace(istrm.peek())) istrm.get(ch);// eat whitespace
        istrm.get(ch);//get ','
        istrm >> v.y;
        while(istrm && isspace(istrm.peek())) istrm.get(ch);// eat whitespace
        istrm.get(ch);//get ','
        istrm >> v.z;
        while(istrm && isspace(istrm.peek())) istrm.get(ch);// eat whitespace
        istrm.get(ch);//get '>'
    }
    else
    {
        // get single-number assignment
        istrm >> v.x;
        v.y = v.x;
        v.z = v.x;
    }
    return istrm;
}

template<typename T>
inline tvec3<T> normal_facing(const tvec3<T> & a, const tvec3<T> & b, const tvec3<T> & c, const tvec3<T> & p)
{
    tvec3<T> norm((b - a).cross(c - a).normalized());
    
    if(norm.dot(p - a) < 0)
        return -norm;
    else
        return norm;
}


template<typename T>
T tvec3<T>::length() const {return sqrt(x*x + y*y + z*z);}

template<>
inline float tvec3<float>::length() const {return sqrtf(x*x + y*y + z*z);}


// Compute the square of the length.
template<typename T>
T tvec3<T>::length_sqr() const {return x*x + y*y + z*z;}

//**************************************************
// TODO: move to geometry include file
template<typename T>
inline tvec3<T> InSphere(const tvec3<T> & pt, const tvec3<T> & center, double radius)
{
    return vlength_sqr(pt - center) < radius*radius;
}

template<typename T>
inline tvec3<T> InBox(const tvec3<T> & pt, const tvec3<T> & p1, const tvec3<T> & p2)
{
    return pt.x >= p1.x && pt.x <= p2.x &&
           pt.y >= p1.y && pt.y <= p2.y &&
           pt.z >= p1.z && pt.z <= p2.z;
}

template<typename T>
inline tvec3<T> InPlane(const tvec3<T> & pt, const tvec3<T> & orig, const tvec3<T> & norm)
{
    return dot(pt - orig, norm) < 0;
}

//**************************************************
//**************************************************

template<typename T>
struct tvec2 {
    typedef T vec_t;
    T x, y;
    
    // Default constructor does no initialization.
    tvec2<T>(): x(), y() {}
    
    // Initialize from array.
    template<typename T2>
    explicit tvec2<T>(T2 arr[2]): x(arr[0]), y(arr[1]) {}
    
    // Initialize from scalar.
    template<typename T2>
    explicit tvec2<T>(T2 scal): x(scal), y(scal) {}
    
    // Initialize with component values.
    template<typename T2, typename T3>
    tvec2<T>(T2 xx, T3 yy): x(xx), y(yy) {}
    
    // Initialize with existing vector.
    template<typename T2>
    tvec2<T>(const tvec2<T2> & vec): x(vec.x), y(vec.y) {}
    
    template<typename T2>
    tvec2<T>(const tvec3<T2> & vec): x(vec.x), y(vec.y) {}
    
    // Initialize with existing vector and the given length.
    template<typename T2, typename Length_T>
    tvec2<T>(const tvec2<T2> & vec, Length_T len): x(vec.x), y(vec.y) {set_length(len);}
    
    
    template<typename T2>
    tvec2<T> & operator=(const T2 & rhs)
    {x = rhs; y = rhs; return *this;}

    template<typename T2>
    tvec2<T> & operator=(const tvec2<T2> & rhs)
    {x = rhs.x; y = rhs.y; return *this;}
    
    
    // Compute the length.
    T length() const {return sqrt(x*x + y*y);}
    
    // Compute the square of the length.
    T length_sqr() const {return x*x + y*y;}
    
    // Set the length without changing the direction.
    // Negative "length" is acceptable, the direction of the resulting vector will be reversed.
    template<typename Length_T>
    tvec2<T> & set_length(Length_T len) {*this *= len/length(); return *this;}
    
    template<typename Length_T>
    tvec2<T> & set_length_sqr(Length_T lensq) {*this *= lensq/length_sqr(); return *this;}
    
    
    // Return a vector with the same direction as this, but with the given length.
    // Negative "length" is acceptable, the direction of the resulting vector will be reversed.
    template<typename Length_T>
    tvec2<T> with_length(Length_T len) {return (*this)*(len/length());}
    
    template<typename Length_T>
    tvec2<T> with_length_sqr(Length_T lensq) {return (*this)*(lensq/length_sqr());}
    
    
    // Length comparison functions.
    // Handier than typing myVec.length_sqr() OP len*len all the time.
    template<typename Length_T>
    bool length_less(Length_T len) const {return length_sqr() < len*len;}
    
    template<typename Length_T>
    bool length_lesseq(Length_T len) const {return length_sqr() <= len*len;}
    
    template<typename Length_T>
    bool length_gthan(Length_T len) const {return length_sqr() > len*len;}
    
    template<typename Length_T>
    bool length_gthaneq(Length_T len) const {return length_sqr() >= len*len;}
    
    
    bool in_rect(const tvec2<T> & p1, const tvec2<T> & p2) const {
        return x > p1.x && x < p2.x && y > p1.y && y < p2.y;
    }
    bool in_rect(T p1x, T p1y, T p2x, T p2y) const {
        return x > p1x && x < p2x && y > p1y && y < p2y;
    }
    
    tvec2<T> difference(const tvec2<T> & vec) const
        {return tvec2<T>(fabs(x - vec.x), fabs(y - vec.y));}
    
    tvec2<T> & normalize_eq() {return *this /= length();}
    tvec2<T> normalized() const {return *this/length();}
    
    
    T dot(const tvec2<T> & vec) const {return x*vec.x + y*vec.y;}
    
    T dot(T rhs_x, T rhs_y) const {return x*rhs_x + y*rhs_y;}
    
    
    tvec2<T> rotated(double amt) {
        double cosAng = cos(amt), sinAng = sin(amt);
        return tvec2<T>(x*cosAng + y*sinAng, -x*sinAng + y*cosAng);
    }
    
    tvec2<T> & rotate_eq(double amt) {return *this = rotated(amt);}
    
    
    // The following comparison operators are defined to give vectors a useful
    // sorting order. They have no other real use.
    // Note that due to floating point precision, two vectors are unlikely to
    // evaluate as equal unless they are specifically set to the same value.
    bool operator<(const tvec3<T> & rhs) const {
        if(x < rhs.x) return true;
        else if(x == rhs.x && y < rhs.y) return true;
        else return false;
    }
    bool operator>(const tvec3<T> & rhs) const {
        if(x > rhs.x) return true;
        else if(x == rhs.x && y > rhs.y) return true;
        else return false;
    }
    bool operator<=(const tvec3<T> & rhs) const {return !(*this > rhs);}
    bool operator>=(const tvec3<T> & rhs) const {return !(*this < rhs);}
    
    
    T & operator[](int i) {return (&x)[i];}
    
    template<typename TX, typename TY>
    void get(TX & xx, TY & yy) {xx = x; yy = y;}
    
    template<typename T2>
    tvec2<T> & set(T2 v) {x = y = v; return *this;}
    
    template<typename T2, typename T3>
    tvec2<T> & set(T2 xx, T3 yy) {x = xx; y = yy; return *this;}
    
    
    template<typename T2>
    tvec2<T> min(const tvec2<T2> & vec) const {
        return tvec2<T>(std::min(x, vec.x), std::min(y, vec.y));
    }
    
    template<typename T2>
    tvec2<T> & mineq(const tvec2<T2> & vec) {
        x = std::min(x, vec.x); y = std::min(y, vec.y);
        return *this;
    }
    
    template<typename T2>
    tvec2<T> max(const tvec2<T2> & vec) const {
        return tvec2<T>(std::max(x, vec.x), std::max(y, vec.y));
    }
    
    template<typename T2>
    tvec2<T> & maxeq(const tvec2<T2> & vec) {
        x = std::max(x, vec.x); y = std::max(y, vec.y);
        return *this;
    }
    
//    std::string string() const {char buf[255]; sprintf(buf, "<%f, %f>", x, y); return std::string(buf);}
    std::string string() const {std::ostringstream strm; strm << *this; return strm.str();}
};


template<typename T2>
T2 vlength(const tvec2<T2> & vec) {return vec.length();}

template<typename T2>
T2 vlength_sqr(const tvec2<T2> & vec) {return vec.length_sqr();}


template<typename T2>
T2 vdot(const tvec2<T2> & lhs, const tvec2<T2> & rhs) {return lhs.dot(rhs);}

template<typename T>
tvec2<T> min(const tvec2<T> & a, const tvec2<T> & b) {return a.min(b);}

template<typename T>
tvec2<T> max(const tvec2<T> & a, const tvec2<T> & b) {return a.max(b);}


template<typename T2>
std::ostream & operator<<(std::ostream & strm, const tvec2<T2> & v) {
    return strm << "<" << v.x << ", " << v.y << ">";
}

template<typename T2>
std::istream & operator>>(std::istream & istrm, tvec2<T2> & v) {
    char ch;
    while(istrm && isspace(istrm.peek())) istrm.get(ch);
    if(istrm.peek() == '<')
    {
        istrm.get(ch);
        istrm >> v.x;
        while(istrm && isspace(istrm.peek())) istrm.get(ch);// eat whitespace
        istrm.get(ch);//get ','
        istrm >> v.y;
        while(istrm && isspace(istrm.peek())) istrm.get(ch);// eat whitespace
        istrm.get(ch);//get '>'
    }
    else
    {
        // get single-number assignment
        istrm >> v.x;
        v.y = v.x;
    }
    return istrm;
}

//**************************************************
template<typename T>
struct tvec4 {
    typedef T vec_t;
    T x, y, z, w;
    
    // Default constructor does no initialization.
    tvec4<T>(): x(), y(), z(), w() {}
    
    // Initialize from array.
    template<typename T2>
    explicit tvec4<T>(T2 arr[4]): x(arr[0]), y(arr[1]), z(arr[2]), w(arr[3]) {}
    
    // Initialize from scalar.
    template<typename T2>
    explicit tvec4<T>(T2 scal): x(scal), y(scal), z(scal), w(scal) {}
    
    // Initialize with component values.
    template<typename T2>
    tvec4<T>(T2 xx, T2 yy, T2 zz, T2 ww): x(xx), y(yy), z(zz), w(ww) {}
    
    // Initialize with existing vector.
    template<typename T2>
    tvec4<T>(const tvec4<T2> & vec): x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
    
    template<typename T2>
    tvec4<T>(const tvec3<T2> & vec, T2 ww): x(vec.x), y(vec.y), z(vec.z), w(ww) {}
    
    // Initialize with existing vector and the given length.
    template<typename T2>
    tvec4<T>(const tvec4<T2> & vec, T2 len): x(vec.x), y(vec.y), z(vec.z), w(vec.w) {set_length(len);}
    
    
    template<typename T2>
    tvec4<T> & operator=(const T2 & rhs)
    {x = rhs; y = rhs; z = rhs; w = rhs; return *this;}

    template<typename T2>
    tvec4<T> & operator=(const tvec2<T2> & rhs)
    {x = rhs.x; y = rhs.y; z = 0;  w = 0; return *this;}

    template<typename T2>
    tvec4<T> & operator=(const tvec3<T2> & rhs)
    {x = rhs.x; y = rhs.y; z = rhs.z; w = 0; return *this;}

    template<typename T2>
    tvec4<T> & operator=(const tvec4<T2> & rhs)
    {x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.t; return *this;}
    
    
    T & operator[](int i) {return (&x)[i];}
    
    template<typename T2>
    void get(T2 & xx, T2 & yy, T2 & zz, T2 & ww) {xx = x; yy = y; zz = z; ww = w;}
    
    template<typename T2>
    tvec4<T> & set(T2 v) {x = y = z = w = v; return *this;}
    
    template<typename T2>
    tvec4<T> & set(T2 xx, T2 yy, T2 zz, T2 ww) {x = xx; y = yy; z = zz; w = ww; return *this;}
    
    
    // Compute the length.
    T length() const {return sqrt(x*x + y*y + z*z + w*w);}
    
    // Compute the square of the length.
    T length_sqr() const {return x*x + y*y + z*z + w*w;}
    
    T max() const {return std::max(std::max(x, y), std::max(z, w));}
    T min() const {return std::min(std::min(x, y), std::min(z, w));}
    
    T sum() const {return x + y + z + w;}
    T prod() const {return x*y*z*w;}
    
    // Set the length without changing the direction.
    // Negative "length" is acceptable, the direction of the resulting vector will be reversed.
    template<typename Length_T>
    tvec4<T> & set_length(Length_T len) {*this *= len/length(); return *this;}
    
    template<typename Length_T>
    tvec4<T> & set_length_sqr(Length_T lensq) {*this *= lensq/length_sqr(); return *this;}
    
    
    // Return a vector with the same direction as this, but with the given length.
    // Negative "length" is acceptable, the direction of the resulting vector will be reversed.
    template<typename Length_T>
    tvec4<T> with_length(Length_T len) {return (*this)*(len/length());}
    
    template<typename Length_T>
    tvec4<T> with_length_sqr(Length_T lensq) {return (*this)*(lensq/length_sqr());}
    
    
    tvec4<T> difference(const tvec4<T> & vec) const
        {return tvec4<T>(fabs(x - vec.x), fabs(y - vec.y), fabs(z - vec.z), fabs(w - vec.w));}
    
    
    tvec4<T> & normalize_eq() {return *this /= length();}
    tvec4<T> normalized() const {return *this/length();}
    
    
    T dot(const tvec4<T> & vec) const {return x*vec.x + y*vec.y + z*vec.z + w*vec.w;}
    T dot(T rhs_x, T rhs_y, T rhs_z, T rhs_w) const {return x*rhs_x + y*rhs_y + z*rhs_z + w*rhs_w;}
    
    
    template<typename T2>
    tvec4<T> min(const tvec4<T2> & vec) const {
        return tvec4<T>(std::min(x, vec.x), std::min(y, vec.y),
                        std::min(z, vec.z), std::min(w, vec.w));
    }
    
    template<typename T2>
    tvec4<T> & mineq(const tvec4<T2> & vec) {
        x = std::min(x, vec.x); y = std::min(y, vec.y);
        z = std::min(z, vec.z); w = std::min(w, vec.w);
        return *this;
    }
    
    template<typename T2>
    tvec4<T> max(const tvec4<T2> & vec) const {
        return tvec4<T>(std::max(x, vec.x), std::max(y, vec.y),
                        std::max(z, vec.z), std::max(w, vec.w));
    }
    
    template<typename T2>
    tvec4<T> & maxeq(const tvec4<T2> & vec) {
        x = std::max(x, vec.x); y = std::max(y, vec.y);
        z = std::max(z, vec.z); w = std::max(w, vec.w);
        return *this;
    }
    
//    std::string string() const {char buf[255]; sprintf(buf, "<%f, %f>", x, y); return std::string(buf);}
    std::string string() const {std::ostringstream strm; strm << *this; return strm.str();}
};


template<typename T2>
T2 vlength(const tvec4<T2> & vec) {return vec.length();}

template<typename T2>
T2 vlength_sqr(const tvec4<T2> & vec) {return vec.length_sqr();}

template<typename T2>
T2 vdot(const tvec4<T2> & lhs, const tvec4<T2> & rhs) {return lhs.dot(rhs);}

template<typename T>
tvec4<T> min(const tvec4<T> & a, const tvec4<T> & b) {return a.min(b);}

template<typename T>
tvec4<T> max(const tvec4<T> & a, const tvec4<T> & b) {return a.max(b);}


template<typename T2>
std::ostream & operator<<(std::ostream & strm, const tvec4<T2> & v) {
    return strm << "<" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ">";
}

template<typename T2>
std::istream & operator>>(std::istream & istrm, tvec4<T2> & v) {
    char ch;
    while(istrm && isspace(istrm.peek())) istrm.get(ch);
    if(istrm.peek() == '<')
    {
        istrm.get(ch);
        istrm >> v.x;
        while(istrm && isspace(istrm.peek())) istrm.get(ch);// eat whitespace
        istrm.get(ch);//get ','
        istrm >> v.y;
        while(istrm && isspace(istrm.peek())) istrm.get(ch);// eat whitespace
        istrm.get(ch);//get ','
        istrm >> v.z;
        while(istrm && isspace(istrm.peek())) istrm.get(ch);// eat whitespace
        istrm.get(ch);//get ','
        istrm >> v.w;
        while(istrm && isspace(istrm.peek())) istrm.get(ch);// eat whitespace
        istrm.get(ch);//get '>'
    }
    else
    {
        // get single-number assignment
        istrm >> v.x;
        v.y = v.x;
    }
    return istrm;
}

//**************************************************

typedef tvec2<double> vec2;
typedef tvec2<float> svec2;
typedef tvec2<int> ivec2;

typedef tvec3<double> vec3;
typedef tvec3<float> svec3;
typedef tvec3<int> ivec3;

typedef tvec4<double> vec4;
typedef tvec4<float> svec4;
typedef tvec4<int> ivec4;

}// namespace m3d

#include "vecops.h"

#endif // VEC3_H
