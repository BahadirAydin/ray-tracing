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

    // iterate over each single pixel
    for (int y = 0; y < ny; ++y) {
      for (int x = 0; x < nx; ++x) {

        // compute the viewing ray
        Ray r = generate_ray(cam, x, y, pixel_width, pixel_height);

        // find the closest intersection with an object for each ray
        Intersection intersection = intersect_objects(r, scene);
        // color is clamped and added +0.5 in the compute_color function
        parser::Vec3i color = compute_color(scene, intersection, r);
        image[i++] = color.x;
        image[i++] = color.y;
        image[i++] = color.z;
      }
    }

    write_ppm(cam.image_name.c_str(), image, nx, ny);
  }
}
