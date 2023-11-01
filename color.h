#ifndef COLOR_H
#define COLOR_H

#include "Ray.h"
#include "intersect.h"
#include "parser.h"
#include "utils.h"
#include <limits>

inline parser::Vec3f calculate_irradiance(const parser::PointLight &light,
                                          const parser::Vec3f &to_light,
                                          const parser::Vec3f &normal) {
    float distance_to_light = get_magn(to_light);

    if (distance_to_light > 0) {
        float irradiance_x =
            light.intensity.x / (distance_to_light * distance_to_light);
        float irradiance_y =
            light.intensity.y / (distance_to_light * distance_to_light);
        float irradiance_z =
            light.intensity.z / (distance_to_light * distance_to_light);
        return {irradiance_x, irradiance_y, irradiance_z};
    }
    return {0, 0, 0};
}

inline parser::Vec3f
calculate_diffuse(const parser::Vec3f &material_diffuse,
                  const parser::Vec3f &irradiance, const parser::Vec3f &normal,
                  const parser::Vec3f &to_light_normalized) {
    parser::Vec3f diffuse = {0, 0, 0};
    float theta = std::max(0.0f, dot_product(normal, to_light_normalized));
    diffuse.x = material_diffuse.x * irradiance.x * theta;
    diffuse.y = material_diffuse.y * irradiance.y * theta;
    diffuse.z = material_diffuse.z * irradiance.z * theta;
    return diffuse;
}

inline parser::Vec3f calculate_specular(float phong,
                                        const parser::Vec3f &normal,
                                        const parser::Vec3f &material_specular,
                                        const parser::Vec3f &irradiance,
                                        const parser::Vec3f &half) {
    parser::Vec3f specular = {0, 0, 0};
    float a = std::max(0.0f, dot_product(normal, half));
    float b = std::pow(a, phong);
    specular.x = material_specular.x * irradiance.x * b;
    specular.y = material_specular.y * irradiance.y * b;
    specular.z = material_specular.z * irradiance.z * b;
    return specular;
}

inline Ray generate_shadow_ray(float eps, const parser::Vec3f &normalized_light,
                               const parser::Vec3f &intersection_point) {
    Ray shadow_ray;
    shadow_ray.set_origin(add_vectors(intersection_point,
                                      multiply_vector(normalized_light, eps)));
    shadow_ray.set_direction(normalized_light);
    return shadow_ray;
}

inline parser::Vec3f compute_color(const parser::Scene &scene,
                                   const Intersection &intersection,
                                   const Ray &r) {
    parser::Vec3f color = {0, 0, 0};

    parser::Vec3f ambient_color = {
        scene.ambient_light.x * intersection.material.ambient.x,
        scene.ambient_light.y * intersection.material.ambient.y,
        scene.ambient_light.z * intersection.material.ambient.z};

    color.x += ambient_color.x;
    color.y += ambient_color.y;
    color.z += ambient_color.z;

    parser::Vec3f eye_v = subtract_vectors(r.get_origin(), intersection.point);
    parser::Vec3f normalized_eye_v = normalize(eye_v);

    for (const parser::PointLight &light : scene.point_lights) {
        parser::Vec3f to_light =
            subtract_vectors(light.position, intersection.point);
        parser::Vec3f to_light_normalized = normalize(to_light);

        Ray shadow_ray = generate_shadow_ray(
            scene.shadow_ray_epsilon, to_light_normalized, intersection.point);

        Intersection shadow_intersection = intersect_objects(shadow_ray, scene);

        float min_t = shadow_intersection.t;
        if (min_t == std::numeric_limits<float>::infinity()) {
            min_t = 0.0f;
        }
        if (min_t - get_magn(to_light) < 0.0f) {
            parser::Vec3f irradiance = calculate_irradiance(
                light, to_light_normalized, intersection.normal);
            parser::Vec3f diffuse =
                calculate_diffuse(intersection.material.diffuse, irradiance,
                                  intersection.normal, to_light_normalized);
            parser::Vec3f half =
                (add_vectors(to_light_normalized, normalized_eye_v));
            parser::Vec3f specular = calculate_specular(
                intersection.material.phong_exponent, intersection.normal,
                intersection.material.specular, irradiance, normalize(half));
            color.x += diffuse.x + specular.x;
            color.y += diffuse.y + specular.y;
            color.z += diffuse.z + specular.z;
        }
    }

    color.x = std::max(0.0f, std::min(255.0f, color.x));
    color.y = std::max(0.0f, std::min(255.0f, color.y));
    color.z = std::max(0.0f, std::min(255.0f, color.z));

    return color;
}

inline parser::Vec3i compute_background_color(const parser::Scene &scene) {
    parser::Vec3i color = scene.background_color;
    return color;
}

#endif
