#ifndef COLOR_H
#define COLOR_H

#include "Ray.h"
#include "intersect.h"
#include "parser.h"
#include "utils.h"
#include <limits>

inline parser::Vec3f compute_color(const parser::Scene &scene,
                                   const Intersection &intersection, Ray &r);

inline parser::Vec3f calculate_irradiance(const parser::PointLight &light,
                                          const parser::Vec3f &light_dir,
                                          const parser::Vec3f &normal,
                                          float distance_to_light) {

  if (distance_to_light > 0) {
    float irradiance_x = light.intensity.x / std::pow(distance_to_light, 2);
    float irradiance_y = light.intensity.y / std::pow(distance_to_light, 2);
    float irradiance_z = light.intensity.z / std::pow(distance_to_light, 2);
    return {irradiance_x, irradiance_y, irradiance_z};
  }
  return {0, 0, 0};
}

inline parser::Vec3f
calculate_diffuse(const parser::Vec3f &material_diffuse,
                  const parser::Vec3f &irradiance, const parser::Vec3f &normal,
                  const parser::Vec3f &light_dir_normalized) {
  parser::Vec3f diffuse = {0, 0, 0};
  float theta = std::max(0.0f, dot_product(normal, light_dir_normalized));
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
  parser::Vec3f origin = add_vectors(intersection_point,
                                     multiply_vector(normalized_light, eps));
  shadow_ray.set_origin(origin);
  shadow_ray.set_direction(normalized_light);
  return shadow_ray;
}

inline parser::Vec3f apply_shading(const parser::Scene &scene,
                                   const Intersection &intersection, Ray &r) {

  // start with the ambient light
  parser::Vec3f ambient_color = {
      scene.ambient_light.x * intersection.material.ambient.x,
      scene.ambient_light.y * intersection.material.ambient.y,
      scene.ambient_light.z * intersection.material.ambient.z};

  parser::Vec3f color;
  color.x = ambient_color.x;
  color.y = ambient_color.y;
  color.z = ambient_color.z;
  parser::Vec3f eye_v = subtract_vectors(r.get_origin(), intersection.point);
  parser::Vec3f normalized_eye_v = normalize(eye_v);

  // add the color from the mirror direction
  if (intersection.material.is_mirror) {
    Ray reflected_ray;
    float cos_theta = dot_product(intersection.normal, normalized_eye_v);
    parser::Vec3f reflected_ray_dir =
        add_vectors(multiply_vector(normalized_eye_v, -1),
                    multiply_vector(intersection.normal, (2 * cos_theta)));

    parser::Vec3f reflected_ray_origin = add_vectors(
        intersection.point,
        multiply_vector(reflected_ray_dir, scene.shadow_ray_epsilon));

    reflected_ray.set_direction(reflected_ray_dir);
    reflected_ray.set_origin(reflected_ray_origin);

    reflected_ray.set_depth(r.get_depth() + 1);
    color = add_vectors(
        color,
        multiply_vector(compute_color(scene, intersection, reflected_ray),
                        intersection.material.phong_exponent));
  }

  // add the diffuse and specular terms

  for (const parser::PointLight &light : scene.point_lights) {
    parser::Vec3f to_light =
        subtract_vectors(light.position, intersection.point);
    parser::Vec3f to_light_normalized = normalize(to_light);

    Ray shadow_ray = generate_shadow_ray(
        scene.shadow_ray_epsilon, to_light_normalized, intersection.point);

    Intersection shadow_intersection = intersect_objects(shadow_ray, scene);
    float distance_to_light = get_magn(to_light);

    float min_t = shadow_intersection.t;

    if (min_t == std::numeric_limits<float>::infinity() ||
        min_t - get_magn(to_light_normalized) < 0.0f) {
      parser::Vec3f irradiance = calculate_irradiance(
          light, to_light_normalized, intersection.normal, distance_to_light);
      parser::Vec3f diffuse =
          calculate_diffuse(intersection.material.diffuse, irradiance,
                            intersection.normal, to_light_normalized);
      parser::Vec3f half = add_vectors(to_light_normalized, normalized_eye_v);
      parser::Vec3f normalized_half = normalize(half);
      parser::Vec3f specular = calculate_specular(
          intersection.material.phong_exponent, intersection.normal,
          intersection.material.specular, irradiance, normalized_half);

      color.x += diffuse.x + specular.x;
      color.y += diffuse.y + specular.y;
      color.z += diffuse.z + specular.z;
    }
  }

  return color;
}

inline parser::Vec3f compute_color(const parser::Scene &scene,
                                   const Intersection &intersection, Ray &r) {

  if (r.get_depth() > scene.max_recursion_depth) {

    parser::Vec3f color = {0, 0, 0};
    return color;
  }

  if (!intersection.is_null) {
    return apply_shading(scene, intersection, r);
  } else if (r.get_depth() == 0) {
    parser::Vec3f background_color = {(float)scene.background_color.x,
                                      (float)scene.background_color.y,
                                      (float)scene.background_color.z};
    return background_color;
  } else {
    parser::Vec3f color = {0, 0, 0};
    return color;
  }
}

inline parser::Vec3i compute_background_color(const parser::Scene &scene) {
  parser::Vec3i color = scene.background_color;
  return color;
}

#endif
