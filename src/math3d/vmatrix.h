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

#ifndef VMATRIX_H
#define VMATRIX_H

#include "vmath.h"
#include "quaternion.h"

// *****************************************************************************
// Matrices
// *****************************************************************************

template<typename T>
struct mat44_templ;

template<typename T>
struct mat33_templ {
    typedef vec_t<T, 4> vec_t;
    vec_t rows[3];
    
    static mat33_templ<T> identity() {
        mat33_templ<T> m;
        m.rows[0] = (vec_t){1, 0, 0, 0};
        m.rows[1] = (vec_t){0, 1, 0, 0};
        m.rows[2] = (vec_t){0, 0, 1, 0};
        return m;
    }
    
    void transpose(mat33_templ<T> & result) const;
};

template<typename T>
struct mat44_templ {
    typedef vec_t<T, 4> vec_t;
    vec_t rows[4];
    
    static mat44_templ<T> identity() {
        mat44_templ<T> m;
        m.rows[0] = (vec_t){1, 0, 0, 0};
        m.rows[1] = (vec_t){0, 1, 0, 0};
        m.rows[2] = (vec_t){0, 0, 1, 0};
        m.rows[3] = (vec_t){0, 0, 0, 1};
        return m;
    }
    
    static mat44_templ<T> translation(T x, T y, T z) {
        mat44_templ<T> m;
        m.rows[0] = (vec_t){1, 0, 0, x};
        m.rows[1] = (vec_t){0, 1, 0, y};
        m.rows[2] = (vec_t){0, 0, 1, z};
        m.rows[3] = (vec_t){0, 0, 0, 1};
        return m;
    }
    
    static mat44_templ<T> scaling(T x, T y, T z) {
        mat44_templ<T> m;
        m.rows[0] = (vec_t){x, 0, 0, 0};
        m.rows[1] = (vec_t){0, y, 0, 0};
        m.rows[2] = (vec_t){0, 0, z, 0};
        m.rows[3] = (vec_t){0, 0, 0, 1};
        return m;
    }
    
    static mat44_templ<T> rotation(const vec4<T> & amt) {
        mat44_templ<T> m, m2;
        m = rotation(vec4<T>{1, 0, 0}, amt.x);
        m.multiply(m2, rotation(vec4<T>{0, 1, 0}, amt.y));
        m2.multiply(m, rotation(vec4<T>{0, 0, 1}, amt.z));
        return m;
    }
    
    static mat44_templ<T> rotation(const vec4<T> & axis, T th) {
        float4 a = axis*(float4)(sin(th/2));
        a.w = cos(th/2);
        float4 aa = a*a;
        float4 wa = a*(float4)(a.w);
        float xy = a.x*a.y, xz = a.x*a.z, yz = a.y*a.z;
        mat44_templ<T> m;
        m.rows[0] = (vec_t){(aa.w + aa.x - aa.y - aa.z),               2*(xy - wa.z),               2*(xz + wa.y), 0};
        m.rows[1] = (vec_t){              2*(xy + wa.z), (aa.w - aa.x + aa.y - aa.z),               2*(yz - wa.x), 0};
        m.rows[2] = (vec_t){              2*(xz - wa.y),               2*(yz + wa.x), (aa.w - aa.x - aa.y + aa.z), 0};
        m.rows[3] = (vec_t){0, 0, 0, 1};
        return m;
    }
    
    mat44_templ() {}
    
    mat44_templ<T> & operator=(const mat44_templ<T> & rhs) {
        rows[0] = rhs.rows[0];
        rows[1] = rhs.rows[1];
        rows[2] = rhs.rows[2];
        rows[3] = rhs.rows[3];
        return *this;
    }
    
    void transpose(mat44_templ<T> & result) const;
    
    void multiply(mat44_templ<T> & result, const mat44_templ<T> & m);
    
    mat44_templ<T> & scale(vec_t v) {
        rows[0] *= v.x;
        rows[1] *= v.y;
        rows[2] *= v.z;
        return *this;
    }
    mat44_templ<T> & translate(vec_t v) {
        rows[0].w += v.x;
        rows[1].w += v.y;
        rows[2].w += v.z;
        return *this;
    }
    mat44_templ<T> & rotate(vec_t v) {
        mat44_templ<T> tmp;
        multiply(tmp, rotation(v));
        return *this = tmp;
    }
    mat44_templ<T> & rotate(vec_t axis, T th) {
        mat44_templ<T> tmp;
        multiply(tmp, rotation(axis, th));
        return *this = tmp;
    }
    
    // Points are transformed using full first three rows of the matrix.
    // Directions are transformed only with the 3x3 submatrix.
    // Normals are transformed with the transpose of the 3x3 submatrix.
    vec_t trans_pt(vec_t pt) const;
    vec_t trans_dir(vec_t pt) const;
    vec_t trans_norm(vec_t pt) const;
    
    void inverse(mat44_templ<T> & result) const;
};


template<typename T>
void mat33_templ<T>::transpose(mat33_templ<T> & result) const
{
    result.rows[0] = (mat33_templ<T>::vec_t){rows[0].x, rows[1].x, rows[2].x, 0};
    result.rows[1] = (mat33_templ<T>::vec_t){rows[0].y, rows[1].y, rows[2].y, 0};
    result.rows[2] = (mat33_templ<T>::vec_t){rows[0].z, rows[1].z, rows[2].z, 0};
}

template<typename T>
void mat44_templ<T>::transpose(mat44_templ<T> & result) const
{
    result.rows[0] = (mat44_templ<T>::vec_t){rows[0].x, rows[1].x, rows[2].x, rows[3].x};
    result.rows[1] = (mat44_templ<T>::vec_t){rows[0].y, rows[1].y, rows[2].y, rows[3].y};
    result.rows[2] = (mat44_templ<T>::vec_t){rows[0].z, rows[1].z, rows[2].z, rows[3].z};
    result.rows[3] = (mat44_templ<T>::vec_t){rows[0].w, rows[1].w, rows[2].w, rows[3].w};
}

template<typename T>
void mat44_templ<T>::multiply(mat44_templ<T> & result, const mat44_templ<T> & m)
{
    for(int j = 0; j < 4; j++)
    {
        result.rows[j] = mat44_templ<T>::vec_t{
            vdot4(rows[j], mat44_templ<T>::vec_t{m.rows[0].x, m.rows[1].x,  m.rows[2].x, m.rows[3].x}),
            vdot4(rows[j], mat44_templ<T>::vec_t{m.rows[0].y, m.rows[1].y,  m.rows[2].y, m.rows[3].y}),
            vdot4(rows[j], mat44_templ<T>::vec_t{m.rows[0].z, m.rows[1].z,  m.rows[2].z, m.rows[3].z}),
            vdot4(rows[j], mat44_templ<T>::vec_t{m.rows[0].w, m.rows[1].w,  m.rows[2].w, m.rows[3].w})
        };
    }
}

template<typename T>
vec4<T> mat44_templ<T>::trans_pt(vec4<T> pt) const
{
    pt.w = 1;
    return mat44_templ<T>::vec_t{
        vdot4(pt, rows[0]),
        vdot4(pt, rows[1]),
        vdot4(pt, rows[2]),
        0
    };
}

template<typename T>
vec4<T> mat44_templ<T>::trans_dir(vec4<T> pt) const
{
    return mat44_templ<T>::vec_t{
        vdot3(pt, rows[0]),
        vdot3(pt, rows[1]),
        vdot3(pt, rows[2]),
        0
    };
}

template<typename T>
vec4<T> mat44_templ<T>::trans_norm(vec4<T> pt) const
{
    mat44_templ<T>::vec_t result;
    result = ((vec4<T>)(pt.x))*rows[0] +
             ((vec4<T>)(pt.y))*rows[1] +
             ((vec4<T>)(pt.z))*rows[2];
    result.w = 0;
    return result;
}

// Invert using Cramer's rule
// Based on Intel AP-928: Streaming SIMD Extensions - Inverse of 4x4 Matrix
// http://download.intel.com/design/PentiumIII/sml/24504301.pdf
template<typename T>
void mat44_templ<T>::inverse(mat44_templ<T> & result) const
{
    // #define m00  (rows[0].x)
    // #define m01  (rows[0].y)
    // #define m02  (rows[0].z)
    // #define m03  (rows[0].w)
    // #define m10  (rows[1].x)
    // #define m11  (rows[1].y)
    // #define m12  (rows[1].z)
    // #define m13  (rows[1].w)
    // #define m20  (rows[2].x)
    // #define m21  (rows[2].y)
    // #define m22  (rows[2].z)
    // #define m23  (rows[2].w)
    // #define m30  (rows[3].x)
    // #define m31  (rows[3].y)
    // #define m32  (rows[3].z)
    // #define m33  (rows[3].w)
    
    #define m00  (rows[0].x)
    #define m01  (rows[1].x)
    #define m02  (rows[2].x)
    #define m03  (rows[3].x)
    #define m10  (rows[0].y)
    #define m11  (rows[1].y)
    #define m12  (rows[2].y)
    #define m13  (rows[3].y)
    #define m20  (rows[0].z)
    #define m21  (rows[1].z)
    #define m22  (rows[2].z)
    #define m23  (rows[3].z)
    #define m30  (rows[0].w)
    #define m31  (rows[1].w)
    #define m32  (rows[2].w)
    #define m33  (rows[3].w)
    
    T tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
    // Compute cofactors
    tmp0 = m20*m33 - m23*m30;
    tmp1 = m20*m32 - m22*m30;
    tmp2 = m20*m31 - m21*m30;
    tmp3 = m21*m33 - m23*m31;
    tmp4 = m21*m32 - m22*m31;
    tmp5 = m22*m33 - m23*m32;
    
    result.rows[0] = (mat44_templ<T>::vec_t){
                    tmp5*m11 - tmp3*m12 + tmp4*m13,
        -tmp5*m10            + tmp0*m12 - tmp1*m13,
         tmp3*m10 - tmp0*m11            + tmp2*m13,
        -tmp4*m10 + tmp1*m11 - tmp2*m12
    };
    
    result.rows[1] = (mat44_templ<T>::vec_t){
                  - tmp5*m01 + tmp3*m02 - tmp4*m03,
         tmp5*m00            - tmp0*m02 + tmp1*m03,
        -tmp3*m00 + tmp0*m01            - tmp2*m03,
         tmp4*m00 - tmp1*m01 + tmp2*m02
    };
    
    tmp0 = m00*m11 - m01*m10;
    tmp1 = m00*m12 - m02*m10;
    tmp2 = m00*m13 - m03*m10;
    tmp3 = m01*m13 - m03*m11;
    tmp4 = m01*m12 - m02*m11;
    tmp5 = m02*m13 - m03*m12;
    
    result.rows[2] = (mat44_templ<T>::vec_t){
                    tmp5*m31 - tmp3*m32 + tmp4*m33,
        -tmp5*m30            + tmp2*m32 - tmp1*m33,
         tmp3*m30 - tmp2*m31            + tmp0*m33,
        -tmp4*m30 + tmp1*m31 - tmp0*m32
    };
    
    result.rows[3] = (mat44_templ<T>::vec_t){
                  - tmp5*m21 + tmp3*m22 - tmp4*m23,
         tmp5*m20            - tmp2*m22 + tmp1*m23,
        -tmp3*m20 + tmp2*m21            - tmp0*m23,
         tmp4*m20 - tmp1*m21 + tmp0*m22
    };
    
    T det = 1.0/vdot4((mat44_templ<T>::vec_t){m00, m01, m02, m03}, result.rows[0]);
    result.rows[0] *= det;
    result.rows[1] *= det;
    result.rows[2] *= det;
    result.rows[3] *= det;
    
    #undef m00
    #undef m01
    #undef m02
    #undef m03
    #undef m10
    #undef m11
    #undef m12
    #undef m13
    #undef m20
    #undef m21
    #undef m22
    #undef m23
    #undef m30
    #undef m31
    #undef m32
    #undef m33
}


typedef mat33_templ<float> mat33f;
typedef mat33_templ<double> mat33d;
typedef mat44_templ<float> mat44f;
typedef mat44_templ<double> mat44d;


#endif // VMATRIX_H
