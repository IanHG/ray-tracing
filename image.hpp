#pragma once
#ifndef RAY_TRACER_IMAGE_HPP_INCLUDED
#define RAY_TRACER_IMAGE_HPP_INCLUDED

#include <memory>
#include <stdio.h>
#include <stdlib.h>

#include "color.hpp"

void write_ppm
   (  const std::string& filename
   ,  color_rgb* data
   ,  float exposure
   ,  float gamma
   ,  int width
   ,  int height
   )
{
   // Open file
   FILE* image_file = fopen(filename.c_str(), "wb"); 
   if(!image_file)
   {
      exit(EXIT_FAILURE);
   }

   // P6 filetype
   fprintf(image_file, "P6\n");   
   // Dimensions
   fprintf(image_file, "%d %d\n", width, height);
   // Max pixel (saturation?)
   fprintf(image_file, "255\n");
   
   // 
   int size = width * height;
   std::unique_ptr<unsigned char[]> pix{new unsigned char[size * 3]};
   
   int pix_count = 0;
   for(int i = 0; i < size; ++i)
   {
      // Get color and apply gamma correction
      color_rgb current = data[i];
      current.apply_gamma_correction(exposure, gamma);
      current.clamp(0.0f, 1.0f);
      
      // Take out pixel
      pix[pix_count + 0] = static_cast<unsigned char>(current.r * 255.0f);
      pix[pix_count + 1] = static_cast<unsigned char>(current.g * 255.0f);
      pix[pix_count + 2] = static_cast<unsigned char>(current.b * 255.0f);
      
      // Increment counter
      pix_count += 3;
   }

   // Write the image
   fwrite(pix.get(), sizeof(unsigned char), size * 3, image_file);
   fclose(image_file);
}

class image
{
   public:
      image(int width, int height)
         :  width(width)
         ,  height(height)
         ,  data{ new color_rgb[width * height] }
      {
      }

      int get_width() const
      {
         return width;
      }

      int get_height() const
      {
         return height;
      }

      color_rgb* get_pixel(int x, int y) 
      {  
         return data.get() + (x + y * width);
      }

      void save_image
         (  const std::string& filename
         ,  const float exposure
         ,  const float gamma
         )  const
      {
         write_ppm(filename, data.get(), exposure, gamma, width, height);
      }

   private:
      int width;
      int height;
      std::unique_ptr<color_rgb[]> data;
};

#endif /* RAY_TRACER_IMAGE_HPP_INCLUDED */
