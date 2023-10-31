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
                std::vector<Intersection> intersection =
                    intersect_objects(r, scene);
                float min_t = std::numeric_limits<float>::max();
                for (Intersection inter : intersection) {
                    if (inter.t < min_t) {
                        min_t = inter.t;
                        parser::Vec3f color = compute_color(scene, inter);
                        image[i] = color.x;
                        image[i + 1] = color.y;
                        image[i + 2] = color.z;
                    }
                }
                if (min_t == std::numeric_limits<float>::max()) {
                    parser::Vec3f pixel =
                        add_vectors(r.get_origin(), r.get_direction());
                    parser::Vec3f color =
                        compute_background_color(pixel, scene);
                    image[i] = color.x;
                    image[i + 1] = color.y;
                    image[i + 2] = color.z;
                }
                i += 3;
            }
        }
        write_ppm("simple.ppm", image, nx, ny);
    }
}
