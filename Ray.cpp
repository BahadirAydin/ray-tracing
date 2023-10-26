#include "parser.h"
#include "utils.cpp"

class Ray {
public:
    parser::Vec3f origin;
    parser::Vec3f direction;
};

Ray generate_ray(const parser::Camera& camera, int i, int j, float pixel_width, float pixel_height) {
    // Extracting values from the Camera object for clarity
    parser::Vec3f e = camera.position;
    parser::Vec3f gaze = camera.gaze;
    parser::Vec3f up = camera.up;
    parser::Vec4f near_plane = camera.near_plane;
    float distance = camera.near_distance;
    
    // Compute the w, u, v vectors
    parser::Vec3f w = {-gaze.x, -gaze.y, -gaze.z}; 
    parser::Vec3f u = cross_product(up, w);
    parser::Vec3f v = {w.y * u.z - w.z * u.y, w.z * u.x - w.x * u.z, w.x * u.y - w.y * u.x}; 

    // Calculate su and sv
    float su =  (i + 0.5) * pixel_width;
    float sv =  (j + 0.5) * pixel_height;

    // Calculate q
    parser::Vec3f m = {e.x + gaze.x * distance, e.y + gaze.y * distance, e.z + gaze.z * distance}; 
    parser::Vec3f q = {
        m.x + near_plane.x * u.x + up.x * near_plane.w,
        m.y + near_plane.x * u.y + up.y * near_plane.w,
        m.z + near_plane.x * u.z + up.z * near_plane.w
    };

    // Calculate the direction d from e to q
    parser::Vec3f d = {q.x - e.x, q.y - e.y, q.z - e.z};

    // Construct the Ray
    Ray ray;
    ray.origin = e;
    ray.direction = d;

    return ray;
}
