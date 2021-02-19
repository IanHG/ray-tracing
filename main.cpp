#include <iostream>

#include "static_vector.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include "image.hpp"
#include "Camera.hpp"

#define PI 3.141592f
   

color_rgb ray_trace
   (  const ray& r, Shape* scene, light* lig, int depth = 64
   )
{
   if(depth == 0)
      return {};

   intersection inter(r);
   if(scene->intersect(inter))
   {
		//diffusion
		auto diffused_rgb = lig->enlighten(inter, scene);
		
		//reflection
		vector3f reflected_vec = r.direction - 2 * dot(r.direction, inter.normal) * inter.normal;
   	auto reflected_rgb     = ray_trace(ray{inter.position(), reflected_vec}, scene, lig, depth - 1);


		float k = inter.pshape->get_reflection();
		auto result_rgb = k * reflected_rgb + (1 - k) * diffused_rgb;
      result_rgb.clamp(0.0f, 1.0f);

		return result_rgb;
   }
   else
   {
      return {};
   }
}

void render_image
   (  image& img, Camera* cam, Shape* scene, light* lig
   )
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

         color_rgb result = ray_trace(r, scene, lig);

         *(img.get_pixel(x, y)) = result;
      }
   }
}

int main(int argc, const char* argv[])
{
   //int width  = 480;
   //int height = 360;
   //int width  = 800;
   //int height = 600;
   int width  = 1920;
   int height = 1080;
   
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
      {  vector3f {0.0f, 0.0f, 0.0f}
      ,  vector3f {0.0f, 1.0f, 0.0f}
      ,  color_rgb{1.0f, 0.0f, 0.0f}
      };
   scene.add_shape(&floor);

   Sphere sphere1
      {  vector3f{0.0f, 1.0f, 0.0f}
      ,  1.0f
      ,  color_rgb{0.0f, 1.0f, 0.0f}
      };
   Sphere sphere2
      {  vector3f{-0.7f, 1.0f, -3.0f}
      ,  0.7f
      ,  color_rgb{0.0f, 0.0f, 1.0f}
      };
   Sphere sphere3
      {  vector3f{-0.7f, 1.0f, 3.0f}
      ,  0.7f
      ,  color_rgb{0.0f, 0.0f, 1.0f}
      };
   sphere2.set_reflection(0.3);
   scene.add_shape(&sphere1);
   scene.add_shape(&sphere2);
   scene.add_shape(&sphere3);

   light lig;
   //lig.add_light(vector3f{+1000.0f, 1000.0f, 0.0f}, 4 * color_rgb{0.5f, 0.3f, 0.8f});
   //lig.add_light(vector3f{-1000.0f, 1000.0f, 0.0f}, 4 * color_rgb{0.5f, 0.3f, 0.8f});
   lig.add_light(vector3f{0.0f, 1000.0f,  1500.0f}, 4 * color_rgb{0.5f, 0.3f, 0.8f});
   //lig.add_light(vector3f{0.0f, 1000.0f, -1500.0f}, 4 * color_rgb{0.5f, 0.6f, 0.2f});

   render_image(img, &cam, &scene, &lig);

   img.save_image("img.ppm", 1.0, 2.2);

   return 0;
}
