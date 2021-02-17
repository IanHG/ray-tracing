#pragma once
#ifndef RAY_TRACER_IMAGE_HPP_INCLUDED
#define RAY_TRACER_IMAGE_HPP_INCLUDED

#include <memory>
#include <stdio.h>
#include <stdlib.h>

void write_ppm
   (  const std::string& filename
   ,  float* data
   ,  int width
   ,  int height
   )
{
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
   
   int size = width * height;
   std::unique_ptr<unsigned char[]> pix{new unsigned char[size * 3]};
   
   int pix_count = 0;
   for(int i = 0; i < size; ++i)
   {
      if(data[i] == 1.0f)
      {
         pix[pix_count + 0] = 255;
         pix[pix_count + 1] = 255;
         pix[pix_count + 2] = 255;
      }
      else
      {
         pix[pix_count + 0] = 0;
         pix[pix_count + 1] = 0;
         pix[pix_count + 2] = 0;
      }
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
         ,  data{ new float[width * height] }
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

      float* get_pixel(int x, int y) 
      {  
         return data.get() + (x + y * width);
      }

      void save_image(const std::string& filename) const
      {
         write_ppm(filename, data.get(), width, height);
      }

   private:
      int width;
      int height;
      std::unique_ptr<float[]> data;
};

#endif /* RAY_TRACER_IMAGE_HPP_INCLUDED */
