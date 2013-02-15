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

#ifndef VMATH_H
#define VMATH_H

#include <cmath>

#ifdef __SSE4_1__
#include "smmintrin.h"
#endif

typedef int v4si __attribute__((__vector_size__(16)));

template<typename T> using vec2 = T __attribute__((ext_vector_type(2)));
template<typename T> using vec4 = T __attribute__((ext_vector_type(4)));
template<typename T, int n> using vec_t = T __attribute__((ext_vector_type(n)));

typedef vec_t<float, 2> float2;
typedef vec_t<float, 4> float4;
typedef vec_t<double, 2> double2;
typedef vec_t<double, 4> double4;


//******************************************************************************

// *****************************************************************************
// Vector operations
// *****************************************************************************

// 3D cross product
static inline float4 vcross3(const float4 & a, const float4 & b)
{
    return (float4){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x, 0};
}

static inline double4 vcross3(double4 a, double4 b)
{
    return (double4){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x, 0};
}


template<typename T>
vec4<T> x_cross3(vec4<T> v) {return vec4<T>{0.0,-v.z, v.y};}

template<typename T>
vec4<T> y_cross3(vec4<T> v) {return vec4<T>{v.z, 0.0,-v.x};}

template<typename T>
vec4<T> z_cross3(vec4<T> v) {return vec4<T>{-v.y, v.x, 0.0};}

template<typename T>
vec4<T> cross3_x(vec4<T> v) {return vec4<T>{0.0, v.z,-v.y};}

template<typename T>
vec4<T> cross3_y(vec4<T> v) {return vec4<T>{v.z, 0.0, v.x};}

template<typename T>
vec4<T> cross3_z(vec4<T> v) {return vec4<T>{v.y,-v.x, 0.0};}


#ifdef __SSE4_1__
static inline float vdot3(float4 a, float4 b)
{
    // float4 tmp = a*b;
    // tmp = _mm_hadd_ps(tmp, tmp);
    // tmp = _mm_hadd_ps(tmp, tmp);
    // return tmp.x;
    return _mm_cvtss_f32(_mm_dp_ps(a, b, 0x71));
}
#else
static inline float vdot3(const float4 & a, const float4 & b)
{
    float4 tmp = a*b;
    return tmp.x + tmp.y + tmp.z;
}
#endif

static inline double vdot3(double4 a, double4 b)
{
    double4 tmp = a*b;
    return tmp.x + tmp.y + tmp.z + tmp.w;
}

// -----------------------------------------------------------------------------

#ifdef __SSE4_1__
static inline float vlength_sqr3(float4 v)
{
    // float4 tmp = v*v;
    // tmp = _mm_hadd_ps(tmp, tmp);
    // tmp = _mm_hadd_ps(tmp, tmp);
    // return tmp.x;
    return _mm_cvtss_f32(_mm_dp_ps(v, v, 0x71));
}
#else
static inline float vlength_sqr3(float4 v)
{
    float4 tmp = v*v;
    return tmp.x + tmp.y + tmp.z;
}
#endif

static inline double vlength_sqr3(double4 v)
{
    double4 tmp = v*v;
    return tmp.x + tmp.y + tmp.z;
}

// -----------------------------------------------------------------------------

#ifdef __SSE4_1__
static inline float vlength3(float4 v)
{
    // float4 tmp = v*v;
    // tmp = _mm_hadd_ps(tmp, tmp);
    // tmp = _mm_hadd_ps(tmp, tmp);
    // return _mm_cvtss_f32(_mm_sqrt_ss(tmp));
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v, v, 0x71)));
}
#else
static inline float vlength3(float4 v)
{
    float4 tmp = v*v;
    return sqrt(tmp.x + tmp.y + tmp.z);
}
#endif

static inline double vlength3(double4 v)
{
    double4 tmp = v*v;
    return sqrt(tmp.x + tmp.y + tmp.z);
}

// -----------------------------------------------------------------------------

template<typename T, typename T2>
vec4<T> v3_with_length(vec4<T> v, T2 len) {return v*(T)(len/vlength3(v));}

// -----------------------------------------------------------------------------

#ifdef __SSE4_1__
static inline float4 vnormalized3(float4 v)
{
    float4 tmp = _mm_sqrt_ss(_mm_dp_ps(v, v, 0x71));
    return v/vlength3(v);
}
#else
static inline float4 vnormalized3(float4 v)
{
    float4 tmp = v*v;
    return v/(float4)(sqrt(tmp.x + tmp.y + tmp.z));
}
#endif

static inline double4 vnormalized3(double4 v)
{
    double4 tmp = v*v;
    return v/(double4)(sqrt(tmp.x + tmp.y + tmp.z));
}

// -----------------------------------------------------------------------------

static inline float4 vquat(const float4 & axis, float th) {
    float4 v = axis*(float4)(sin(th/2));
    v.w = cos(th/2);
    return v;
}

// -----------------------------------------------------------------------------

// This is simpler than it looks: create a quaternion for the given rotation and
// make a 3x3 matrix that gets used for a single vector multiplication.
static inline float4 vrotated(float4 v, float4 axis, float th) {
    // float4 a = axis*(float4)(sin(th/2));
    // float w = cos(th/2);
    // float ww = w*w;
    // float xx = a.x*a.x, yy = a.y*a.y, zz = a.z*a.z;
    // float wx = w*a.x,   wy = w*a.y,   wz = w*a.z;
    // float xy = a.x*a.y, xz = a.x*a.z, yz = a.y*a.z;
    // return (float4){(ww + xx - yy - zz)*v.x +         2*(xy - wz)*v.y +         2*(xz + wy)*v.z,
    //                         2*(xy + wz)*v.x + (ww - xx + yy - zz)*v.y +         2*(yz - wx)*v.z,
    //                         2*(xz - wy)*v.x +         2*(yz + wx)*v.y + (ww - xx - yy + zz)*v.z};
    
    float4 a = axis*(float4)(sin(th/2));
    v.w = cos(th/2);
    float4 aa = a*a;
    float4 wa = a*v.w;
    float xy = a.x*a.y, xz = a.x*a.z, yz = a.y*a.z;
    return (float4){
        (aa.w + aa.x - aa.y - aa.z)*v.x +               2*(xy - wa.z)*v.y +               2*(xz + wa.y)*v.z,
                      2*(xy + wa.z)*v.x + (aa.w - aa.x + aa.y - aa.z)*v.y +               2*(yz - wa.x)*v.z,
                      2*(xz - wa.y)*v.x +               2*(yz + wa.x)*v.y + (aa.w - aa.x - aa.y + aa.z)*v.z};
}

// -----------------------------------------------------------------------------
#ifdef __SSE4_1__
static inline float vdot4(float4 a, float4 b)
{
    return _mm_cvtss_f32(_mm_dp_ps(a, b, 0xF1));
}
#else
static inline float vdot4(const float4 & a, const float4 & b)
{
    float4 tmp = a*b;
    return tmp.x + tmp.y + tmp.z + tmp.w;
}
#endif

#endif // VMATH_H
