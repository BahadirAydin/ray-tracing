#ifndef RAY_H
#define RAY_H

#include "parser.h"

#include "utils.h"

#include <iostream>

class Ray {
public:
  Ray(parser::Vec3f origin, parser::Vec3f direction) {
    this->origin = origin;
    this->direction = direction;
  }
  void set_origin(const parser::Vec3f &origin) { this->origin = origin; }
  void set_direction(const parser::Vec3f &direction) {
    this->direction = direction;
  }
  parser::Vec3f get_origin() const { return origin; }
  parser::Vec3f get_direction() const { return direction; }
  parser::Vec3f get_point(float t) const {
    return add_vectors(origin, multiply_vector(direction, t));
  }

  int get_depth() { return depth; }

  int inc_depth() {
    this->depth += 1;
    return this->depth;
  }

  // this was depth+= d before i changed it but it might be wrong
  void set_depth(int d) { this->depth = d; }

private:
  parser::Vec3f origin;
  parser::Vec3f direction;
  int depth = 0;
};

inline Ray generate_ray(const parser::Camera &camera, int i, int j,
                        float pixel_width, float pixel_height) {

  const parser::Vec3f e = camera.position;
  const parser::Vec3f v = camera.up;
  const parser::Vec3f u = camera.u;
  const parser::Vec3f q = camera.q;
  const parser::Vec3f m = camera.plane_center;

  const float su = (i + 0.5f) * pixel_width;
  const float sv = (j + 0.5f) * pixel_height;

  parser::Vec3f s = add_vectors(
      q, add_vectors(multiply_vector(u, su), multiply_vector(v, -1 * sv)));

  parser::Vec3f direction = subtract_vectors(s, e);

  return Ray(e, direction);
}
#endif // RAY_H
