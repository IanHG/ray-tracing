#pragma once
#ifndef RAY_TRACER_FRAME_INCLUDE_HPP
#define RAY_TRACER_FRAME_INCLUDE_HPP

#include <chrono>
#include <thread>
#include <sstream>
#include <cstdlib>

#include "screen.hpp"
#include "screen_rgb.hpp"

/**
 * Struct for controlling when to draw next frame.
 **/
struct 
{
   decltype(std::chrono::milliseconds(int(1)/int(1))) frame      = std::chrono::milliseconds(1000 / 144);
   decltype(std::chrono::system_clock::now())         next_frame = std::chrono::system_clock::now();
   decltype(next_frame - frame)                       last_frame = next_frame - frame;
   
   void waitForNextFrame()
   {
      // Wait for next frame
      last_frame  = next_frame;
      next_frame += frame;
      std::this_thread::sleep_until(next_frame);
   }

   void show_frame_rate(screen_rgb& scr, int y, int x) const
   {      
      //int fps = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_frame).count();
      float fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_frame).count();
      //my_move(0,0);
      //printf(CSI "38;2;255;255;255m%i", fps);
      //char buffer[8];
      scr.frame_buffer_head += my_move(scr.frame_buffer_head, 0, 0);
      sprintf(scr.frame_buffer_head, "%f", fps);
      //mvprintw(y, x, "%s", buffer);
   }
} frame;

#endif /* RAY_TRACER_FRAME_INCLUDE_HPP */
