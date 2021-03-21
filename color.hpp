#pragma once
#ifndef RAY_TRACER_COLOR_HPP_INCLUDED
#define RAY_TRACER_COLOR_HPP_INCLUDED

#include <cmath>

#include "util.hpp"

/**
 * -------------------------------------
 * Additive mixing:
 * -------------------------------------
 *    r = r_1 + r_2
 *    g = g_1 + g_2
 *    b = b_1 + b_2
 *
 * -------------------------------------
 * Multiplicative mixing:
 * -------------------------------------
 *    r = r_1 * r_2
 *    g = g_1 * g_2
 *    b = b_1 * b_2
 *
 * Represents absorption. Taking color out.
 *
 * -------------------------------------
 * Gamma correction
 * -------------------------------------
 * 
 *    out = (exposure_value * in) ^ gamma
 *
 *    traditionally gamma = 2.2
 *       ^ darker image
 *       ̣↓ brighter image
 *
 * This will convert from internal linear scheme, 
 * to a more realistic logarithmic scheme.
 **/

struct color_rgb
{
   float r = 0.0f;
   float g = 0.0f;
   float b = 0.0f;

   color_rgb() = default;

   color_rgb(float l)
      :  r(l), g(l), b(l)
   {
   }

   color_rgb(float r, float g, float b)
      :  r(r), g(g), b(b)
   {
   }

   void clamp(float min = 0.0f, float max = 1.0f)
   {
      this->r = ::clamp(this->r, min, max); 
      this->g = ::clamp(this->g, min, max); 
      this->b = ::clamp(this->b, min, max); 
   }

   void apply_gamma_correction(float exposure, float gamma)
   {
      this->r = std::pow((exposure * this->r), gamma);
      this->g = std::pow((exposure * this->g), gamma);
      this->b = std::pow((exposure * this->b), gamma);
   }

   color_rgb& operator=(const color_rgb& c)
   {
      if(this != &c)
      {
         this->r = c.r;
         this->g = c.g;
         this->b = c.b;
      }
      return *this;
   }
   
   color_rgb& operator+=(const color_rgb& c)
   {
      this->r += c.r;
      this->g += c.g;
      this->b += c.b;
      return *this;
   }
   
   color_rgb& operator*=(const color_rgb& c)
   {
      this->r *= c.r;
      this->g *= c.g;
      this->b *= c.b;
      return *this;
   }
   
   color_rgb& operator*=(const float f)
   {
      this->r *= f;
      this->g *= f;
      this->b *= f;
      return *this;
   }
   
   color_rgb& operator/=(const float f)
   {
      this->r /= f;
      this->g /= f;
      this->b /= f;
      return *this;
   }
};

inline color_rgb operator+(const color_rgb& c1, const color_rgb& c2)
{
   color_rgb color{c1.r, c1.g, c1.b};
   color += c2;
   return color;
}

inline color_rgb operator*(const color_rgb& c1, const color_rgb& c2)
{
   color_rgb color{c1.r, c1.g, c1.b};
   color *= c2;
   return color;
}

inline color_rgb operator*(const color_rgb& c, float f)
{
   return {c.r * f, c.g * f, c.b * f};
}

inline color_rgb operator*(float f, const color_rgb& c)
{
   return c * f;
}

inline color_rgb operator/(const color_rgb& c, float f)
{
   return {c.r / f, c.g / f, c.b / f};
}

inline color_rgb operator/(float f, const color_rgb& c)
{
   return c / f;
}

inline std::ostream& operator<<(std::ostream& ostr, const color_rgb& col)
{
   ostr << "R" << col.r
        << "G" << col.g
        << "B" << col.b
        ;
   return ostr;
}

inline float convert_to_grayscale(const color_rgb& col)
{
   //return 0.299f * col.r + 0.587f * col.g + 0.114f * col.b;
   //return 0.2126f * col.r + 0.7152f * col.g + 0.0722f * col.b;
   return (col.r + col.g + col.b)/ 3.0f;
}

// luminance formula credits: https://stackoverflow.com/a/596243
// >>> Luminance = 0.2126*R + 0.7152*G + 0.0722*B <<<
inline float luminance(const color_rgb col)
{
   return 0.299f * col.r + 0.587f * col.g + 0.114f * col.b;
   //return 0.2126f * col.r + 0.7152f * col.g + 0.0722f * col.b;
}

#endif /* RAY_TRACER_COLOR_HPP_INCLUDED */
