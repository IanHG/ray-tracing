#pragma once
#ifndef RAY_TRACER_FRAME_INCLUDE_HPP
#define RAY_TRACER_FRAME_INCLUDE_HPP

#include <chrono>
#include <thread>
#include <sstream>
#include <cstdlib>

#include "screen.hpp"

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
      last_frame = next_frame;
      next_frame += frame;
      std::this_thread::sleep_until(next_frame);
   }

   void show_frame_rate(Screen& scr, int y, int x) const
   {      
      int fps = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_frame).count();
      char buffer[8];
      sprintf(buffer, "%d", fps);
      mvprintw(y, x, "%s", buffer);
   }
} frame;

#endif /* RAY_TRACER_FRAME_INCLUDE_HPP */
