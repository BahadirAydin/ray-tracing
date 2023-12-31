#ifndef INTERSECT_H
#define INTERSECT_H

#include "Ray.h"
#include "parser.h"
#include "utils.h"
#include <complex>
#include <limits>
#include <vector>

struct Intersection {
  parser::Vec3f point;
  parser::Vec3f normal;
  const parser::Material *material;
  float t;
  bool is_null = true;
};

inline float intersect_sphere(const parser::Vec3f &vertex, float radius,
                              const Ray &r) {
  const parser::Vec3f origin = r.get_origin();
  const parser::Vec3f direction = r.get_direction();

  const float a = dot_product(direction, direction);
  const float b = 2 * dot_product(direction, subtract_vectors(origin, vertex));
  const float c = dot_product(subtract_vectors(origin, vertex),
                              subtract_vectors(origin, vertex)) -
                  std::pow(radius, 2);
  const float delta = std::pow(b, 2) - 4 * a * c;
  if (delta < 0.0f) {
    return -1;
  }
  const float eps = 1e-6;
  float t1 = (-b + std::sqrt(delta)) / (2 * a);
  float t2 = (-b - std::sqrt(delta)) / (2 * a);

  if (t1 < eps && t2 < eps) {
    return -1;
  }

  if (t1 < eps) {
    return t2;
  }

  if (t2 < eps) {
    return t1;
  }

  return std::min(t1, t2);
}

inline float intersect_triangle(const parser::Vec3f &vertex1,
                                const parser::Vec3f &vertex2,
                                const parser::Vec3f &vertex3,
                                const parser::Vec3f edge1,
                                const parser::Vec3f edge2, const Ray &r) {
  const parser::Vec3f h = cross_product(r.get_direction(), edge2);
  float a = dot_product(edge1, h);

  if (std::abs(a) < std::numeric_limits<float>::epsilon()) {
    return -1;
  }

  const float f = 1.0f / a;
  parser::Vec3f s = subtract_vectors(r.get_origin(), vertex1);
  const float u = f * dot_product(s, h);

  if (u < 0.0f || u > 1.0f)
    return -1;

  parser::Vec3f q = cross_product(s, edge1);
  const float v = f * dot_product(r.get_direction(), q);

  if (v < 0.0f || u + v > 1.0f)
    return -1;

  const float t = f * dot_product(edge2, q);
  if (t > std::numeric_limits<float>::epsilon())
    return t;

  return -1;
}

inline Intersection intersect_objects(const Ray &r, const parser::Scene &s) {

  float t = -1;
  Intersection min_intersection;
  min_intersection.t = std::numeric_limits<float>::infinity();
  for (const parser::Sphere &sphere : s.spheres) {
    parser::Vec3f center = s.vertex_data[sphere.center_vertex_id - 1];
    t = intersect_sphere(center, sphere.radius, r);
    if (t > 0.0f && t < min_intersection.t) {
      Intersection intersection;
      intersection.point = r.get_point(t);
      parser::Vec3f normal = subtract_vectors(intersection.point, center);
      intersection.normal = normalize(normal);
      intersection.material = &s.materials[sphere.material_id - 1];
      intersection.t = t;
      min_intersection = intersection;
      min_intersection.is_null = false;
    }
  }

  for (const parser::Triangle &triangle : s.triangles) {
    const parser::Vec3f vertex1 = s.vertex_data[triangle.indices.v0_id - 1];
    const parser::Vec3f vertex2 = s.vertex_data[triangle.indices.v1_id - 1];
    const parser::Vec3f vertex3 = s.vertex_data[triangle.indices.v2_id - 1];

    float t = intersect_triangle(vertex1, vertex2, vertex3, triangle.edge1, triangle.edge2, r);

    if (t > 0.0f && t < min_intersection.t) {
      Intersection intersection;
      intersection.point = r.get_point(t);

      intersection.normal = triangle.normal;
      intersection.material = &s.materials[triangle.material_id - 1];
      intersection.t = t;
      min_intersection = intersection;
      min_intersection.is_null = false;
    }
  }

  for (const parser::Mesh &mesh : s.meshes) {
    for (const parser::Face &face : mesh.faces) {
      const parser::Vec3f vertex1 = s.vertex_data[face.v0_id - 1];
      const parser::Vec3f vertex2 = s.vertex_data[face.v1_id - 1];
      const parser::Vec3f vertex3 = s.vertex_data[face.v2_id - 1];

      float t = intersect_triangle(vertex1, vertex2, vertex3, face.edge1,
                                   face.edge2, r);

      if (t > 0.0f && t < min_intersection.t) {
        Intersection intersection;
        intersection.point = r.get_point(t);
        intersection.normal = face.normal;
        intersection.material = &s.materials[mesh.material_id - 1];
        intersection.t = t;
        min_intersection = intersection;
        min_intersection.is_null = false;
      }
    }
  }
  return min_intersection;
}
#endif
