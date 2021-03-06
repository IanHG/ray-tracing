#pragma once
#ifndef RAY_TRACER_UTIL_HPP_INCLUDED
#define RAY_TRACER_UTIL_HPP_INCLUDED

#include <cstdlib>

/**
 * Clamp a value between minimum and maximum.
 **/
inline float clamp(float value, float min, float max)
{
   if (value < min) return min;
   if (value > max) return max;
   return value;
}

/**
 *
 **/
template<class T>
inline T random_floating_point() 
{
   // Returns a random real in [0,1).
   return rand() / (RAND_MAX + static_cast<T>(1.0));
}

/**
 *
 **/
template<class T>
inline T random_floating_point(T min, T max) 
{
   // Returns a random real in [min,max).
   return min + (max-min)*random_floating_point<T>();
}


/**
 *
 **/
inline bool solve_quadratic
   (  const float a
   ,  const float b
   ,  const float c
   ,  float& x0
   ,  float& x1
   )
{ 
   float discr = b * b - 4.0f * a * c; 
   if (discr < 0.0f) 
   {
      return false;
   }
   else if (discr == 0.0f) 
   {
      x0 = x1 = - 0.5f * b / a; 
   }
   else 
   { 
      float q 
         = (b > 0.0f) 
         ? -0.5f * (b + sqrt(discr)) 
         : -0.5f * (b - sqrt(discr))
         ; 
      x0 = q / a; 
      x1 = c / q; 
   } 
   if (x0 > x1) 
   {
      std::swap(x0, x1);
   }
   
   return true;
} 

#endif /* RAY_TRACER_UTIL_HPP_INCLUDED */
