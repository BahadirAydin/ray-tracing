#ifndef RAY_H
#define RAY_H

#include "parser.h"

#include "utils.h"

#include <iostream>

class Ray {
public:
  void set_origin(const parser::Vec3f &origin) { this->origin = origin; }
  void set_direction(const parser::Vec3f &direction) {
    this->direction = direction;
  }
  parser::Vec3f get_origin() const { return origin; }
  parser::Vec3f get_direction() const { return direction; }
  parser::Vec3f get_point(float t) const {
    return add_vectors(origin, multiply_vector(direction, t));
  }

  void print() {
    std::cout << "origin: " << origin.x << " " << origin.y << " " << origin.z
              << " "
              << "direction: " << direction.x << " " << direction.y << " "
              << direction.z << std::endl;
  }

  void init_depth() { this->depth = 0; }

  int get_depth() { return depth; }

  int inc_depth() {
    this->depth += 1;
    return this->depth;
  }

  int set_depth(int d) {
    this->depth += d;
    return this->depth;
  }

private:
  parser::Vec3f origin;
  parser::Vec3f direction;
  int depth = 0;
};

inline Ray generate_ray(const parser::Camera &camera, int i, int j,
                        float pixel_width, float pixel_height) {

  parser::Vec3f e = camera.position;
  parser::Vec3f gaze = camera.gaze;
  parser::Vec3f v = camera.up;
  parser::Vec3f w = camera.w;
  parser::Vec3f u = camera.u;
  parser::Vec3f q = camera.q;
  parser::Vec4f near_plane = camera.near_plane;
  parser::Vec3f m = camera.plane_center;
  float distance = camera.near_distance;

  float su = (i + 0.5) * pixel_width;
  float sv = (j + 0.5) * pixel_height;

  parser::Vec3f s = add_vectors(
      q, add_vectors(multiply_vector(u, su), multiply_vector(v, -1 * sv)));

  Ray ray;
  ray.set_origin(e);
  parser::Vec3f direction = subtract_vectors(s, e);

  ray.set_direction(direction);

  return ray;
}
#endif // RAY_H
