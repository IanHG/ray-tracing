#include <iostream>
#include <unistd.h>

#include "static_vector.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include "image.hpp"
#include "Camera.hpp"
#include "screen.hpp"
#include "frame.hpp"

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

void render_screen
   (  Screen& scr, Camera* cam, Shape* scene, light* lig
   )
{
   for(int x = 0; x < scr.get_width(); ++x)
   {
      for(int y = 0; y < scr.get_height(); ++y)
      {
         vector2f screen_coord
            {  ( 2.0f * x) / scr.get_width()  - 1.0f
            ,  (-2.0f * y) / scr.get_height() + 1.0f
            };

         ray r = cam->make_ray(screen_coord);

         color_rgb result = ray_trace(r, scene, lig);

         auto result_gray = static_cast<unsigned char>(clamp(convert_to_grayscale(result), 0.0f, 1.0f) * 255.0f);
         
         scr.draw_point(x, y, result_gray);
      }
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

void render_image_ssaa
   (  image& img, Camera* cam, Shape* scene, light* lig
   )
{
   float jitter_matrix[4 * 2] = 
   {  -1.0/4.0,  3.0/4.0
   ,   3.0/4.0,  1.0/4.0
   ,  -3.0/4.0, -1.0/4.0
   ,   1.0/4.0, -3.0/4.0
   };

   for(int x = 0; x < img.get_width(); ++x)
   {
      for(int y = 0; y < img.get_height(); ++y)
      {
         for(int sample = 0; sample < 4; ++sample)
         {
            vector2f screen_coord
               {  ( 2.0f * (x + jitter_matrix[2 * sample + 0])) / img.get_width()  - 1.0f
               ,  (-2.0f * (y + jitter_matrix[2 * sample + 1])) / img.get_height() + 1.0f
               };

            ray r = cam->make_ray(screen_coord);

            color_rgb result = /*0.5f * */ ray_trace(r, scene, lig);

            *(img.get_pixel(x, y)) += result;
         }
         
         auto scale = 1.0f / 4.0f;
         *(img.get_pixel(x, y)) *= scale;
      }
   }
}


int main(int argc, const char* argv[])
{
   int width  = 210;
   int height = 2 * 56;
   //int width  = 800;
   //int height = 600;
   //int width  = 1920;
   //int height = 1080;
   
   image img(width, height);

   vector3f position{-5.0f, 1.0f, 0.0f};
   float    pitch = 0.0f; 
   float    yaw   = 0.0f;
   float    roll  = PI;
   
   PerspectiveCamera cam
      {  position
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
   
   vector3f sphere1_position{0.0f, 1.0f, 0.0f};
   Sphere sphere1
      {  sphere1_position
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
   //sphere2.set_reflection(0.3);
   scene.add_shape(&sphere1);
   scene.add_shape(&sphere2);
   scene.add_shape(&sphere3);

   light lig;
   //lig.add_light(vector3f{+1000.0f, 1000.0f, 0.0f}, 4 * color_rgb{0.5f, 0.3f, 0.8f});
   //lig.add_light(vector3f{-2.0f, 2.0f, 0.0f}, 4 * color_rgb{0.5f, 0.3f, 0.8f});
   //lig.add_light(vector3f{0.0f, 1000.0f,  1500.0f}, 4 * color_rgb{0.5f, 0.3f, 0.8f});
   //lig.add_light(vector3f{-3.0f, 3.0f, +5.0f}, 4 * color_rgb{0.5f, 0.6f, 0.2f});
   //lig.add_light(vector3f{-2.0f, 4.0f, -5.0f}, 3 * color_rgb{0.2f, 0.3f, 0.7f});
   lig.add_light(vector3f{-2.0f, 4.0f, 1.0f},  5 * color_rgb{1.0f, 1.0f, 1.0f});
   //lig.add_light(vector3f{-2.0f, 4.0f, -5.0f}, 3 * color_rgb{0.2f, 0.3f, 0.7f});

   //render_image(img, &cam, &scene, &lig);
   //render_image_ssaa(img, &cam, &scene, &lig);
   //img.save_image("img.ppm", 1.0, 2.2);
   //
   Screen scr{width, height};
   
   char  ch;
   float scale       = 0.05f;
   float scale_angle = 1.0f;
   
   constexpr int keyboard_buffer_size = 9;
   char keyboard_buffer[keyboard_buffer_size];

   auto fill_keyboard_buffer = [](char* buffer, int size)
   {
      char ch;
      int count = 0;
      for(int i = 0; i < size - 1; ++i)
      {
         if((ch = getch()) != ERR)
         {
            buffer[i] = ch;
            ++count;
         }
      }
      buffer[count] = '\0';

      return count != 0;
   };

   while(true)
   {
      if((ch = getch()) != ERR)
      {
         if(ch == 'w')
            position += scale * cam.get_forward();
         else if(ch == 's')
            position -= scale * cam.get_forward();
         else if(ch == 'd')
            position += scale * cam.get_right();
         else if(ch == 'a')
            position -= scale * cam.get_right();
         else if (ch == 'l')	
            pitch += scale_angle * 1.0f / 120.0f / 5.0f * (2.0f * PI);
         else if (ch == 'j')	
            pitch -= scale_angle * 1.0f / 120.0f / 5.0f * (2.0f * PI);
         else if (ch == 'i')	
            yaw += scale_angle * 1.0f / 120.0f / 5.0f * (2.0f * PI);
         else if (ch == 'k')	
            yaw -= scale_angle * 1.0f / 120.0f / 5.0f * (2.0f * PI);
         else if(ch == 'b')
            sphere1_position += scale * vector3f{0.1f, 0.0f, 0.0f};
         else if(ch == 'n')
            sphere1_position -= scale * vector3f{0.1f, 0.0f, 0.0f};

         sphere1.set_center(sphere1_position);
         
         cam.set_position (position);
         cam.set_direction(yaw, pitch, roll); 
      }
      
      render_screen(scr, &cam, &scene, &lig);
      scr.Draw();
      frame.show_frame_rate(scr, 0, 0);
      scr.refresh();
      frame.waitForNextFrame();
   }

   return 0;
}
