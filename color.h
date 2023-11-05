#ifndef COLOR_H
#define COLOR_H

#include "Ray.h"
#include "intersect.h"
#include "parser.h"
#include "utils.h"
#include <limits>

inline parser::Vec3i compute_color(const parser::Scene &scene,
                                   const Intersection &intersection, Ray &r);

inline parser::Vec3f calculate_irradiance(const parser::PointLight &light,
                                          const parser::Vec3f &light_dir,
                                          const parser::Vec3f &normal,
                                          float distance_to_light) {

  float distance_to_light_squared = std::pow(distance_to_light, 2);
  if (distance_to_light > 0.0f) {
    float irradiance_x = light.intensity.x / distance_to_light_squared;
    float irradiance_y = light.intensity.y / distance_to_light_squared;
    float irradiance_z = light.intensity.z / distance_to_light_squared;
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
                                        const parser::Vec3f &half,
                                        const parser::Vec3f &to_light) {

  parser::Vec3f specular = {0, 0, 0};
  float cos_alpha_prime = std::max(0.0f, dot_product(normal, half));
  float angle = angle_bw_vectors(to_light, half);

  if (angle < 90) {
    float b = std::pow(cos_alpha_prime, phong);
    specular.x = material_specular.x * irradiance.x * b;
    specular.y = material_specular.y * irradiance.y * b;
    specular.z = material_specular.z * irradiance.z * b;
  }
  return specular;
}

inline Ray generate_shadow_ray(float eps, const parser::Vec3f &normalized_light,
                               const parser::Vec3f &intersection_point) {
  const parser::Vec3f origin =
      add_vectors(intersection_point, multiply_vector(normalized_light, eps));
  const parser::Vec3f direction = multiply_vector(normalized_light, eps);
  return Ray(origin, direction);
}

inline parser::Vec3f apply_shading(const parser::Scene &scene,
                                   const Intersection &intersection, Ray &r) {

  // start with the ambient light
  parser::Vec3f ambient_color = {
      scene.ambient_light.x * intersection.material->ambient.x,
      scene.ambient_light.y * intersection.material->ambient.y,
      scene.ambient_light.z * intersection.material->ambient.z};

  parser::Vec3f color;
  color.x = ambient_color.x;
  color.y = ambient_color.y;
  color.z = ambient_color.z;
  parser::Vec3f eye_v = subtract_vectors(r.get_origin(), intersection.point);
  parser::Vec3f normalized_eye_v = normalize(eye_v);

  int current_depth = r.get_depth();

  // add the color from the mirror direction
  if (intersection.material->is_mirror &&
      current_depth < scene.max_recursion_depth) {
    float cos_theta = dot_product(intersection.normal, normalized_eye_v);
    parser::Vec3f reflected_ray_dir =
        add_vectors(multiply_vector(normalized_eye_v, -1),
                    multiply_vector(intersection.normal, (2 * cos_theta)));

    parser::Vec3f reflected_ray_origin = add_vectors(
        intersection.point,
        multiply_vector(reflected_ray_dir, scene.shadow_ray_epsilon));

    Ray reflected_ray(reflected_ray_origin, reflected_ray_dir);

    Intersection reflected_intersection =
        intersect_objects(reflected_ray, scene);

    if (!reflected_intersection.is_null && reflected_intersection.t > 0.0f) {
      reflected_ray.set_depth(current_depth + 1);
      parser::Vec3i reflected_color =
          compute_color(scene, reflected_intersection, reflected_ray);

      reflected_color.x *= intersection.material->mirror.x;
      reflected_color.y *= intersection.material->mirror.y;
      reflected_color.z *= intersection.material->mirror.z;

      color.x += reflected_color.x;
      color.y += reflected_color.y;
      color.z += reflected_color.z;
    }
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
        min_t < get_magn(to_light_normalized)) {
      parser::Vec3f irradiance = calculate_irradiance(
          light, to_light_normalized, intersection.normal, distance_to_light);
      parser::Vec3f diffuse =
          calculate_diffuse(intersection.material->diffuse, irradiance,
                            intersection.normal, to_light_normalized);
      parser::Vec3f half = add_vectors(to_light_normalized, normalized_eye_v);
      parser::Vec3f normalized_half = normalize(half);
      parser::Vec3f specular = calculate_specular(
          intersection.material->phong_exponent, intersection.normal,
          intersection.material->specular, irradiance, normalized_half,
          to_light);

      color.x += diffuse.x + specular.x;
      color.y += diffuse.y + specular.y;
      color.z += diffuse.z + specular.z;
    }
  }

  return color;
}

inline parser::Vec3i compute_color(const parser::Scene &scene,
                                   const Intersection &intersection, Ray &r) {

  if (r.get_depth() > scene.max_recursion_depth) {
    return {0, 0, 0};
  }
  if (!intersection.is_null) {
    return float_to_int_color(clamp(apply_shading(scene, intersection, r)));

  } else if (r.get_depth() == 0) {
    return scene.background_color;

  } else {
    return {0, 0, 0};
  }
}

#endif
