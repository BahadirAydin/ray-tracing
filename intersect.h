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
  parser::Material material;
  float t;
  int is_null = 1;
};

inline float intersect_sphere(const parser::Vec3f &vertex, float radius,
                              const Ray &r) {
  parser::Vec3f origin = r.get_origin();
  parser::Vec3f direction = r.get_direction();

  float a = dot_product(direction, direction);
  float b = 2 * dot_product(direction, subtract_vectors(origin, vertex));
  float c = dot_product(subtract_vectors(origin, vertex),
                        subtract_vectors(origin, vertex)) -
            radius * radius;
  float delta = b * b - 4 * a * c;
  if (delta > -0.00001 && delta < 0.00001) {
    return -1;
  }
  float t1 = (-b + std::sqrt(delta)) / (2 * a);
  float t2 = (-b - std::sqrt(delta)) / (2 * a);

  if (t1 < 0.0f && t2 < 0.0f) {
    return -1;
  }

  if (t1 > -0.00001 && t1 < 0.00001) {
    return t2;
  }

  if (t2 > -0.00001 && t2 < 0.00001) {
    return t1;
  }

  return std::min(t1, t2);
}

inline float intersect_triangle(const parser::Vec3f &vertex1,
                                const parser::Vec3f &vertex2,
                                const parser::Vec3f &vertex3, const Ray &r) {
  parser::Vec3f edge1 = subtract_vectors(vertex2, vertex1);
  parser::Vec3f edge2 = subtract_vectors(vertex3, vertex1);
  parser::Vec3f h = cross_product(r.get_direction(), edge2);
  float a = dot_product(edge1, h);

  if (std::abs(a) < std::numeric_limits<float>::epsilon()) {
    return -1;
  }

  float f = 1.0f / a;
  parser::Vec3f s = subtract_vectors(r.get_origin(), vertex1);
  float u = f * dot_product(s, h);

  if (u < 0.0f || u > 1.0f)
    return -1;

  parser::Vec3f q = cross_product(s, edge1);
  float v = f * dot_product(r.get_direction(), q);

  if (v < 0.0f || u + v > 1.0f)
    return -1;

  float t = f * dot_product(edge2, q);
  if (t > 0.0f)
    return t;

  return -1;
}

inline Intersection intersect_objects(const Ray &r, const parser::Scene &s) {
  std::vector<parser::Sphere> spheres = s.spheres;
  std::vector<parser::Triangle> triangles = s.triangles;
  std::vector<parser::Mesh> meshes = s.meshes;

  float t = -1;
  Intersection min_intersection;
  min_intersection.t = std::numeric_limits<float>::infinity();
  for (parser::Sphere sphere : spheres) {
    parser::Vec3f center = s.vertex_data[sphere.center_vertex_id - 1];
    t = intersect_sphere(center, sphere.radius, r);
    if (t > 0.0f && t < min_intersection.t) {
      Intersection intersection;
      intersection.point = r.get_point(t);
      parser::Vec3f normal = subtract_vectors(intersection.point, center);
      intersection.normal = normalize(normal);
      intersection.material = s.materials[sphere.material_id - 1];
      intersection.t = t;
      min_intersection = intersection;
      min_intersection.is_null = 0;
    }
  }

  for (const parser::Triangle &triangle : s.triangles) {
    parser::Vec3f vertex1 = s.vertex_data[triangle.indices.v0_id - 1];
    parser::Vec3f vertex2 = s.vertex_data[triangle.indices.v1_id - 1];
    parser::Vec3f vertex3 = s.vertex_data[triangle.indices.v2_id - 1];

    float t = intersect_triangle(vertex1, vertex2, vertex3, r);

    if (t > 0.0f && t < min_intersection.t) {
      Intersection intersection;
      intersection.point = r.get_point(t);

      parser::Vec3f edge1 = subtract_vectors(vertex2, vertex1);
      parser::Vec3f edge2 = subtract_vectors(vertex3, vertex1);
      parser::Vec3f cross_val = cross_product(edge1, edge2);
      intersection.normal = normalize(cross_val);
      intersection.material = s.materials[triangle.material_id - 1];
      intersection.t = t;
      min_intersection = intersection;
      min_intersection.is_null = 0;
    }
  }

  for (const parser::Mesh &mesh : s.meshes) {
    for (const parser::Face &face : mesh.faces) {
      parser::Vec3f vertex1 = s.vertex_data[face.v0_id - 1];
      parser::Vec3f vertex2 = s.vertex_data[face.v1_id - 1];
      parser::Vec3f vertex3 = s.vertex_data[face.v2_id - 1];

      float t = intersect_triangle(vertex1, vertex2, vertex3, r);

      if (t > 0.0f && t < min_intersection.t) {
        Intersection intersection;
        intersection.point = r.get_point(t);

        parser::Vec3f edge1 = subtract_vectors(vertex2, vertex1);
        parser::Vec3f edge2 = subtract_vectors(vertex3, vertex1);
        parser::Vec3f cross_val = cross_product(edge1, edge2);
        intersection.normal = normalize(cross_val);

        intersection.material = s.materials[mesh.material_id - 1];
        intersection.t = t;
        min_intersection = intersection;
        min_intersection.is_null = 0;
      }
    }
  }
  return min_intersection;
}
#endif
