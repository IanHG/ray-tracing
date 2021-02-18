#pragma once
#ifndef RAY_TRACER_RAY_HPP_INCLUDED
#define RAY_TRACER_RAY_HPP_INCLUDED

#include <vector>

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
   vector3f normal;

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

#include "shape.hpp"

/**
 *
 **/
class light
{
   public:
   	void add_light(const vector3f& source, const color_rgb& intensity)
   	{
   		this->source.push_back(source);
   		this->intensity.push_back(intensity);
   		n++;
   	}
   
   	color_rgb enlighten(const intersection& inter, Shape* scene) const
   	{
   		color_rgb light_rgb = {0.0f, 0.0f, 0.0f};
   		for (int i = 0; i < n; ++i)
   		{
			   ray to_light(inter.position(), (source[i] - inter.position()).normalized());
   			color_rgb diffused{};
   			if (!scene->does_intersect(to_light))
            {
   			   //diffused += intensity[i] * inter.pshape->get_brightness() * dot(to_light.direction, inter.normal);
   			   diffused += intensity[i] * inter.color * dot(to_light.direction, inter.normal);
            }
   			if (diffused.r >= 0.0f)
   			   light_rgb.r += diffused.r;
   			if (diffused.g >= 0.0f)
   			   light_rgb.g += diffused.g;
   			if (diffused.b >= 0.0f)
   			   light_rgb.b += diffused.b;
   		}
      
   		return light_rgb;
   	}
   private:
   	std::vector<vector3f>  source;
   	std::vector<color_rgb> intensity;
   	int n = 0;
};

#endif /* RAY_TRACER_RAY_HPP_INCLUDED */
