#include "parser.h"
#include <cmath>

parser::Vec3f cross_product(const parser::Vec3f &a, const parser::Vec3f &b) {
  parser::Vec3f result;
  result.x = a.y * b.z - a.z * b.y;
  result.y = a.z * b.x - a.x * b.z;
  result.z = a.x * b.y - a.y * b.x;
  return result;
}

parser::Vec3f add_vectors(const parser::Vec3f &a, const parser::Vec3f &b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}
parser::Vec3f multiply_vector(const parser::Vec3f &a, float b) {
  return {a.x * b, a.y * b, a.z * b};
}

float dot_product(const parser::Vec3f &a, const parser::Vec3f &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

void normalize(parser::Vec3f &vec) {
  float magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
  vec.x /= magnitude;
  vec.y /= magnitude;
  vec.z /= magnitude;
}
