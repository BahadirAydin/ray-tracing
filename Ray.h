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

  void print() {
    std::cout << "origin: " << origin.x << " " << origin.y << " " << origin.z
              << " "
              << "direction: " << direction.x << " " << direction.y << " "
              << direction.z << std::endl;
  }

private:
  parser::Vec3f origin;
  parser::Vec3f direction;
};

Ray generate_ray(const parser::Camera &camera, int i, int j, float pixel_width,
                 float pixel_height) {

  parser::Vec3f e = camera.position;
  parser::Vec3f gaze = camera.gaze;
  parser::Vec3f v = camera.up;
  parser::Vec4f near_plane = camera.near_plane;
  float distance = camera.near_distance;

  parser::Vec3f w = {-gaze.x, -gaze.y, -gaze.z};
  parser::Vec3f u = cross_product(v, w);

  float su = (i + 0.5) * pixel_width;
  float sv = (j + 0.5) * pixel_height;

  parser::Vec3f m = add_vectors(e, multiply_vector(gaze, distance));

  parser::Vec3f q =
      add_vectors(m, add_vectors(multiply_vector(u, near_plane.x),
                                 multiply_vector(v, near_plane.w)));

  parser::Vec3f s = add_vectors(
      q, add_vectors(multiply_vector(u, su), multiply_vector(v, -1 * sv)));

  Ray ray;
  ray.set_origin(e);
  parser::Vec3f direction = substract_vectors(s, e);

  ray.set_direction(direction);

  return ray;
}

float intersect_sphere(const parser::Vec3f &vertex, float radius,
                       const Ray &r) {
  parser::Vec3f origin = r.get_origin();
  parser::Vec3f direction = r.get_direction();

  float a = dot_product(direction, direction);
  float b = 2 * dot_product(direction, substract_vectors(origin, vertex));
  float c = dot_product(substract_vectors(origin, vertex),
                        substract_vectors(origin, vertex)) -
            radius * radius;

  float delta = b * b - 4 * a * c;

  // std::cout << "delta: " << delta << std::endl;
  if (delta < 0) {
    return -1;
  }

  float t1 = (-b + std::sqrt(delta)) / (2 * a);
  float t2 = (-b - std::sqrt(delta)) / (2 * a);

  if (t1 < 0 && t2 < 0) {
    return -1;
  }

  if (t1 < 0) {
    return t2;
  }

  if (t2 < 0) {
    return t1;
  }

  return std::min(t1, t2);
}

float intersect_triangle(const parser::Vec3f &vertex1,
                         const parser::Vec3f &vertex2,
                         const parser::Vec3f &vertex3, const Ray &r) {
  // TODO implement barycentric coordinates method
  return 0;
}
float intersect_mesh()
#endif // RAY_H
