#include "Ray.h"
#include "color.h"
#include "intersect.h"
#include "parser.h"
#include "ppm.h"
#include "utils.h"

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
                Intersection intersection = intersect_objects(r, scene);
                if (intersection.t == std::numeric_limits<float>::infinity()) {

                    image[i] = scene.background_color.x;
                    image[i + 1] = scene.background_color.y;
                    image[i + 2] = scene.background_color.z;
                } else {
                    parser::Vec3f color = compute_color(scene, intersection, r);
                    image[i] = color.x + 0.5;
                    image[i + 1] = color.y + 0.5;
                    image[i + 2] = color.z + 0.5;
                }
                i += 3;
            }
        }
        write_ppm("simple.ppm", image, nx, ny);
    }
}
