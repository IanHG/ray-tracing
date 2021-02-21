#pragma once
#ifndef RAY_TRACER_SHAPE_HPP_INCLUDED
#define RAY_TRACER_SHAPE_HPP_INCLUDED

#include <vector>

#include "static_vector.hpp"
#include "ray.hpp"
#include "color.hpp"
#include "util.hpp"

class reflective_shape
{
   public:
	   void set_reflection(float temp)
	   { 
         reflection = temp; 
      }

	   void set_brightness(float temp)
	   {  
         brightness = temp; 
      }

	   float get_reflection() const
	   {  
         return reflection; 
      }

	   float get_brightness() const
	   { 
         return brightness; 
      }

   private:
	   float brightness = 1.0f;
	   float reflection = 0.5f;
};

class Shape
   :  public reflective_shape
{
   public:
      virtual ~Shape() = 0;

      virtual bool intersect(intersection& i)   = 0;
      virtual bool does_intersect(const ray& r) = 0;
};


Shape::~Shape()
{
}

class ShapeSet
   :  public Shape
{
   protected:
      std::vector<Shape*> shapes;

   public:
      ShapeSet()
      {
      }

      virtual ~ShapeSet()
      {
      }

      void add_shape(Shape* shape)
      {
         shapes.push_back(shape);
      }
      
      virtual bool intersect(intersection& i)
      {
         bool does_intersect = false;

         for(const auto& shape : shapes)
         {
            if(shape->intersect(i))
            {
               does_intersect = true;
            }
         }
         return does_intersect;
      }

      virtual bool does_intersect(const ray& r)
      {
         for(const auto& shape : shapes)
         {
            if(shape->does_intersect(r))
            {
               return true;
            }
         }
         return false;
      }
};

class Plane
   :  public Shape
{
   protected:
      vector3f position;
      vector3f normal;
      color_rgb color;

   public:
      Plane
         (  const vector3f& position
         ,  const vector3f& normal
         ,  const color_rgb& color = color_rgb{1.0f, 1.0f, 1.0f}
         )
         :  position(position)
         ,  normal  (normal)
         ,  color   (color)
      {
      }

      virtual ~Plane() 
      {
      }

      virtual bool intersect(intersection& i)
      {
         float d_dot_n = dot(i.r.direction, normal);

         if(d_dot_n == 0.0f) // Float eq??
         {
            return false;
         }
         
         float inv_d_dot_n = 1.0f / d_dot_n;
         float t = dot(position - i.r.origin, normal) * inv_d_dot_n;

         if(t <= RAY_T_MIN || t >= i.t)
         {
            return false;
         }

         i.t      = t;
         i.pshape = this;
         i.color  = this->color;
		   if (d_dot_n > 0.0f)
            i.normal = -normal;
		   else
		      i.normal = normal;

         return true;
      }

      virtual bool does_intersect(const ray& r)
      {
         float d_dot_n = dot(r.direction, normal);
         
         if(d_dot_n == 0.0f) // Float eq??
         {
            return false;
         }
         
         float inv_d_dot_n = 1.0f / d_dot_n;
         float t = dot(position - r.origin, normal) * inv_d_dot_n;
         
         if(t <= RAY_T_MIN || t >= r.tmax)
         {
            return false;
         }

         return true;
      }

};

class Sphere
   :  public Shape
{
   protected:
      vector3f  center;
      float     radius;
      color_rgb color;

   public:
      Sphere
         (  const vector3f& center
         ,  float radius
         ,  const color_rgb& color = color_rgb{1.0f, 1.0f, 1.0f}
         )
         :  center(center)
         ,  radius(radius)
         ,  color(color)
      {
      }

      virtual ~Sphere() 
      {
      }

      void set_center(const vector3f& pos)
      {
         center = pos;
      }

      virtual bool intersect(intersection& i)
      {
         vector3f l = i.r.origin - center;
         float a    = dot(i.r.direction, i.r.direction);
         float b    = 2.0f * dot(i.r.direction, l);
         float c    = dot(l, l) - radius * radius;

         float t1, t2;
         if(!solve_quadratic(a, b, c, t1, t2))
         {
            return false;
         }

         if(t1 > RAY_T_MIN && t1 < i.t)
         {
            i.t = t1;
            float inv_radius = 1.0f / radius;
            i.normal = (i.position() - center) * inv_radius;
         }
         else if(t2 > RAY_T_MIN && t2 < i.t)
         {
            i.t = t2;
            float inv_radius = 1.0f / radius;
            i.normal = -(i.position() - center) * inv_radius;
         }
         else
         {
            return false;
         }

         i.pshape = this;
         i.color  = this->color;

         return true;
      }

      virtual bool does_intersect(const ray& r)
      {
         vector3f l = r.origin - center;
         float a    = dot(r.direction, r.direction);
         float b    = 2.0f * dot(r.direction, l);
         float c    = dot(l, l) - radius * radius;

         float t1, t2;
         if(!solve_quadratic(a, b, c, t1, t2))
         {
            return false;
         }
         
         if(t1 > RAY_T_MIN && t1 < r.tmax)
         {
            return true;
         }
         
         if(t2 > RAY_T_MIN && t2 < r.tmax)
         {
            return true;
         }

         return false;
      }

};

#endif /* RAY_TRACER_SHAPE_HPP_INCLUDED */
