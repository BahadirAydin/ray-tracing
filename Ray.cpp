#include "Ray.h"
#include <cmath>

Ray generate_ray(const parser::Camera& camera, int i, int j) {
    Ray ray;
    ray.set_origin(camera.position);

    parser::Vec3f right_vector = cross_product(camera.gaze, camera.up);
    normalize(right_vector);

    parser::Vec3f up_vector = cross_product(right_vector, camera.gaze);
    normalize(up_vector);

    float l = camera.near_plane.x;
    float r = camera.near_plane.y;
    float b = camera.near_plane.z;
    float t = camera.near_plane.w;
    
    float u = l + (r - l) * (j + 0.5) / camera.image_width;
    float v = b + (t - b) * (i + 0.5) / camera.image_height;

    parser::Vec3f direction;
    direction.x = camera.gaze.x * camera.near_distance + right_vector.x * u + up_vector.x * v;
    direction.y = camera.gaze.y * camera.near_distance + right_vector.y * u + up_vector.y * v;
    direction.z = camera.gaze.z * camera.near_distance + right_vector.z * u + up_vector.z * v;

    normalize(direction);
    ray.set_direction(direction);

    return ray;
}

// Helper function to compute the cross product of two vectors
parser::Vec3f cross_product(const parser::Vec3f& a, const parser::Vec3f& b) {
    parser::Vec3f result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

// Helper function to normalize a vector
void normalize(parser::Vec3f& vec) {
    float magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    vec.x /= magnitude;
    vec.y /= magnitude;
    vec.z /= magnitude;
}

// You should also have getters and setters for origin and direction in the Ray class
