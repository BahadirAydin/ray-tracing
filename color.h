#ifndef COLOR_H
#define COLOR_H

#include "intersect.h"
#include "parser.h"
#include "utils.h"

inline parser::Vec3f compute_color(const parser::Scene &scene,
                                   const Intersection &intersection) {
    parser::Vec3f color = {0, 0, 0};

    // Ambient lighting component
    parser::Vec3f ambient_color = {
        scene.ambient_light.x * intersection.material.ambient.x,
        scene.ambient_light.y * intersection.material.ambient.y,
        scene.ambient_light.z * intersection.material.ambient.z};

    color.x += ambient_color.x;
    color.y += ambient_color.y;
    color.z += ambient_color.z;

    for (const parser::PointLight &light : scene.point_lights) {
        parser::Vec3f to_light = {light.position.x - intersection.point.x,
                                  light.position.y - intersection.point.y,
                                  light.position.z - intersection.point.z};
        float distance_to_light =
            sqrt(to_light.x * to_light.x + to_light.y * to_light.y +
                 to_light.z * to_light.z);

        if (distance_to_light > 0) {
            to_light.x /= distance_to_light;
            to_light.y /= distance_to_light;
            to_light.z /= distance_to_light;

            float d = dot_product(to_light, intersection.normal);

            if (d > 0) {
                parser::Vec3f diffuse_color = {
                    intersection.material.diffuse.x * light.intensity.x * d,
                    intersection.material.diffuse.y * light.intensity.y * d,
                    intersection.material.diffuse.z * light.intensity.z * d};

                color.x += diffuse_color.x;
                color.y += diffuse_color.y;
                color.z += diffuse_color.z;
            }
        }
    }

    color.x = std::max(0.0f, std::min(255.0f, color.x));
    color.y = std::max(0.0f, std::min(255.0f, color.y));
    color.z = std::max(0.0f, std::min(255.0f, color.z));

    return color;
}

inline parser::Vec3f compute_background_color(const parser::Vec3f &pixel,
                                              const parser::Scene &scene) {
    parser::Vec3f color = {0, 0, 0};

    parser::Vec3f ambient_color = {
        scene.ambient_light.x * scene.background_color.x,
        scene.ambient_light.y * scene.background_color.y,
        scene.ambient_light.z * scene.background_color.z};

    color.x += ambient_color.x;
    color.y += ambient_color.y;
    color.z += ambient_color.z;

    for (const parser::PointLight &light : scene.point_lights) {
        parser::Vec3f to_light = {light.position.x - pixel.x,
                                  light.position.y - pixel.y,
                                  light.position.z - pixel.z};
        float distance_to_light =
            sqrt(to_light.x * to_light.x + to_light.y * to_light.y +
                 to_light.z * to_light.z);

        if (distance_to_light > 0) {
            to_light.x /= distance_to_light;
            to_light.y /= distance_to_light;
            to_light.z /= distance_to_light;

            // HACK burada gaze vektörünün tersini kullanmamız gerekiyor olabilir ama kafam karıştı
            float d = dot_product(to_light, {0, 0, 1});

            if (d > 0) {
                parser::Vec3f diffuse_color = {
                    scene.background_color.x * light.intensity.x * d,
                    scene.background_color.y * light.intensity.y * d,
                    scene.background_color.z * light.intensity.z * d};

                color.x += diffuse_color.x;
                color.y += diffuse_color.y;
                color.z += diffuse_color.z;
            }
        }
    }
    color.x = std::max(0.0f, std::min(255.0f, color.x));
    color.y = std::max(0.0f, std::min(255.0f, color.y));
    color.z = std::max(0.0f, std::min(255.0f, color.z));
    return color;
}

#endif