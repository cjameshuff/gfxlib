// Copyright (C) 2012 Christopher James Huff
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef QUATERNION_H
#define QUATERNION_H

#include <algorithm>
#include <iostream>
#include "vmath.h"

#ifdef __SSE4_1__
#include "smmintrin.h"
#endif

//----------------------------------------------------------------------------------------------------

template<typename T>
class tquat {
  private:
    vec4<T> v;
    
  public:
    tquat(): v(0, 0, 0, 1) {}
    tquat(T bb, T cc, T dd, T ww): v(bb, cc, dd, ww) {}
    tquat(const vec4<T> & vv): v(vv) {}
    tquat(const tquat<T> & orig): v(orig.v) {}
    
    // all angles in radians
    tquat(const vec4<T> & axis, T angle): v(axis.xyz*(sin(angle/2)/vlength3(axis)), cos(angle/2)) {}
    
    tquat<T> & operator=(const tquat<T> & rhs) {v = rhs.v; return *this;}
    
    tquat<T> & operator+=(const tquat<T> & rhs) {v += rhs.v; return *this;}
    tquat<T> & operator-=(const tquat<T> & rhs) {v -= rhs.v; return *this;}
    
    tquat<T> & operator*=(double rhs) {v *= rhs; return *this;}
    tquat<T> & operator/=(double rhs) {v /= rhs; return *this;}
    
    tquat<T> operator*(double rhs) const {return tquat<T>(v*rhs);}
    tquat<T> operator/(double rhs) const {return tquat<T>(v/rhs);}
    
    tquat<T> operator+(const tquat<T> & rhs) const {return tquat<T>(v + rhs.v);}
    tquat<T> operator-(const tquat<T> & rhs) const {return tquat<T>(v - rhs.v);}
    
    tquat<T> operator*(const tquat<T> & rhs) const {
        return tquat<T>(
            (vec4<T>){
                v.x*rhs.v.x - v.y*rhs.v.y - v.z*rhs.v.z - v.w*rhs.v.w,
                v.x*rhs.v.y + v.y*rhs.v.x + v.z*rhs.v.w - v.w*rhs.v.z,
                v.x*rhs.v.z - v.y*rhs.v.w + v.z*rhs.v.x + v.w*rhs.v.y,
                v.x*rhs.v.w + v.y*rhs.v.z - v.z*rhs.v.y + v.w*rhs.v.x
            });
    }
    
    tquat<T> & operator*=(const tquat<T> & rhs) {return *this = *this*rhs;}
    
    tquat<T> operator-() const {return tquat<T>{-v.x, -v.y, -v.z, v.w};}
    
    void identity() {v = (vec4<T>){0, 0, 0, 1};}
    
    void clear() {identity();}
    
    T length_sqr() const {return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;}
    T length() const {return sqrt(length_sqr());}
    
    tquat<T> & normalize_eq() {return *this /= length();}
    tquat<T> normalized() {return *this/length();}
    
    tquat<T> inverse() const {T l = length_sqr(); return tquat<T>((vec4<T>){-v.x, -v.y, -v.z, v.w}/l);}
    tquat<T> invert() {T l = length_sqr(); v = (vec4<T>){-v.x, -v.y, -v.z, v.w}/l;}
    
    tquat<T> & set_rotation(const vec4<T> & axis, T angle) {return *this = tquat<T>(axis, angle);}
    tquat<T> & append_rotation(T angle, const vec4<T> & axis) {return *this *= tquat<T>(axis, angle);}
    tquat<T> & append_x_rotation(T angle) {return append_rotation(angle, vec4<T>(1, 0, 0, 0));}
    tquat<T> & append_y_rotation(T angle) {return append_rotation(angle, vec4<T>(0, 1, 0, 0));}
    tquat<T> & append_z_rotation(T angle) {return append_rotation(angle, vec4<T>(0, 0, 1, 0));}
    
    template<typename T2>
    vec4<T2> operator()(const vec4<T2> & pt) const {return rotate(pt);}
    
    template<typename T2>
    vec4<T2> operator()(size_t count, vec4<T2> * pts) const {return rotate(count, pts);}
    
    template<typename T2>
    vec4<T2> rotate(const vec4<T2> & pt) const
    {
        vec4<T2> result;
        T ww = v.w*v.w;
        T xx = v.x*v.x, yy = v.y*v.y, zz = v.z*v.z;
        T wx = v.w*v.x, wy = v.w*v.y, wz = v.w*v.z;
        T xy = v.x*v.y, xz = v.x*v.z, yz = v.y*v.z;
        
        result.x = (ww + xx - yy - zz)*pt.x +         2*(xy - wz)*pt.y +         2*(xz + wy)*pt.z;
        result.y =         2*(xy + wz)*pt.x + (ww - xx + yy - zz)*pt.y +         2*(yz - wx)*pt.z;
        result.z =         2*(xz - wy)*pt.x +         2*(yz + wx)*pt.y + (ww - xx - yy + zz)*pt.z;
        
        return result;
    }
    
    template<typename T2>
    void rotate(size_t count, vec4<T2> * pts) const {
        double mat[3][3];
        Get3x3(mat);
        for(size_t j = 0; j < count; ++j)
        {
            vec4<T> pt = pts[j];
            pts[j].x = mat[0][0]*pt.x + mat[1][0]*pt.y + mat[2][0]*pt.z;
            pts[j].y = mat[0][1]*pt.x + mat[1][1]*pt.y + mat[2][1]*pt.z;
            pts[j].z = mat[0][2]*pt.x + mat[1][2]*pt.y + mat[2][2]*pt.z;
        }
    }
    
    template<typename T2>
    vec4<T2> inv_rotate(const vec4<T2> & pt) const {
        tquat<T> inv(inverse());
        return inv.rotate(pt);
    }
    
    template<typename T2>
    void inv_rotate(size_t count, vec4<T2> * pts) const {
        tquat<T> inv(inverse());
        return inv.rotate(count, pts);
    }
    
    vec4<T> rotated_x() const {
        T ww = v.w*v.w, xx = v.x*v.x, yy = v.y*v.y, zz = v.z*v.z;
        T wy = v.w*v.y, wz = v.w*v.z;
        T xy = v.x*v.y, xz = v.x*v.z;
        return (vec4<T>)(ww + xx - yy - zz, 2*(xy + wz), 2*(xz - wy), 0);
    }
    
    vec4<T> rotated_y() const {
        T ww = v.w*v.w, xx = v.x*v.x, yy = v.y*v.y, zz = v.z*v.z;
        T wx = v.w*v.x, wz = v.w*v.z;
        T xy = v.x*v.y, yz = v.y*v.z;
        return (vec4<T>)(2*(xy - wz), ww - xx + yy - zz, 2*(yz + wx), 0);
    }
    
    vec4<T> rotated_z() const {
        T ww = v.w*v.w, xx = v.x*v.x, yy = v.y*v.y, zz = v.z*v.z;
        T wx = v.w*v.x, wy = v.w*v.y;
        T xz = v.x*v.z, yz = v.y*v.z;
        return (vec4<T>)(2*(xz + wy), 2*(yz - wx), ww - xx - yy + zz, 0);
    }
    
    void rotated_xyz(vec4<T> & rx, vec4<T> & ry, vec4<T> & rz) const {
        T ww = v.w*v.w, xx = v.x*v.x, yy = v.y*v.y, zz = v.z*v.z;
        T wx = v.w*v.x, wy = v.w*v.y, wz = v.w*v.z;
        T xy = v.x*v.y, xz = v.x*v.z, yz = v.y*v.z;
        
        rx = (vec4<T>)(ww + xx - yy - zz, 2*(xy + wz), 2*(xz - wy), 0);
        ry = (vec4<T>)(2*(xy - wz), ww - xx + yy - zz, 2*(yz + wx), 0);
        rz = (vec4<T>)(2*(xz + wy), 2*(yz - wx), ww - xx - yy + zz, 0);
    }
    
    template<typename T2>
    void Get3x3(T2 mat[3][3]) const {
        T2 ww = v.w*v.w;
        T2 xx = v.x*v.x, yy = v.y*v.y, zz = v.z*v.z;
        T2 wx = v.w*v.x, wy = v.w*v.y, wz = v.w*v.z;
        T2 xy = v.x*v.y, xz = v.x*v.z, yz = v.y*v.z;
        
        mat[0][0] = (ww + xx - yy - zz); mat[1][0] = 2*(xy - wz);         mat[2][0] = 2*(xz + wy);
        mat[0][1] = 2*(xy + wz);         mat[1][1] = (ww - xx + yy - zz); mat[2][1] = 2*(yz - wx);
        mat[0][2] = 2*(xz - wy);         mat[1][2] = 2*(yz + wx);         mat[2][2] = (ww - xx - yy + zz);
    }
    
    template<typename T2>
    void Get4x4(T2 mat[16]) const {
        T2 ww = v.w*v.w;
        T2 xx = v.x*v.x, yy = v.y*v.y, zz = v.z*v.z;
        T2 wx = v.w*v.x, wy = v.w*v.y, wz = v.w*v.z;
        T2 xy = v.x*v.y, xz = v.x*v.z, yz = v.y*v.z;
        
        mat[0] = (ww + xx - yy - zz); mat[4] = 2*(xy - wz);         mat[8]  = 2*(xz + wy);         mat[12] = 0;
        mat[1] = 2*(xy + wz);         mat[5] = (ww - xx + yy - zz); mat[9]  = 2*(yz - wx);         mat[13] = 0;
        mat[2] = 2*(xz - wy);         mat[6] = 2*(yz + wx);         mat[10] = (ww - xx - yy + zz); mat[14] = 0;
        mat[3] = 0;                   mat[7] = 0;                   mat[11] = 0;                   mat[15] = 1;
    }
};

//----------------------------------------------------------------------------------------------------

typedef tquat<float> fquat;
typedef tquat<double> dquat;

#endif // QUATERNION_H
