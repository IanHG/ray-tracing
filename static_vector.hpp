#pragma once
#ifndef RAY_TRACER_STATIC_VECTOR_HPP_INCLUDED
#define RAY_TRACER_STATIC_VECTOR_HPP_INCLUDED

#include <utility>
#include <cmath>

/**
 * Static vector class.
 **/
template<class T, int N>
class static_vector
{
   public:
      constexpr static int order = N;

      static_vector()
      {
         for(int i = 0; i < N; ++i)
         {
            elements[i] = T{0.0};
         }
      }
   
      template<class... Ts, std::enable_if_t<N == sizeof...(Ts), int> = 0>
      static_vector(Ts&&... ts)
         :  elements{std::forward<Ts>(ts)...}
      {
         static_assert(sizeof...(Ts) == N, "Number of arguments incorrect.");
      }

      static_vector(const static_vector& other)
      {
         for(int i = 0; i < N; ++i)
         {
            this->elements[i] = other.elements[i];
         }
      }

      static_vector& operator+=(const static_vector& other)
      {
         for(int i = 0; i < N; ++i)
         {
            elements[i] += other.elements[i];
         }
         return *this;
      }
      
      static_vector& operator-=(const static_vector& other)
      {
         for(int i = 0; i < N; ++i)
         {
            elements[i] -= other.elements[i];
         }
         return *this;
      }

      static_vector& operator*=(const T& factor)
      {
         for(int i = 0; i < N; ++i)
         {
            elements[i] *= factor;
         }
         return *this;
      }
      
      static_vector& operator/=(const T& factor)
      {
         for(int i = 0; i < N; ++i)
         {
            elements[i] /= factor;
         }
         return *this;
      }

      static_vector operator+(const static_vector& other) const
      {
         static_vector vec{*this};
         vec += other;
         return vec;
      }
      
      static_vector operator-(const static_vector& other) const
      {
         static_vector vec{*this};
         vec -= other;
         return vec;
      }
      

      static_vector operator-() const
      {
         static_vector vec{*this};
         for(int i = 0; i < N; ++i)
         {
            vec.elements[i] = -vec.elements[i];
         }
         return vec;
      }

      T magnitude2() const
      {
         auto mag = T{0.0};
         for(int i = 0; i < N; ++i)
         {
            mag += elements[i] * elements[i];
         }
         return mag;
      }

      T magnitude() const
      {
         return std::sqrt(this->magnitude2());
      }

      void normalize()
      {
         auto mag = this->magnitude();
         // fast inv sqrt??
         *this /= mag;
      }

      static_vector normalized() const
      {
         auto mag = this->magnitude();
         return *this / mag;
      }

      const T& operator[](const int i) const
      {
         return elements[i];
      }
      
      T& operator[](const int i)
      {
         return elements[i];
      }

      const T& at(const int i) const
      {
         return elements[i];
      }
      
      T& at(const int i) 
      {
         return elements[i];
      }

   private:
      //T elements[N] alignas(32);
      T elements[N];
};

template<class T, int N>
static_vector<T, N> operator*(const static_vector<T, N>& vec, const T& factor)
{
   static_vector<T, N> scaled{vec};
   scaled *= factor;
   return scaled;
}

template<class T, int N>
static_vector<T, N> operator*(const T& factor, const static_vector<T, N>& vec)
{
   return vec * factor;
}

template<class T, int N>
static_vector<T, N> operator/(const static_vector<T, N>& vec, const T& factor)
{
   static_vector<T, N> div{vec};
   div /= factor;
   return div;
}

/**
 * 
 **/
template<class T, int N>
T dot(const static_vector<T, N>& vec1, const static_vector<T, N> vec2)
{
   auto dot_prod = T{0.0};
   for(int i = 0; i < N; ++i)
   {
      dot_prod += vec1[i] * vec2[i];
   }
   return dot_prod;
}

/**
 *
 **/
template<class T>
static_vector<T, 3> cross(const static_vector<T, 3>& vec1, const static_vector<T, 3>& vec2)
{  
   return 
      {  vec1[1] * vec2[2] - vec1[2] * vec2[1]
      ,  vec1[2] * vec2[0] - vec1[0] * vec2[2]
      ,  vec1[0] * vec2[1] - vec1[1] * vec2[0]
      };
}

/**
 *
 **/
template<class T, int N>
static_vector<T, N> hadamard(const static_vector<T, N>& vec1, const static_vector<T, N> vec2)
{
   static_vector<T, N> hada{vec1};
   for(int i = 0; i < N; ++i)
   {
      hada[i] *= vec2[i];
   }
   return hada;
}

/**
 *
 **/
template<class T, int N>
static_vector<T, N> hadamard_div(const static_vector<T, N>& vec1, const static_vector<T, N> vec2)
{
   static_vector<T, N> hada{vec1};
   for(int i = 0; i < N; ++i)
   {
      hada[i] /= vec2[i];
   }
   return hada;
}

/**
 *
 **/
using vector2f = static_vector<float, 2>;
using vector3f = static_vector<float, 3>;
using vector4f = static_vector<float, 4>;

#endif /* RAY_TRACER_STATIC_VECTOR_HPP_INCLUDED */
