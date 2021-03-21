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
inline int my_move(char* buffer, int x, int y)
//int my_move(int x, int y)
{
   // x + 1 and y + 1 ??
   return sprintf(buffer, CSI "%d;%dH", y + 1, x + 1);
   //printf(CSI "%d;%dH", y + 1, x + 1);
}

// Colorize
//int colorize(char* buffer, char ch, int r, int g, int b)
inline int colorize(char* buffer, unsigned char r, unsigned char g, unsigned char b)
//void colorize(char ch, unsigned char r, unsigned char g, unsigned char b)
{
   // Set foreground/background coloring, and RGB value
   //    foreground: 38
   //    background: 48
   //printf(CSI "%d;2;%d;%d;%dm%c", /* foreground */ 38, r, g, b, ch);
   return sprintf(buffer, CSI "%d;2;%d;%d;%dm", /* foreground */ 48, r, g, b);
}

inline int my_addch(char* buffer, char ch)
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
   struct lum_rgb
   {
      unsigned char lum;
      unsigned char r;
      unsigned char g;
      unsigned char b;
   };

   public:
      screen_rgb(int width, int height)
         :  width (width)
         ,  height(height)
         ,  canvas     {new lum_rgb[width * height]}
         ,  canvas_last{new lum_rgb[width * height]}
         ,  frame_buffer(new char[25 * width * height])
         ,  frame_buffer_head(frame_buffer.get())
      {
         clear();
         init();
      }

      void clear() 
      {
         for (int i = 0; i < height; i++) 
         {
            for (int j = 0; j < width; j++) 
            {
               canvas     [i* width + j] = { .lum = 0, .r = 0, .g = 0, .b = 0 };
               canvas_last[i* width + j] = { .lum = 0, .r = 0, .g = 0, .b = 0 };
            }
         }
      }

      void draw();
      void refresh();

      ~screen_rgb()
      {
         printf(CLS);
         printf(SCS);
         restore_tty();
      }

   public:
      int   palette = 2;
      int   width   = 0;
      int   height  = 0;
      
      std::unique_ptr<lum_rgb[]> canvas      = nullptr;
      std::unique_ptr<lum_rgb[]> canvas_last = nullptr;

      std::unique_ptr<char[]> frame_buffer      = nullptr;
      char*                   frame_buffer_head = nullptr;
      lum_rgb                 prev{ .lum = static_cast<unsigned char>(0), .r = static_cast<unsigned char>(0), .g = static_cast<unsigned char>(0), .b = static_cast<unsigned char>(0)};
      int                     x_prev = width  + 1;
      int                     y_prev = height + 1;

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

inline bool operator!=(const screen_rgb::lum_rgb& lhs, const screen_rgb::lum_rgb& rhs)
{
   return      (lhs.lum != rhs.lum) 
            || (lhs.r   != rhs.r)
            || (lhs.g   != rhs.g)
            || (lhs.b   != rhs.b)
            ;
}

std::ostream& operator<<(std::ostream& os, const screen_rgb::lum_rgb& lrgb)
{
   os << int(lrgb.lum) << " "
      << int(lrgb.r  ) << " "
      << int(lrgb.g  ) << " "
      << int(lrgb.b  ) ;
   return os;
}

void screen_rgb::draw() 
{
   //allocate
   int Y = height / dH, X = width / dW;

   int inv = 1 / (dH * dW);
   
   this->frame_buffer_head = frame_buffer.get();

   int count = 0;
   for (int y = 0; y < height; ++y) 
   {
      //frame_buffer_head += my_move(frame_buffer_head, 0, y);

      for (int x = 0; x < width; ++x) 
      {
         //std::cout << " x           : " << x << std::endl;
         //std::cout << " y           : " << y << std::endl;
         //std::cout << " x_prev      : " << x_prev << std::endl;
         //std::cout << " y_prev      : " << y_prev << std::endl;
         //std::cout << " count       : " << count << std::endl;
         //std::cout << " canvas      : " << canvas     [count] << std::endl;
         //std::cout << " canvas_last : " << canvas_last[count] << std::endl;

         if(canvas[count] != canvas_last[count])
         {
            //std::cout << " NOT EQUAL " << std::endl;
            lum_rgb curr = canvas[count];

            if( (y != y_prev) || (x != (x_prev + 1)) )
            {
               //std::cout << " MOVING " << std::endl;
               frame_buffer_head += my_move(frame_buffer_head, x, y);
            }

            if( curr.r != prev.r || curr.g != prev.g || curr.b != prev.b )
            {
               frame_buffer_head += colorize(frame_buffer_head, curr.r, curr.g, curr.b);
            }

            //frame_buffer_head += my_addch(frame_buffer_head, brightness(curr.lum));
            frame_buffer_head += my_addch(frame_buffer_head, ' ');
            
            prev   = curr;
            x_prev = x;
            y_prev = y;
         }
         
         //lum_rgb curr = canvas[count];
         ////std::cout << x << " " << y << " " << curr << std::endl;
         //if( curr.r != prev.r || curr.g != prev.g || curr.b != prev.b )
         //{
         //   frame_buffer_head += colorize(frame_buffer_head, curr.r, curr.g, curr.b);
         //}
         ////frame_buffer_head += colorize(frame_buffer_head, curr.r, curr.g, curr.b);
         ////frame_buffer_head += my_addch(frame_buffer_head, brightness(curr.lum));
         //frame_buffer_head += my_addch(frame_buffer_head, ' ');
         //prev   = curr;

         ++count;
      }
   }

   (*frame_buffer_head) = '\0';

   //memcpy(canvas_last.get(), canvas.get(), height * width * sizeof(lum_rgb));
   std::swap(canvas, canvas_last);
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
   printf(HCS);

   // Dump initial canvas to screen
   this->frame_buffer_head = frame_buffer.get();
   int count = 0;
   frame_buffer_head += colorize(frame_buffer_head, prev.r, prev.g, prev.b);
   for (int y = 0; y < height; ++y) 
   {
      frame_buffer_head += my_move(frame_buffer_head, 0, y);
      for (int x = 0; x < width; ++x) 
      {
         lum_rgb curr = canvas[count];
         if( curr.r != prev.r || curr.g != prev.g || curr.b != prev.b )
         {
            frame_buffer_head += colorize(frame_buffer_head, curr.r, curr.g, curr.b);
         }
         frame_buffer_head += my_addch(frame_buffer_head, ' ');
         prev   = curr;
         ++count;
      }
   }

   (*frame_buffer_head) = '\0';
   
   std::swap(canvas, canvas_last);
   
   x_prev = this->width  - 1;
   y_prev = this->height - 1;

   this->refresh();
}

#endif /* RAY_TRACER_SCREEN_HPP_INCLUDED */
