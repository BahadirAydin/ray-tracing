#ifndef UTILS_H
#define UTILS_H
#include "parser.h"
#include <cmath>

inline parser::Vec3f cross_product(const parser::Vec3f &a,
                                   const parser::Vec3f &b) {
  parser::Vec3f result;
  result.x = a.y * b.z - a.z * b.y;
  result.y = a.z * b.x - a.x * b.z;
  result.z = a.x * b.y - a.y * b.x;
  return result;
}

inline parser::Vec3f add_vectors(const parser::Vec3f &a,
                                 const parser::Vec3f &b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline parser::Vec3f subtract_vectors(const parser::Vec3f &a,
                                      const parser::Vec3f &b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline parser::Vec3f multiply_vector(const parser::Vec3f &a, float b) {
  return {a.x * b, a.y * b, a.z * b};
}

inline float dot_product(const parser::Vec3f &a, const parser::Vec3f &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float get_magn(const parser::Vec3f &vec) {
  return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline parser::Vec3f normalize(parser::Vec3f &vec) {
  float magnitude = get_magn(vec);
  vec.x /= magnitude;
  vec.y /= magnitude;
  vec.z /= magnitude;
  return vec;
}

inline float calc_det(float x00, float x01, float x02, float x10, float x11,
                      float x12, float x20, float x21, float x22) {
  return (x00 * ((x11 * x22) - (x21 * x12))) -
         (x10 * ((x22 * x01) - (x02 * x21))) +
         (x20 * ((x01 * x12) - (x02 * x11)));
}

inline parser::Vec3f clamp(parser::Vec3f color) {
  parser::Vec3f c;
  c.x = std::max(0.0f, std::min(255.0f, color.x));
  c.y = std::max(0.0f, std::min(255.0f, color.y));
  c.z = std::max(0.0f, std::min(255.0f, color.z));

  return c;
}

#endif // UTILS_H
