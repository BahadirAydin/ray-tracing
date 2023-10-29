#include "Ray.h"

#include "parser.h"

#include "utils.h"

#include <vector>

struct Intersection {
   parser::Vec3f point;
   parser::Vec3f normal;
   parser::Material material;
   float t;
};

inline float intersect_sphere(const parser::Vec3f & vertex, float radius,
   const Ray & r) {
   parser::Vec3f origin = r.get_origin();
   parser::Vec3f direction = r.get_direction();

   float a = dot_product(direction, direction);
   float b = 2 * dot_product(direction, subtract_vectors(origin, vertex));
   float c = dot_product(subtract_vectors(origin, vertex),
         subtract_vectors(origin, vertex)) -
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

inline float intersect_triangle(const parser::Vec3f & vertex1,
   const parser::Vec3f & vertex2,
      const parser::Vec3f & vertex3,
         const Ray & r) {
   // TODO implement barycentric coordinates method
   return 0;
}
// TODO
inline float intersect_mesh() {
   return 0;
}

inline std::vector < Intersection > intersect_objects(const Ray & r,
   const parser::Scene & s) {
   std::vector < parser::Sphere > spheres = s.spheres;
   std::vector < parser::Triangle > triangles = s.triangles;
   std::vector < parser::Mesh > meshes = s.meshes;

   std::vector < Intersection > intersections;

   float t = -1;
   for (parser::Sphere sphere: spheres) {
      parser::Vec3f center = s.vertex_data[sphere.center_vertex_id - 1];
      t = intersect_sphere(center, sphere.radius, r);
      if (t > 0) {
         Intersection intersection;
         intersection.point = add_vectors(
            r.get_origin(), multiply_vector(r.get_direction(), t));
         parser::Vec3f normal =
            subtract_vectors(intersection.point, center);
         normalize(normal);
         intersection.normal = normal;
         intersection.material = s.materials[sphere.material_id - 1];
         intersection.t = t;
         intersections.push_back(intersection);
      }
   }
   //TODO 
}