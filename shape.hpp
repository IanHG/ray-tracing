#pragma once
#ifndef RAY_TRACER_SHAPE_HPP_INCLUDED
#define RAY_TRACER_SHAPE_HPP_INCLUDED

#include <vector>

#include "static_vector.hpp"
#include "ray.hpp"
#include "color.hpp"

class Shape
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

      virtual bool intersect(intersection& i)
      {
         ray local_ray     = i.r;
         local_ray.origin -= this->center;

         float a = local_ray.direction.magnitude2();
         float b = 2.0f * dot(local_ray.direction, local_ray.origin);
         float c = local_ray.origin.magnitude2() - radius * radius;

         float discriminant = b * b - 4 * a * c;

         if(discriminant < 0.0f)
         {
            return false;
         }

         float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
         float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

         if(t1 > RAY_T_MIN && t1 < i.t)
         {
            i.t = t1;
         }
         else if(t2 > RAY_T_MIN && t2 < i.t)
         {
            i.t = t2;
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
         ray local_ray     = r;
         local_ray.origin -= this->center;

         float a = local_ray.direction.magnitude2();
         float b = 2.0f * dot(local_ray.direction, local_ray.origin);
         float c = local_ray.origin.magnitude2() - radius * radius;

         float discriminant = b * b - 4 * a * c;

         if(discriminant < 0.0f)
         {
            return false;
         }

         float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
         if(t1 > RAY_T_MIN && t1 < r.tmax)
         {
            return true;
         }
         
         float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);
         if(t2 > RAY_T_MIN && t2 < r.tmax)
         {
            return true;
         }

         return false;
      }

};

#endif /* RAY_TRACER_SHAPE_HPP_INCLUDED */
