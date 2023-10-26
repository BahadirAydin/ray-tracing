#ifndef RAY_H
#define RAY_H

#include "parser.h"
#include <iostream>
#include "utils.h"

class Ray {
public:
    void set_origin(const parser::Vec3f &origin) { this->origin = origin; }

    void set_direction(const parser::Vec3f &direction) {
        this->direction = direction;
    }

    void print() {
        std::cout << "origin: " << origin.x << " " << origin.y << " " << origin.z 
                  << " " << "direction: " <<direction.x << " " << direction.y << " " 
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
  parser::Vec3f direction = add_vectors(s, multiply_vector(e, -1));
  
  ray.set_direction(direction);

  return ray;
}

#endif // RAY_H
