#pragma once
#ifndef RAY_TRACER_UTIL_HPP_INCLUDED
#define RAY_TRACER_UTIL_HPP_INCLUDED

float clamp(float value, float min, float max)
{
   if (value < min) return min;
   if (value > max) return max;
   return value;
}

#endif /* RAY_TRACER_UTIL_HPP_INCLUDED */
