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

    bool print_flag = false;
    // iterate over each single pixel
    for (int y = 0; y < ny; ++y) {
      for (int x = 0; x < nx; ++x) {

        if (x == 400 && y == 400) {
          print_flag = true;
        } else {
          print_flag = false;
        }
        // compute the viewing ray
        Ray r = generate_ray(cam, x, y, pixel_width, pixel_height);

        // primary ray
        r.init_depth();

        // find the closest intersection with an object for each ray
        Intersection intersection = intersect_objects(r, scene);
        parser::Vec3f color = compute_color(scene, intersection, r, print_flag);
        color = clamp(color);

        if (print_flag) {
          std::cout << "color: " << color.x << " " << color.y << " " << color.z
                    << std::endl;
        }

        image[i++] = color.x + 0.5;
        image[i++] = color.y + 0.5;
        image[i++] = color.z + 0.5;
      }
    }

    write_ppm(cam.image_name.c_str(), image, nx, ny);
  }
}
