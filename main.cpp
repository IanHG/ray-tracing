#include <iostream>

#include "static_vector.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include "image.hpp"
#include "Camera.hpp"

#define PI 3.14159
   
template<class T, int N>
std::ostream& operator<<(std::ostream& ostr, const static_vector<T, N>& vec)
{
   for(int i = 0; i < N; ++i)
   {
      ostr << vec.at(i) << " ";
   }
   return ostr;
}

void ray_trace
   (  image& img, Camera* cam, Shape* scene)
{
   for(int x = 0; x < img.get_width(); ++x)
   {
      for(int y = 0; y < img.get_height(); ++y)
      {
         vector2f screen_coord
            {  ( 2.0f * x) / img.get_width()  - 1.0f
            ,  (-2.0f * y) / img.get_height() + 1.0f
            };

         ray r = cam->make_ray(screen_coord);

         float* cur_pixel = img.get_pixel(x, y);

         intersection inter(r);
         if(scene->intersect(inter))
         {
            *cur_pixel = 1.0f;
         }
         else
         {
            *cur_pixel = 0.0f;
         }
      }
   }
}

int main(int argc, const char* argv[])
{
   int width  = 480;
   int height = 360;
   //int width  = 800;
   //int height = 600;
   //int width  = 1920;
   //int height = 1080;
   
   image img(width, height);
   
   PerspectiveCamera cam
      {  vector3f{-5.0f, 1.0f, 0.0f}
      ,  vector3f{ 0.0f, 1.0f, 0.0f}
      ,  vector3f{ 0.0f, 1.0f, 0.0f}
      ,  PI / 4.0f
      ,  (float) width / (float) height
      };

   ShapeSet scene;
   
   Plane floor
      {  vector3f{0.0f, 0.0f, 0.0f}
      ,  vector3f{0.0f, 1.0f, 0.0f}
      };
   scene.add_shape(&floor);

   Sphere sphere
      {  vector3f{0.0f, 1.0f, 0.0f}
      ,  1.0f
      };
   scene.add_shape(&sphere);

   ray_trace(img, &cam, &scene);

   img.save_image("img.ppm");

   return 0;
}
