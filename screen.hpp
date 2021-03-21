#pragma once
#ifndef RAY_TRACER_SCREEN_HPP_INCLUDED
#define RAY_TRACER_SCREEN_HPP_INCLUDED
/**
 * Initialliy stolen from: 
 **/

#include <iostream>
#include <stdio.h>
#include <string>
#include <ncurses.h>

typedef unsigned char byte;

constexpr int dW = 1, dH = 2;

class Screen 
{
   public:
      Screen(int width, int height)
         :  width (width)
         ,  height(height)
         ,  canvas{new byte[width * height]}
      {
         Setup();
         clear();
      }

      void clear() 
      {
         for (int i = 0; i < height; i++) 
         {
            for (int j = 0; j < width; j++) 
            {
               canvas[i* width + j] = 0;
            }
         }
      }

      void draw_point(int x, int y, int luminance)
      {
         if (x < 0 || y < 0 || x >= width || y >= height) 
         {
            return;
         }

         canvas[x + y * width] = luminance;
      }

      void Draw() 
      {
        //allocate
        int Y = height / dH, X = width / dW + 1;
     
        char* frame=new char[Y*X];

        for (int i = 0; i < Y - 1; ++i) {
            frame[X * i + X - 1] = '\n';
        }
        frame[X * (Y - 1) + X - 1] = '\0';

        for (int i = 0; i < Y; i++) {
            for (int j = 0; j < X-1; j++) {
                int count = 0;

                // calculating brightness
                for (int k = 0; k < dH; k++) {
                    for (int l = 0; l < dW; l++) {
                        count += canvas[(dH * i + k)* width +(dW * j + l)];
                    }
                }
                frame[X*i+j] = brightness(count);
            }
        }
        // borders
        for (int i = 0; i < Y; ++i) {
            frame[X * i] = '@';
            frame[X * i + X - 2] = '@';
        }
        for (int j = 0; j < X-1; ++j) {
            frame[j] = '@';
            frame[X*(Y-1)+j] = '@';
        }
        FillScreenWithString(frame);
        delete[] frame;
      }

      int get_height() const 
      {
         return height;
      }

      int get_width() const 
      {
         return width;
      }

      void set_palette(int pal) 
      {
         this->palette = pal;
      }

      void refresh()
      {
         ::refresh();
      }

      ~Screen()
      {
         endwin();
      }

   private:
      int   palette = 2;
      int   width   = 0;
      int   height  = 0;
      
      std::unique_ptr<byte[]> canvas = nullptr;

      void Setup();
      void FillScreenWithString(const char *frame);

      char brightness(int count) const 
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
            return pal.s[count * (pal.n) / 256 / dW / dH];
         } 
         else 
         {
            return ' ';
         }
      }
};


void Screen::FillScreenWithString(const char *frame) {
   mvprintw(0, 0, "%s", frame);
   
   //std::cout << " PRINTING FRAME " << std::endl;
   //std::cout << frame << std::endl;
}

void Screen::Setup() 
{
   //this function is a mess, just ignore it
   initscr();
   keypad(stdscr, TRUE);
   cbreak();
   noecho();
   curs_set(0);
   nodelay(stdscr, true);

   //std::cout << LINES << " " << COLS << std::endl;

   //CONSOLE_FONT_INFOEX cf = {0};
   //cf.cbSize = sizeof cf;
   //cf.dwFontSize.X = dW;
   //cf.dwFontSize.Y = dH;
   //wcscpy_s(cf.FaceName, L"Terminal");
   //SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), 0, &cf);

   //HWND console = GetConsoleWindow();
   //RECT ConsoleRect;
   //GetWindowRect(console, &ConsoleRect);
   //HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
   //CONSOLE_SCREEN_BUFFER_INFO csbi;
   //GetConsoleScreenBufferInfo(hstdout, &csbi);

   //csbi.dwSize.X = csbi.dwMaximumWindowSize.X;
   //csbi.dwSize.Y = csbi.dwMaximumWindowSize.Y;
   //SetConsoleScreenBufferSize(hstdout, csbi.dwSize);
   //MoveWindow(console, 0, 0, width, height, TRUE);
   //SetConsoleDisplayMode(hstdout, CONSOLE_FULLSCREEN_MODE, 0);
   //ShowScrollBar(console, SB_BOTH, FALSE);
   
   clear();
   refresh();
}

#endif /* RAY_TRACER_SCREEN_HPP_INCLUDED */
