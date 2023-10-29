#include "Ray.h"
#include "parser.h"
#include "ppm.h"

typedef unsigned char RGB[3];

int main(int argc, char *argv[]) {
    // Sample usage for reading an XML scene file
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

    for (parser::Camera cam : scene.cameras) {
        int nx = cam.image_width;
        int ny = cam.image_height;
        unsigned char *image = new unsigned char[nx * ny * 3];
        // x y z w => l r b t

        int i = 0;
        float pixel_width = (cam.near_plane.y - cam.near_plane.x) / nx;
        float pixel_height = (cam.near_plane.w - cam.near_plane.z) / ny;
        for (int y = 0; y < ny; ++y) {
            for (int x = 0; x < nx; ++x) {
                Ray r = generate_ray(cam, x, y, pixel_width, pixel_height);
                for (parser::Sphere sphere : scene.spheres) {
                    // TODO find the first object hit by the ray and its surface
                    // normal n
                    //  set pixel color to value computed from hit point, light
                    //  and n
                    if (intersect_sphere(
                            scene.vertex_data[sphere.center_vertex_id - 1],
                            sphere.radius, r) > 0) {
                        // FIXME there should be color computations here
                        image[i++] = 80;
                        image[i++] = 80;
                        image[i++] = 80;
                    } else {
                        image[i++] = scene.background_color.x;
                        image[i++] = scene.background_color.y;
                        image[i++] = scene.background_color.z;
                    }
                }
            }
        }
        write_ppm("simple.ppm", image, nx, ny);
    }
}
