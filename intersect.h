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

    if (t1 < 0.00001 && t2 < 0.00001) {
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

    if (a > -0.00001 && a < 0.00001)
        return -1;

    float f = 1.0 / a;
    parser::Vec3f s = subtract_vectors(r.get_origin(), vertex1);
    float u = f * dot_product(s, h);

    if (u < 0.0 || u > 1.0)
        return -1;

    parser::Vec3f q = cross_product(s, edge1);
    float v = f * dot_product(r.get_direction(), q);

    if (v < 0.0 || u + v > 1.0)
        return -1;

    float t = f * dot_product(edge2, q);
    if (t > 0.00001)
        return t;

    return -1;
}

inline float intersect_mesh(const parser::Mesh &mesh,
                            const std::vector<parser::Vec3f> &vertex_data,
                            const Ray &r) {
    float closest_t = std::numeric_limits<float>::infinity();

    for (const parser::Face &face : mesh.faces) {
        const parser::Vec3f &v0 = vertex_data[face.v0_id - 1];
        const parser::Vec3f &v1 = vertex_data[face.v1_id - 1];
        const parser::Vec3f &v2 = vertex_data[face.v2_id - 1];

        float t = intersect_triangle(v0, v1, v2, r);

        if (t > 0 && t < closest_t) {
            closest_t = t;
        }
    }

    return closest_t == std::numeric_limits<float>::infinity() ? -1 : closest_t;
}

inline std::vector<Intersection> intersect_objects(const Ray &r,
                                                   const parser::Scene &s) {
    std::vector<parser::Sphere> spheres = s.spheres;
    std::vector<parser::Triangle> triangles = s.triangles;
    std::vector<parser::Mesh> meshes = s.meshes;

    std::vector<Intersection> intersections;

    float t = -1;
    for (parser::Sphere sphere : spheres) {
        parser::Vec3f center = s.vertex_data[sphere.center_vertex_id - 1];
        t = intersect_sphere(center, sphere.radius, r);
        if (std::abs(t - 0.0001) > 0) {
            Intersection intersection;
            intersection.point = r.get_point(t);
            parser::Vec3f normal = subtract_vectors(intersection.point, center);
            normal = normalize(normal);
            intersection.normal = normal;
            intersection.material = s.materials[sphere.material_id - 1];
            intersection.t = t;
            intersections.push_back(intersection);
        }
    }

    for (const parser::Triangle &triangle : s.triangles) {
        parser::Vec3f vertex1 = s.vertex_data[triangle.indices.v0_id - 1];
        parser::Vec3f vertex2 = s.vertex_data[triangle.indices.v1_id - 1];
        parser::Vec3f vertex3 = s.vertex_data[triangle.indices.v2_id - 1];

        float t = intersect_triangle(vertex1, vertex2, vertex3, r);

        if (t > 0) {
            Intersection intersection;
            intersection.point = r.get_point(t);

            parser::Vec3f edge1 = subtract_vectors(vertex2, vertex1);
            parser::Vec3f edge2 = subtract_vectors(vertex3, vertex1);
            parser::Vec3f cross_val = cross_product(edge1, edge2);
            intersection.normal = normalize(cross_val);

            intersection.material = s.materials[triangle.material_id - 1];
            intersection.t = t;
            intersections.push_back(intersection);
        }
    }

    for (const parser::Mesh &mesh : s.meshes) {
        for (const parser::Face &face : mesh.faces) {
            parser::Vec3f vertex1 = s.vertex_data[face.v0_id - 1];
            parser::Vec3f vertex2 = s.vertex_data[face.v1_id - 1];
            parser::Vec3f vertex3 = s.vertex_data[face.v2_id - 1];

            float t = intersect_triangle(vertex1, vertex2, vertex3, r);

            if (t > 0) {
                Intersection intersection;
                intersection.point = r.get_point(t);

                parser::Vec3f edge1 = subtract_vectors(vertex2, vertex1);
                parser::Vec3f edge2 = subtract_vectors(vertex3, vertex1);
                parser::Vec3f cross_val = cross_product(edge1, edge2);
                intersection.normal = normalize(cross_val);

                intersection.material = s.materials[mesh.material_id - 1];
                intersection.t = t;
                intersections.push_back(intersection);
            }
        }
    }
    return intersections;
}
#endif
