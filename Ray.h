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
//TODO
float intersect_mesh() { return 0; }
#endif // RAY_H
