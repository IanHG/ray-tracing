#pragma once
#ifndef RAY_TRACER_RAY_HPP_INCLUDED
#define RAY_TRACER_RAY_HPP_INCLUDED

#include "static_vector.hpp"
#include "color.hpp"

// RAY_T_MIN, to prevent bouncing rays from self-interacting
#define RAY_T_MIN 0.0001f
#define RAY_T_MAX 1.0e30f

/**
 *
 **/
struct ray
{
   vector3f origin;
   vector3f direction;
   float    tmax;

   ray
      (  const vector3f& origin    = vector3f{0.0f, 0.0f, 0.0f}
      ,  const vector3f& direction = vector3f{0.0f, 1.0f, 0.0f}
      ,  float tmax                = RAY_T_MAX
      )
      :  origin   {origin}
      ,  direction{direction}
      ,  tmax     {tmax}
   {
   }

   ~ray() = default;

   ray(const ray& other) = default;

   ray& operator=(const ray& other) = default;

   vector3f calculate(float t) const
   {
      return origin +  t * direction;
   }
};

class Shape;

/**
 * MORE DETAILS TO COME FOR THIS CLASS
 **/
struct intersection
{
   ray   r;
   float t       = RAY_T_MAX;
   Shape *pshape = nullptr;
   color_rgb color;

   intersection() = default;

   intersection(const intersection& i) = default;
   intersection(const ray& r)
      :  r(r)
      ,  t(r.tmax)
   {
   }

   intersection& operator=(const intersection& i) = default;

   bool intersected() const
   {
      return bool(pshape);
   }

   vector3f position() const
   {
      return r.calculate(t);
   }
};

#endif /* RAY_TRACER_RAY_HPP_INCLUDED */
