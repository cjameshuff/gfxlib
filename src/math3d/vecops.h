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

// This file is generated by genops.rb.

#ifndef VECOPS_H
#define VECOPS_H

namespace m3d {

//*******************************************************************************
// Arithmetic operators
//*******************************************************************************
//*******************************************************************************
// LHS: tvec2, operator: unary -
//*******************************************************************************
template<typename T1>
tvec2<T1> operator-(const tvec2<T1> & rhs)
{
    return tvec2<T1>(-rhs.x, -rhs.y);
}

//*******************************************************************************
// LHS: tvec2, operator: +
//*******************************************************************************
// scalar + tvec2
template<typename T1, typename T2>
tvec2<T2> operator+(const T1 & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs + rhs.x, lhs + rhs.y);
}

// tvec2 + scalar
template<typename T1, typename T2>
tvec2<T1> operator+(const tvec2<T1> & lhs, const T2 & rhs)
{
    return tvec2<T2>(lhs.x + rhs, lhs.y + rhs);
}

// tvec2 += scalar
template<typename T1, typename T2>
tvec2<T1> & operator+=(tvec2<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs + rhs);
}

// tvec2 + tvec2
template<typename T1, typename T2>
tvec2<T1> operator+(const tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs.x + rhs.x, lhs.y + rhs.y);
}

// tvec2 += tvec2
template<typename T1, typename T2>
tvec2<T1> & operator+=(tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs + rhs);
}

// tvec2 + tvec3
template<typename T1, typename T2>
tvec2<T1> operator+(const tvec2<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec2<T2>(lhs.x + rhs.x, lhs.y + rhs.y);
}

// tvec2 + tvec4
template<typename T1, typename T2>
tvec2<T1> operator+(const tvec2<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec2<T2>(lhs.x + rhs.x, lhs.y + rhs.y);
}


//*******************************************************************************
// LHS: tvec2, operator: -
//*******************************************************************************
// scalar - tvec2
template<typename T1, typename T2>
tvec2<T2> operator-(const T1 & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs - rhs.x, lhs - rhs.y);
}

// tvec2 - scalar
template<typename T1, typename T2>
tvec2<T1> operator-(const tvec2<T1> & lhs, const T2 & rhs)
{
    return tvec2<T2>(lhs.x - rhs, lhs.y - rhs);
}

// tvec2 -= scalar
template<typename T1, typename T2>
tvec2<T1> & operator-=(tvec2<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs - rhs);
}

// tvec2 - tvec2
template<typename T1, typename T2>
tvec2<T1> operator-(const tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs.x - rhs.x, lhs.y - rhs.y);
}

// tvec2 -= tvec2
template<typename T1, typename T2>
tvec2<T1> & operator-=(tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs - rhs);
}

// tvec2 - tvec3
template<typename T1, typename T2>
tvec2<T1> operator-(const tvec2<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec2<T2>(lhs.x - rhs.x, lhs.y - rhs.y);
}

// tvec2 - tvec4
template<typename T1, typename T2>
tvec2<T1> operator-(const tvec2<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec2<T2>(lhs.x - rhs.x, lhs.y - rhs.y);
}


//*******************************************************************************
// LHS: tvec2, operator: *
//*******************************************************************************
// scalar * tvec2
template<typename T1, typename T2>
tvec2<T2> operator*(const T1 & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs * rhs.x, lhs * rhs.y);
}

// tvec2 * scalar
template<typename T1, typename T2>
tvec2<T1> operator*(const tvec2<T1> & lhs, const T2 & rhs)
{
    return tvec2<T2>(lhs.x * rhs, lhs.y * rhs);
}

// tvec2 *= scalar
template<typename T1, typename T2>
tvec2<T1> & operator*=(tvec2<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs * rhs);
}

// tvec2 * tvec2
template<typename T1, typename T2>
tvec2<T1> operator*(const tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs.x * rhs.x, lhs.y * rhs.y);
}

// tvec2 *= tvec2
template<typename T1, typename T2>
tvec2<T1> & operator*=(tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs * rhs);
}

// tvec2 * tvec3
template<typename T1, typename T2>
tvec2<T1> operator*(const tvec2<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec2<T2>(lhs.x * rhs.x, lhs.y * rhs.y);
}

// tvec2 * tvec4
template<typename T1, typename T2>
tvec2<T1> operator*(const tvec2<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec2<T2>(lhs.x * rhs.x, lhs.y * rhs.y);
}


//*******************************************************************************
// LHS: tvec2, operator: /
//*******************************************************************************
// scalar / tvec2
template<typename T1, typename T2>
tvec2<T2> operator/(const T1 & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs / rhs.x, lhs / rhs.y);
}

// tvec2 / scalar
template<typename T1, typename T2>
tvec2<T1> operator/(const tvec2<T1> & lhs, const T2 & rhs)
{
    return tvec2<T2>(lhs.x / rhs, lhs.y / rhs);
}

// tvec2 /= scalar
template<typename T1, typename T2>
tvec2<T1> & operator/=(tvec2<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs / rhs);
}

// tvec2 / tvec2
template<typename T1, typename T2>
tvec2<T1> operator/(const tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs.x / rhs.x, lhs.y / rhs.y);
}

// tvec2 /= tvec2
template<typename T1, typename T2>
tvec2<T1> & operator/=(tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs / rhs);
}

// tvec2 / tvec3
template<typename T1, typename T2>
tvec2<T1> operator/(const tvec2<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec2<T2>(lhs.x / rhs.x, lhs.y / rhs.y);
}

// tvec2 / tvec4
template<typename T1, typename T2>
tvec2<T1> operator/(const tvec2<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec2<T2>(lhs.x / rhs.x, lhs.y / rhs.y);
}


//*******************************************************************************
// LHS: tvec2, operator: %
//*******************************************************************************
// scalar % tvec2
template<typename T1, typename T2>
tvec2<T2> operator%(const T1 & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs % rhs.x, lhs % rhs.y);
}

// tvec2 % scalar
template<typename T1, typename T2>
tvec2<T1> operator%(const tvec2<T1> & lhs, const T2 & rhs)
{
    return tvec2<T2>(lhs.x % rhs, lhs.y % rhs);
}

// tvec2 %= scalar
template<typename T1, typename T2>
tvec2<T1> & operator%=(tvec2<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs % rhs);
}

// tvec2 % tvec2
template<typename T1, typename T2>
tvec2<T1> operator%(const tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec2<T2>(lhs.x % rhs.x, lhs.y % rhs.y);
}

// tvec2 %= tvec2
template<typename T1, typename T2>
tvec2<T1> & operator%=(tvec2<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs % rhs);
}

// tvec2 % tvec3
template<typename T1, typename T2>
tvec2<T1> operator%(const tvec2<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec2<T2>(lhs.x % rhs.x, lhs.y % rhs.y);
}

// tvec2 % tvec4
template<typename T1, typename T2>
tvec2<T1> operator%(const tvec2<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec2<T2>(lhs.x % rhs.x, lhs.y % rhs.y);
}


//*******************************************************************************
// LHS: tvec3, operator: unary -
//*******************************************************************************
template<typename T1>
tvec3<T1> operator-(const tvec3<T1> & rhs)
{
    return tvec3<T1>(-rhs.x, -rhs.y, -rhs.z);
}

//*******************************************************************************
// LHS: tvec3, operator: +
//*******************************************************************************
// scalar + tvec3
template<typename T1, typename T2>
tvec3<T2> operator+(const T1 & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
}

// tvec3 + scalar
template<typename T1, typename T2>
tvec3<T1> operator+(const tvec3<T1> & lhs, const T2 & rhs)
{
    return tvec3<T2>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
}

// tvec3 += scalar
template<typename T1, typename T2>
tvec3<T1> & operator+=(tvec3<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs + rhs);
}

// tvec3 + tvec2
template<typename T1, typename T2>
tvec3<T1> operator+(const tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec3<T2>(lhs.x + rhs.x, lhs.y + rhs.y, 0);
}

// tvec3 += tvec2
template<typename T1, typename T2>
tvec3<T1> & operator+=(tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs + rhs);
}

// tvec3 + tvec3
template<typename T1, typename T2>
tvec3<T1> operator+(const tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

// tvec3 += tvec3
template<typename T1, typename T2>
tvec3<T1> & operator+=(tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs + rhs);
}

// tvec3 + tvec4
template<typename T1, typename T2>
tvec3<T1> operator+(const tvec3<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec3<T2>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}


//*******************************************************************************
// LHS: tvec3, operator: -
//*******************************************************************************
// scalar - tvec3
template<typename T1, typename T2>
tvec3<T2> operator-(const T1 & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
}

// tvec3 - scalar
template<typename T1, typename T2>
tvec3<T1> operator-(const tvec3<T1> & lhs, const T2 & rhs)
{
    return tvec3<T2>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
}

// tvec3 -= scalar
template<typename T1, typename T2>
tvec3<T1> & operator-=(tvec3<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs - rhs);
}

// tvec3 - tvec2
template<typename T1, typename T2>
tvec3<T1> operator-(const tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec3<T2>(lhs.x - rhs.x, lhs.y - rhs.y, 0);
}

// tvec3 -= tvec2
template<typename T1, typename T2>
tvec3<T1> & operator-=(tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs - rhs);
}

// tvec3 - tvec3
template<typename T1, typename T2>
tvec3<T1> operator-(const tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

// tvec3 -= tvec3
template<typename T1, typename T2>
tvec3<T1> & operator-=(tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs - rhs);
}

// tvec3 - tvec4
template<typename T1, typename T2>
tvec3<T1> operator-(const tvec3<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec3<T2>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}


//*******************************************************************************
// LHS: tvec3, operator: *
//*******************************************************************************
// scalar * tvec3
template<typename T1, typename T2>
tvec3<T2> operator*(const T1 & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

// tvec3 * scalar
template<typename T1, typename T2>
tvec3<T1> operator*(const tvec3<T1> & lhs, const T2 & rhs)
{
    return tvec3<T2>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

// tvec3 *= scalar
template<typename T1, typename T2>
tvec3<T1> & operator*=(tvec3<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs * rhs);
}

// tvec3 * tvec2
template<typename T1, typename T2>
tvec3<T1> operator*(const tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec3<T2>(lhs.x * rhs.x, lhs.y * rhs.y, 0);
}

// tvec3 *= tvec2
template<typename T1, typename T2>
tvec3<T1> & operator*=(tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs * rhs);
}

// tvec3 * tvec3
template<typename T1, typename T2>
tvec3<T1> operator*(const tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

// tvec3 *= tvec3
template<typename T1, typename T2>
tvec3<T1> & operator*=(tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs * rhs);
}

// tvec3 * tvec4
template<typename T1, typename T2>
tvec3<T1> operator*(const tvec3<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec3<T2>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}


//*******************************************************************************
// LHS: tvec3, operator: /
//*******************************************************************************
// scalar / tvec3
template<typename T1, typename T2>
tvec3<T2> operator/(const T1 & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}

// tvec3 / scalar
template<typename T1, typename T2>
tvec3<T1> operator/(const tvec3<T1> & lhs, const T2 & rhs)
{
    return tvec3<T2>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

// tvec3 /= scalar
template<typename T1, typename T2>
tvec3<T1> & operator/=(tvec3<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs / rhs);
}

// tvec3 / tvec2
template<typename T1, typename T2>
tvec3<T1> operator/(const tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec3<T2>(lhs.x / rhs.x, lhs.y / rhs.y, 0);
}

// tvec3 /= tvec2
template<typename T1, typename T2>
tvec3<T1> & operator/=(tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs / rhs);
}

// tvec3 / tvec3
template<typename T1, typename T2>
tvec3<T1> operator/(const tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

// tvec3 /= tvec3
template<typename T1, typename T2>
tvec3<T1> & operator/=(tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs / rhs);
}

// tvec3 / tvec4
template<typename T1, typename T2>
tvec3<T1> operator/(const tvec3<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec3<T2>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}


//*******************************************************************************
// LHS: tvec3, operator: %
//*******************************************************************************
// scalar % tvec3
template<typename T1, typename T2>
tvec3<T2> operator%(const T1 & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs % rhs.x, lhs % rhs.y, lhs % rhs.z);
}

// tvec3 % scalar
template<typename T1, typename T2>
tvec3<T1> operator%(const tvec3<T1> & lhs, const T2 & rhs)
{
    return tvec3<T2>(lhs.x % rhs, lhs.y % rhs, lhs.z % rhs);
}

// tvec3 %= scalar
template<typename T1, typename T2>
tvec3<T1> & operator%=(tvec3<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs % rhs);
}

// tvec3 % tvec2
template<typename T1, typename T2>
tvec3<T1> operator%(const tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec3<T2>(lhs.x % rhs.x, lhs.y % rhs.y, 0);
}

// tvec3 %= tvec2
template<typename T1, typename T2>
tvec3<T1> & operator%=(tvec3<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs % rhs);
}

// tvec3 % tvec3
template<typename T1, typename T2>
tvec3<T1> operator%(const tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec3<T2>(lhs.x % rhs.x, lhs.y % rhs.y, lhs.z % rhs.z);
}

// tvec3 %= tvec3
template<typename T1, typename T2>
tvec3<T1> & operator%=(tvec3<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs % rhs);
}

// tvec3 % tvec4
template<typename T1, typename T2>
tvec3<T1> operator%(const tvec3<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec3<T2>(lhs.x % rhs.x, lhs.y % rhs.y, lhs.z % rhs.z);
}


//*******************************************************************************
// LHS: tvec4, operator: unary -
//*******************************************************************************
template<typename T1>
tvec4<T1> operator-(const tvec4<T1> & rhs)
{
    return tvec4<T1>(-rhs.x, -rhs.y, -rhs.z, -rhs.w);
}

//*******************************************************************************
// LHS: tvec4, operator: +
//*******************************************************************************
// scalar + tvec4
template<typename T1, typename T2>
tvec4<T2> operator+(const T1 & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
}

// tvec4 + scalar
template<typename T1, typename T2>
tvec4<T1> operator+(const tvec4<T1> & lhs, const T2 & rhs)
{
    return tvec4<T2>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
}

// tvec4 += scalar
template<typename T1, typename T2>
tvec4<T1> & operator+=(tvec4<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs + rhs);
}

// tvec4 + tvec2
template<typename T1, typename T2>
tvec4<T1> operator+(const tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec4<T2>(lhs.x + rhs.x, lhs.y + rhs.y, 0, 0);
}

// tvec4 += tvec2
template<typename T1, typename T2>
tvec4<T1> & operator+=(tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs + rhs);
}

// tvec4 + tvec3
template<typename T1, typename T2>
tvec4<T1> operator+(const tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec4<T2>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, 0);
}

// tvec4 += tvec3
template<typename T1, typename T2>
tvec4<T1> & operator+=(tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs + rhs);
}

// tvec4 + tvec4
template<typename T1, typename T2>
tvec4<T1> operator+(const tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

// tvec4 += tvec4
template<typename T1, typename T2>
tvec4<T1> & operator+=(tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return (lhs = lhs + rhs);
}


//*******************************************************************************
// LHS: tvec4, operator: -
//*******************************************************************************
// scalar - tvec4
template<typename T1, typename T2>
tvec4<T2> operator-(const T1 & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
}

// tvec4 - scalar
template<typename T1, typename T2>
tvec4<T1> operator-(const tvec4<T1> & lhs, const T2 & rhs)
{
    return tvec4<T2>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
}

// tvec4 -= scalar
template<typename T1, typename T2>
tvec4<T1> & operator-=(tvec4<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs - rhs);
}

// tvec4 - tvec2
template<typename T1, typename T2>
tvec4<T1> operator-(const tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec4<T2>(lhs.x - rhs.x, lhs.y - rhs.y, 0, 0);
}

// tvec4 -= tvec2
template<typename T1, typename T2>
tvec4<T1> & operator-=(tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs - rhs);
}

// tvec4 - tvec3
template<typename T1, typename T2>
tvec4<T1> operator-(const tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec4<T2>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, 0);
}

// tvec4 -= tvec3
template<typename T1, typename T2>
tvec4<T1> & operator-=(tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs - rhs);
}

// tvec4 - tvec4
template<typename T1, typename T2>
tvec4<T1> operator-(const tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

// tvec4 -= tvec4
template<typename T1, typename T2>
tvec4<T1> & operator-=(tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return (lhs = lhs - rhs);
}


//*******************************************************************************
// LHS: tvec4, operator: *
//*******************************************************************************
// scalar * tvec4
template<typename T1, typename T2>
tvec4<T2> operator*(const T1 & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
}

// tvec4 * scalar
template<typename T1, typename T2>
tvec4<T1> operator*(const tvec4<T1> & lhs, const T2 & rhs)
{
    return tvec4<T2>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

// tvec4 *= scalar
template<typename T1, typename T2>
tvec4<T1> & operator*=(tvec4<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs * rhs);
}

// tvec4 * tvec2
template<typename T1, typename T2>
tvec4<T1> operator*(const tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec4<T2>(lhs.x * rhs.x, lhs.y * rhs.y, 0, 0);
}

// tvec4 *= tvec2
template<typename T1, typename T2>
tvec4<T1> & operator*=(tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs * rhs);
}

// tvec4 * tvec3
template<typename T1, typename T2>
tvec4<T1> operator*(const tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec4<T2>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, 0);
}

// tvec4 *= tvec3
template<typename T1, typename T2>
tvec4<T1> & operator*=(tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs * rhs);
}

// tvec4 * tvec4
template<typename T1, typename T2>
tvec4<T1> operator*(const tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

// tvec4 *= tvec4
template<typename T1, typename T2>
tvec4<T1> & operator*=(tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return (lhs = lhs * rhs);
}


//*******************************************************************************
// LHS: tvec4, operator: /
//*******************************************************************************
// scalar / tvec4
template<typename T1, typename T2>
tvec4<T2> operator/(const T1 & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w);
}

// tvec4 / scalar
template<typename T1, typename T2>
tvec4<T1> operator/(const tvec4<T1> & lhs, const T2 & rhs)
{
    return tvec4<T2>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
}

// tvec4 /= scalar
template<typename T1, typename T2>
tvec4<T1> & operator/=(tvec4<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs / rhs);
}

// tvec4 / tvec2
template<typename T1, typename T2>
tvec4<T1> operator/(const tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec4<T2>(lhs.x / rhs.x, lhs.y / rhs.y, 0, 0);
}

// tvec4 /= tvec2
template<typename T1, typename T2>
tvec4<T1> & operator/=(tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs / rhs);
}

// tvec4 / tvec3
template<typename T1, typename T2>
tvec4<T1> operator/(const tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec4<T2>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, 0);
}

// tvec4 /= tvec3
template<typename T1, typename T2>
tvec4<T1> & operator/=(tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs / rhs);
}

// tvec4 / tvec4
template<typename T1, typename T2>
tvec4<T1> operator/(const tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

// tvec4 /= tvec4
template<typename T1, typename T2>
tvec4<T1> & operator/=(tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return (lhs = lhs / rhs);
}


//*******************************************************************************
// LHS: tvec4, operator: %
//*******************************************************************************
// scalar % tvec4
template<typename T1, typename T2>
tvec4<T2> operator%(const T1 & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs % rhs.x, lhs % rhs.y, lhs % rhs.z, lhs % rhs.w);
}

// tvec4 % scalar
template<typename T1, typename T2>
tvec4<T1> operator%(const tvec4<T1> & lhs, const T2 & rhs)
{
    return tvec4<T2>(lhs.x % rhs, lhs.y % rhs, lhs.z % rhs, lhs.w % rhs);
}

// tvec4 %= scalar
template<typename T1, typename T2>
tvec4<T1> & operator%=(tvec4<T1> & lhs, const T2 & rhs)
{
    return (lhs = lhs % rhs);
}

// tvec4 % tvec2
template<typename T1, typename T2>
tvec4<T1> operator%(const tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return tvec4<T2>(lhs.x % rhs.x, lhs.y % rhs.y, 0, 0);
}

// tvec4 %= tvec2
template<typename T1, typename T2>
tvec4<T1> & operator%=(tvec4<T1> & lhs, const tvec2<T2> & rhs)
{
    return (lhs = lhs % rhs);
}

// tvec4 % tvec3
template<typename T1, typename T2>
tvec4<T1> operator%(const tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return tvec4<T2>(lhs.x % rhs.x, lhs.y % rhs.y, lhs.z % rhs.z, 0);
}

// tvec4 %= tvec3
template<typename T1, typename T2>
tvec4<T1> & operator%=(tvec4<T1> & lhs, const tvec3<T2> & rhs)
{
    return (lhs = lhs % rhs);
}

// tvec4 % tvec4
template<typename T1, typename T2>
tvec4<T1> operator%(const tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return tvec4<T2>(lhs.x % rhs.x, lhs.y % rhs.y, lhs.z % rhs.z, lhs.w % rhs.w);
}

// tvec4 %= tvec4
template<typename T1, typename T2>
tvec4<T1> & operator%=(tvec4<T1> & lhs, const tvec4<T2> & rhs)
{
    return (lhs = lhs % rhs);
}



//*******************************************************************************
// Equality tests
//*******************************************************************************
template<typename T1>
tvec2<T1> operator==(const tvec2<T1> & lhs, const tvec2<T1> & rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<typename T1>
tvec2<T1> operator!=(const tvec2<T1> & lhs, const tvec2<T1> & rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

template<typename T1>
tvec3<T1> operator==(const tvec3<T1> & lhs, const tvec3<T1> & rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<typename T1>
tvec3<T1> operator!=(const tvec3<T1> & lhs, const tvec3<T1> & rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}

template<typename T1>
tvec4<T1> operator==(const tvec4<T1> & lhs, const tvec4<T1> & rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

template<typename T1>
tvec4<T1> operator!=(const tvec4<T1> & lhs, const tvec4<T1> & rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w;
}


} // namespace m3d
#endif // VECOPS_H
