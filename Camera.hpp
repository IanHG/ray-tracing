#pragma once
#ifndef RAY_TRACER_CAMERA_HPP_INCLUDED
#define RAY_TRACER_CAMERA_HPP_INCLUDED

#include <cmath>

class Camera
{
   public:
      virtual ~Camera()
      {
      }

      virtual ray make_ray(const vector2f& point) const = 0;
};

class PerspectiveCamera
   :  public Camera
{
   public:
      PerspectiveCamera
         (  const vector3f& origin
         ,  const vector3f& target
         ,  const vector3f& upguide
         ,  float fov
         ,  float aspect_ratio
         )
         :  origin(origin)
      {
         forward = (target - origin).normalized();
         right   = cross(forward, upguide).normalized();
         up      = cross(right, forward);

         h = std::tan(fov);
         w = h * aspect_ratio;
      }

      virtual ray make_ray(const vector2f& point) const
      {
         vector3f direction 
            =  this->forward 
            +  (point[0] * w) * right
            +  (point[1] * h) * up
            ;

         return ray{origin, direction.normalized()};
      }

      void set_position(const vector3f& pos)
      {
         origin = pos;
      }
      
      void set_direction(float alfa, float beta)
      {
          forward = vector3f(std::cos(beta) * std::cos(alfa), std::cos(beta) * std::sin(alfa), std::sin(beta));
          right = vector3f(-std::sin(alfa), std::cos(alfa), 0.0f);
          up = cross(forward, right);
      }

   private:
      // Camera origin
      vector3f origin;
      
      // Unit guides
      vector3f forward;
      vector3f up;
      vector3f right;
      
      // Width and height of capture screen
      float h;
      float w;
};

#endif /* RAY_TRACER_CAMERA_HPP_INCLUDED */
