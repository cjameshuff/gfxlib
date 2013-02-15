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

#ifndef MISC3D_H
#define MISC3D_H

#include <algorithm>
#include <cmath>

namespace m3d {

//****************************************************************************************************
//****************************************************************************************************

template<typename T>
inline T max3(T a, T b, T c) {return std::max(std::max(a, b), c);}

template<typename T>
inline T min3(T a, T b, T c) {return std::min(std::min(a, b), c);}

template<typename T> T clamp(const T & t) {return std::min(1.0, std::max(0.0, t));}
template<> inline long double clamp(const long double & t) {return fminl(1.0, fmaxl(0.0, t));}
template<> inline double clamp(const double & t) {return fmin(1.0, fmax(0.0, t));}
template<> inline float clamp(const float & t) {return fminf(1.0f, fmaxf(0.0f, t));}

template<typename T> T clamp(const T & t, const T & mn, const T & mx) {
    return std::min(mx, std::max(mn, t));
}


// returns:
// -1 if negative
// 0 if 0
// +1 if positive
template<typename T>
inline T sign(T n) {
    if(n == 0) return 0;
    else if(n > 0) return 1;
    else return -1;
}

template<typename T>
inline T sqr(T n) {return n*n;}

template<typename T>
inline T cub(T n) {return n*n*n;}

// Linear interpolation
template<typename tT, typename vT>
inline vT lerp(const tT & v, const vT & a, const vT & b) {return v*(b - a) + a;}

// A useful polynomial S-curve function, assumes x in range [0, 1].
// 3x^2 - 2*x^3
template<typename T>
inline T scurve(T x) {return x*x*(3 - 2*x);}

// A higher polynomial variation of scurve with both first and second derivatives
// equal to 0 at x = 0 and 1.
// Uses the polynomial curve 6*x^5 - 15*x^4 + 10*x^3
template<typename T>
inline T scurve2(T x) {return ((6*x - 15)*x + 10)*x*x*x;}


template<typename T>
T deg2rad(T n) {return n*(M_PI/180.0);}

template<typename T>
T rad2deg(T n) {return n*(180.0/M_PI);}


//template<typename T>
//T factorial(T n)
//{
//    T fact = 1;
//    for(T j = 2; j <= n; ++j)
//        fact *= j;
//    return fact;
//}

// This function computes a hash value of a string, returning an unsigned int
// in the range [0, tableSize].
// Based on code from "The Practice of Programming" by
// Brian W. Kernighan and Rob Pike
/*inline unsigned int HashString(const char * str, unsigned int tableSize)
{
    unsigned int h = 0;
    const int kStringHashMult = 31;
    for(unsigned char * p = (unsigned char *)str; *p != '\0'; p++)
        h = kStringHashMult*h + *p;
    
    return h % tableSize;
}*/


//************************************************************************************************
//************************************************************************************************
// Mean and variance
// (see Knuth, or <http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance>)
//************************************************************************************************
//************************************************************************************************

// Accumulate mean into m, returning mean.
// n is count of samples.
template<typename T> T & accum_mean(T & m, const T & x, int n) {
    return m += (x - m)/(n + 1);
}

// Accumulate variance into s, returning variance.
// n is count of samples.
template<typename T> T & accum_variance(T & s, const T & m_old, const T & m_new, const T & x, int n) {
    return s = (n*s + (x - m_new)*(x - m_old))/(n + 1);
}

// Accumulate mean and variance into m and s respectively, returning mean.
// n is count of samples.
template<typename T> T & accum_mean_var(T & m, T & s, const T & x, int n) {
    T m_old = m;
    accum_mean(m, x, n);
    accum_variance(s, m_old, m, x, n);
    return m;
}



//************************************************************************************************
//************************************************************************************************
// Derivatives
//************************************************************************************************
//************************************************************************************************

const double kEpsilonDefault = 0.0001;

// Individual derivatives of a function, from first to fourth.
template<double (*Fn)(double)>
double derivative(double x, double epsilon = kEpsilonDefault) {
    return (Fn(x + epsilon) - Fn(x - epsilon))/(2.0*epsilon);
}

template<double (*Fn)(double)>
double derivative2(double x, double epsilon = kEpsilonDefault) {
    return (Fn(x - epsilon) - Fn(x)*2 + Fn(x + epsilon))/(epsilon*epsilon);
}

template<double (*Fn)(double)>
double derivative3(double x, double epsilon = kEpsilonDefault) {
    return (Fn(x + 2*epsilon) - 2*Fn(x + epsilon) + 2*Fn(x - epsilon) - Fn(x - 2*epsilon))
        /(2*epsilon*epsilon*epsilon);
}

template<double (*Fn)(double)>
double derivative4(double x, double epsilon = kEpsilonDefault) {
    return (Fn(x + 2*epsilon) - 4*Fn(x + epsilon) + 6*Fn(x) - 4*Fn(x - epsilon) + Fn(x - 2*epsilon))
        /(epsilon*epsilon*epsilon*epsilon);
}

// Simultaneous computation of function value and derivatives, up to fourth derivative.
// These functions can be more efficient if multiple derivatives are needed,
// because they reduce duplication of function evaluations with the same arguments.

// Compute the function and first and second derivatives.
// Uses 3 function evaluations.
template<double (*Fn)(double)>
double derivatives(double x, double & d1, double & d2, double epsilon = kEpsilonDefault) {
    double f_xme = Fn(x - epsilon);
    double f_xpe = Fn(x + epsilon);
    double f_x = Fn(x);
    d1 = (f_xpe - f_xme)/(2.0*epsilon);
    d2 = (f_xme - 2*f_x + f_xpe)/(epsilon*epsilon);
    return f_x;
}

// Compute the function and first, second, and third derivatives.
// Uses 5 function evaluations.
template<double (*Fn)(double)>
double derivatives(double x, double & d1, double & d2, double & d3, double epsilon = kEpsilonDefault) {
    double f_xme = Fn(x - epsilon);
    double f_xpe = Fn(x + epsilon);
    double f_xm2e = Fn(x - 2*epsilon);
    double f_xp2e = Fn(x + 2*epsilon);
    double f_x = Fn(x);
    d1 = (f_xpe - f_xme)/(2.0*epsilon);
    d2 = (f_xme - 2*f_x + f_xpe)/(epsilon*epsilon);
    d3 = (f_xp2e - 2*f_xpe + 2*f_xme - f_xm2e)/(2*epsilon*epsilon*epsilon);
    return f_x;
}


// Compute the function and first, second, third, and fourth derivatives.
// Uses 5 function evaluations.
template<double (*Fn)(double)>
double derivatives(double x, double & d1, double & d2, double & d3, double & d4,
                   double epsilon = kEpsilonDefault)
{
    double f_xme = Fn(x - epsilon);
    double f_xpe = Fn(x + epsilon);
    double f_xm2e = Fn(x - 2*epsilon);
    double f_xp2e = Fn(x + 2*epsilon);
    double f_x = Fn(x);
    d1 = (f_xpe - f_xme)/(2.0*epsilon);
    d2 = (f_xme - 2*f_x + f_xpe)/(epsilon*epsilon);
    d3 = (f_xp2e - 2*f_xpe + 2*f_xme - f_xm2e)/(2*epsilon*epsilon*epsilon);
    d4 = (f_xp2e - 4*f_xpe + 6*f_x - 4*f_xme + f_xm2e)/(epsilon*epsilon*epsilon*epsilon);
    return f_x;
}


//************************************************************************************************
//************************************************************************************************
// Root solvers
//************************************************************************************************
//************************************************************************************************

template<double (*Fn)(double)>
double BisectFindRootR(double v1, double x1, double v2, double x2, double eps, int & iterations)
{
    if((x2 - x1) < eps) {
        double result = (v1*x1 - v1*x2)/(v2 - v1) + x1;
//        cout << "Converged to: " << result << " with " << iterations << " iterations." << endl;
        return result;
    }
    else {
//        cout << "Bisect interval: [" << x1 << ", " << x2 << "]. Values: " << v1 << ", " << v2 << endl;
//        cout << "\tApproximate root: " << (v1*x1 - v1*x2)/(v2 - v1) + x1 << endl;
        double mdPt = (x1 + x2)/2;
        double mdPtVal = Fn(mdPt);
        ++iterations;
        //choose the interval which straddles the root
        if(mdPtVal*v1 < 0)//if mdPtVal and v1 are of same sign, mdPtVal*v1 > 0
            return BisectFindRootR<Fn>(v1, x1, mdPtVal, mdPt, eps, iterations);
        else
            return BisectFindRootR<Fn>(mdPtVal, mdPt, v2, x2, eps, iterations);
    }
}

//bisection algorithm, recursive implementation
template<double (*Fn)(double)>
double BisectFindRoot(double x1, double x2, double eps = kEpsilonDefault) {
    int iterations = 0;
    if(x2 > x1)
        return BisectFindRootR<Fn>(Fn(x1), x1, Fn(x2), x2, eps, iterations);
    else
        return BisectFindRootR<Fn>(Fn(x2), x2, Fn(x1), x1, eps, iterations);
}


template<double (*Fn)(double)>
double SecantFindRoot(double x1, double x2, double eps = kEpsilonDefault, int maxN = 100){
    double a = Fn(x1);
    double b = Fn(x2);
    double tmp;
    double d;
    
    for(int j = 2; j < maxN; ++j) {
        if(fabs(a) > fabs(b)) {
            tmp = x1; x1 = x2; x2 = tmp;
            tmp = a; a = b; b = tmp;
        }
        
//        cout << "Secant interval: [" << x1 << ", " << x2 << "].\n\tValues: " << a << ", " << b << endl;
        
        d = a*(x2 - x1)/(b - a);//slope over interval
        x2 = x1;
        b = a;
//        d *= a;//horizontal offset of x-intercept from x1
        
//        cout << " d = " << d << endl;
        if(fabs(d) < eps) {
//            cout << "Converged to: " << x1 << " with " << j << " iterations." << endl;
            return x1;
        }
        x1 -= d;
        a = Fn(x1);
    }
//    cout << "Did not converge to within " << epsilon << " of value." << endl;
    return x1;
}



template<double (*Fn)(double), double (*FnD)(double)>
double NewtonFindRoot(double x, double eps = kEpsilonDefault,
                      double sigma = kEpsilonDefault, int maxN = 100)
{
    for(int j = 1; j < maxN; ++j) {
        double y = Fn(x);
        double dy = FnD(x);
//        cout << "NewtonFindRoot(): x = " << x << " f(x) = " << y << " f'(x) = " << dy << endl;
        if(fabs(dy) < sigma) {
/*            cout << "NewtonFindRoot(): Function derivative near 0.\tx = " << x
                 << ", f(x) = " << y
                 << ", f'(x) = " << dy
                 << "\n\t" << j << " iterations." << endl;*/
            return x;
        }
        double d = y/dy;
        x -= d;
        if(fabs(d) < sigma) {
//            cout << "NewtonFindRoot(): Converged to: " << x << " with " << j << " iterations." << endl;
            return x;
        }
    }
//    cout << "Did not converge to within " << epsilon << " of value." << endl;
    return x;
}

template<double (*Fn)(double)>
double NewtonFindRoot(double x, double eps = kEpsilonDefault,
                      double sigma = kEpsilonDefault, int maxN = 100)
{
    return NewtonFindRoot<Fn, derivative<Fn> >(x, eps, sigma, maxN);
}


template<double (*Fn)(double), double (*FnD)(double), double (*FnD2)(double)>
double OlverFindRoot(double x, double eps = kEpsilonDefault, int maxN = 100)
{
    for(int j = 0; j < maxN; ++j) {
        double y = Fn(x);
        double dy = FnD(x);
        double ddy = FnD2(x);
//        cout << "x = " << x << " f(x) = " << y << " f'(x) = " << dy << " f\"(x) = " << ddy << endl;
//        double d = y/dy + (ddy/(2*dy))*((y*y)/(dy*dy));
        double d = (y + (ddy*y*y)/(2*dy*dy))/dy;
        x -= d;
        if(fabs(d) < eps) {
//            cout << "Converged to: " << x << " with " << j + 1 << " iterations." << endl;
            return x;
        }
    }
    return x;
}

/*template<double (*Fn)(double)>
double OlverFindRoot(double x, double eps = epsilon, int maxN = 100)
{
    for(int j = 0; j < maxN; ++j) {
        double dy = FnD(x);
        double ddy = FnD2(x);
        double y = Derivatives<Fn>(x, dy, ddy);
//        cout << "x = " << x << " f(x) = " << y << " f'(x) = " << dy << " f\"(x) = " << ddy << endl;
        double d = (y + (ddy*y*y)/(2*dy*dy))/dy;
        x -= d;
        if(fabs(d) < epsilon) {
//            cout << "Converged to: " << x << " with " << j + 1 << " iterations." << endl;
            return x;
        }
    }
    return x;
}*/


//************************************************************************************************
//************************************************************************************************
// Numeric integration
//************************************************************************************************
//************************************************************************************************

// Trapezoidal integration
template<double (*Fn)(double)>
double TrapIntegrate(double a, double b, int steps = 100)
{
    double sum = (Fn(a) + Fn(b))/2;
    double xdelt = (b - a)/steps;
    for(size_t j = 1; j < steps; ++j)
        sum += Fn(a + j*xdelt);
    
    return sum*xdelt;
}

// Templatized step count version. Compiler can unroll loop, but step count must be specified
// at compile time. A smart compiler may be able to do this with the first version, which
// would make this version obsolete. TODO: benchmark!
template<double (*Fn)(double), int steps>
double TrapIntegrate(double a, double b)
{
    double sum = (Fn(a) + Fn(b))/2;
    double xdelt = (b - a)/steps;
    for(size_t j = 1; j < steps; ++j)
        sum += Fn(a + j*xdelt);
    
    return sum*xdelt;
}


// Integrate using Simpson's rule. Number of steps must be odd and > 5.
// Simson's rule gives very good accuracy for most functions, and gives exact
// results for polynomials of degree 3 or lower.
// sample weights are:
// 1 4 2 4 1
// 1 4 2 4 2 4 1
// 1 4 2 4 2 ... 4 2 4 1
template<double (*Fn)(double), int steps>
double SimpsonIntegrate(double a, double b)
{
    double ones = Fn(a) + Fn(b);
    double xdelt = (b - a)/steps;
    
    // Every other sample excluding the first and last, starting at the
    // third sample and ending at the third from last.
    double twos = 0;
    for(int j = 2, n = steps - 2; j < n; j += 2)
        twos += Fn(xdelt*j + a);
    
    // Every other sample excluding the first and last, starting at the
    // second sample and ending at the second from last.
    double fours = 0;
    for(int j = 1; j < steps; j += 2)//FIXME? j < steps - 1?
        fours += Fn(xdelt*j + a);
    
    return (ones + 2*twos + 4*fours)*xdelt/3;
}


// Romberg extrapolation of trapezoidal rule.
// Recursively find progressively refined estimates with the trapezoidal rule,
// placing estimates in an array. Then do successive Romberg extrapolations, 
// reducing the number of estimates until only one is left.
// This method is highly accurate, but the large number of extrapolation
// computations may cause excessive overhead.
/*template<double (*Fn)(double), int steps>
double RombergIntegrate(double a, double b)
{
    double sum = (Fn(a) + Fn(b))/2;
    double xdelt = (b - a)/steps;
    for(double x = xdelt; x < b; x += xdelt)
        sum += Fn(x);
    
    return sum*xdelt;
}*/


// 1, 2, and 3D Monte-Carlo integration.
// For the 1D case, Monte-Carlo integration usually gives poor accuracy
// for the number of samples. However, it can be a useful estimate for
// higher dimensional functions.
/*class RNG;
template<double (*Fn)(double, double), int samples, RNG * Rand>
double MonteCarloIntegrate(double a1, double b1)
{
    double sum = 0;
    for(int j = 0; j < samples; ++j)
        sum += Fn((*Rand)(a1, b1));
    
    return sum/samples;
}


template<double (*Fn)(double, double), int samples, RNG * Rand>
double MonteCarloIntegrate(double a1, double b1, double a2, double b2)
{
    double sum = 0;
    for(int j = 0; j < samples; ++j)
        sum += Fn((*Rand)(a1, b1), (*Rand)(a2, b2));
    
    return sum/samples;
}


template<double (*Fn)(double, double), int samples, RNG * Rand>
double MonteCarloIntegrate(double a1, double b1, double a2, double b2, double a3, double b3)
{
    double sum = 0;
    for(int j = 0; j < samples; ++j)
        sum += Fn((*Rand)(a1, b1), (*Rand)(a2, b2), (*Rand)(a3, b3));
    
    return sum/samples;
}*/

}// namespace m3d

#endif