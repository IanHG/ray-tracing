#pragma once
#ifndef RAY_TRACER_SCREEN_RGB_HPP_INCLUDED
#define RAY_TRACER_SCREEN_RGB_HPP_INCLUDED
/**
 * Initialliy stolen from: 
 **/

#include <iostream>
#include <stdio.h>
//#include <string>
#include <cstring>
#include <termios.h>

//typedef char byte;

// Control Sequence Introducer (CSI) 
#define CSI "\x1B["
// Clear screen
#define CLS CSI "2J"
// Hide/Show cursor
#define HCS CSI "?25l"
#define SCS CSI "?25h"
// Reset char
#define RCH CSI "0m"

#define MOV(x, y) CSI + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H"

void my_clear()
{
   printf(CLS);
}

// Move cursor to position
int my_move(char* buffer, int x, int y)
//int my_move(int x, int y)
{
   // x + 1 and y + 1 ??
   return sprintf(buffer, CSI "%d;%dH", y, x);
   //printf(CSI "%d;%dH", y + 1, x + 1);
}

// Colorize
//int colorize(char* buffer, char ch, int r, int g, int b)
int colorize(char* buffer, unsigned char r, unsigned char g, unsigned char b)
//void colorize(char ch, unsigned char r, unsigned char g, unsigned char b)
{
   // Set foreground/background coloring, and RGB value
   //    foreground: 38
   //    background: 48
   //printf(CSI "%d;2;%d;%d;%dm%c", /* foreground */ 38, r, g, b, ch);
   return sprintf(buffer, CSI "%d;2;%d;%d;%dm", /* foreground */ 48, r, g, b);
}

int my_addch(char* buffer, char ch)
{
   return sprintf(buffer, "%c", ch);
}

struct termios ptty, ctty;
      
bool restore_tty() {
  if (tcsetattr(STDIN_FILENO, TCSANOW, &ptty) != 0) {
    std::cout << "can not restore default tty attr" << std::endl;

    return false;
  }

  return true;
}

bool init_tty() {
  memset(&ctty, 0, sizeof(ctty));

  if (tcgetattr(STDIN_FILENO, &ctty) != 0) {
    std::cout << "can not get tty attr" << std::endl;

    return false;
  }

  memcpy(&ptty, &ctty, sizeof(ptty));

  ctty.c_lflag &= ~(ICANON | ECHO);

  if (tcsetattr(STDIN_FILENO, TCSANOW, &ctty) != 0) {
    std::cout << "can not set tty attr" << std::endl;

    return false;
  }

  return true;
}

struct screen_rgb
{
   struct rgb
   {
      unsigned char r;
      unsigned char g;
      unsigned char b;
   };

   public:
      screen_rgb(int width, int height)
         :  width (width)
         ,  height(height)
         ,  canvas{new color_rgb[width * height]}
         ,  frame_buffer(new char[25 * width * height])
         ,  frame_buffer_head(frame_buffer.get())
      {
         init();
         clear();
      }

      void clear() 
      {
         for (int i = 0; i < height; i++) 
         {
            for (int j = 0; j < width; j++) 
            {
               canvas[i* width + j] = { .r = 0, .g = 0, .b = 0 };
            }
         }
      }

      void draw();
      void refresh();

      ~screen_rgb()
      {
         restore_tty();
         std::cout << SCS;
      }

   public:
      int   palette = 2;
      int   width   = 0;
      int   height  = 0;
      
      std::unique_ptr<color_rgb[]> canvas = nullptr;

      std::unique_ptr<char[]> frame_buffer      = nullptr;
      char*                   frame_buffer_head = nullptr;
   private:

      void init();
      void fill_screen();

      char brightness(int lum) const 
      {
         static const struct 
         {
            int n;
            const char s[27];
         } p[] 
            =  {  {7, " .:+%#@"}
               ,  {18, " .,^:-+abcdwf$&%#@"}
               ,  {26, " .,:;'\"<>i!()[](){}*8B%$#@"}
               ,  {3, " .:"}
               ,  {10, " .:-=+*#%@"}
               };

         if (0 <= palette && palette <= 4) 
         {
            const auto &pal = p[palette];
            return pal.s[lum * (pal.n) / 256 / dW / dH];
         } 
         else 
         {
            return ' ';
         }
      }
};

void screen_rgb::draw() 
{
   //allocate
   int Y = height / dH, X = width / dW;

   int inv = 1 / (dH * dW);

   int r_prev = 255;
   int g_prev = 255;
   int b_prev = 255;

   this->frame_buffer_head = frame_buffer.get();

   for (int y = 0; y < Y; ++y) 
   {
      frame_buffer_head += my_move(frame_buffer_head, 0, y);

      for (int x = 0; x < X; ++x) 
      {
         color_rgb count{ .r = 0.0f, .g = 0.0f, .b = 0.0f};

         // calculating brightness
         for (int h = 0; h < dH; ++h) 
         {
            for (int w = 0; w < dW; ++w) 
            {
               count += canvas[(dH * y + h)* width +(dW * x + w)];
            }
         }

         //std::cout << count  << std::endl;
         
         count.r /= (dH * dW);
         count.g /= (dH * dW);
         count.b /= (dH * dW);
         
         count.apply_gamma_correction(1.0, 2.2);
         
         //count *= inv;
            //{  .r = static_cast<unsigned char>(clamp(result.r, 0.0f, 1.0f) * 255.0f)
            //,  .g = static_cast<unsigned char>(clamp(result.g, 0.0f, 1.0f) * 255.0f)
            //,  .b = static_cast<unsigned char>(clamp(result.b, 0.0f, 1.0f) * 255.0f)
            //};
         //std::cout << "Luminance  : " << luminance(count) << std::endl;
         //std::cout << "Luminance  : " << static_cast<int>(clamp(luminance(count), 0.0f, 1.0f) * 255.0f) << std::endl;
         //std::cout << "Brightness : " << brightness(static_cast<int>(clamp(luminance(count), 0.0f, 1.0f) * 255.0f)) << std::endl;;

         auto br = brightness(static_cast<int>(clamp(luminance(count), 0.0f, 1.0f) * 255.0f));
         auto r  = static_cast<unsigned char>(clamp(count.r, 0.0f, 1.0f) * 255.0f);
         auto g  = static_cast<unsigned char>(clamp(count.g, 0.0f, 1.0f) * 255.0f);
         auto b  = static_cast<unsigned char>(clamp(count.b, 0.0f, 1.0f) * 255.0f);

         if( r != r_prev || g != g_prev || b != b_prev )
            frame_buffer_head += colorize(frame_buffer_head, r, g, b);

         frame_buffer_head += my_addch(frame_buffer_head, ' ');

         r_prev = r;
         g_prev = g;
         b_prev = b;
      }
   }
   (*frame_buffer_head) = '\0';
}

void screen_rgb::refresh()
{
   printf("%s", frame_buffer.get());
   fflush(stdout);
}

void screen_rgb::init() 
{
   init_tty();
   if(setvbuf(stdout, NULL, _IOFBF, 0) != 0)
   {
      std::cout << "could not set buffer " << std::endl;
      std::exit(1);
   }
   my_clear();
   //std::cout << MOV(0,0);
   printf(HCS);
   //std::cout << HCS << MOV(0, 0);
}

#endif /* RAY_TRACER_SCREEN_HPP_INCLUDED */
