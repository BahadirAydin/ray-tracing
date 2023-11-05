#include "Ray.h"
#include "color.h"
#include "intersect.h"
#include "parser.h"
#include "ppm.h"
#include "utils.h"
#include <pthread.h>

typedef unsigned char RGB[3];

struct ThreadData {
  int start;
  int end;
  int nx;
  parser::Camera *cam;
  unsigned char *image;
  float pixel_width;
  float pixel_height;
  parser::Scene *scene;
  int i;
};

void *render(void *args) {
  struct ThreadData *data;
  data = (struct ThreadData *)args;
  int start = data->start;
  int end = data->end;
  int nx = data->nx;
  parser::Camera *cam = data->cam;
  unsigned char *image = data->image;
  float pixel_width = data->pixel_width;
  float pixel_height = data->pixel_height;
  parser::Scene *scene = data->scene;
  int i = data->i;

  for (int y = start; y < end; ++y) {
    for (int x = 0; x < nx; ++x) {
      Ray r = generate_ray(*cam, x, y, pixel_width, pixel_height);
      Intersection intersection = intersect_objects(r, *scene);
      parser::Vec3i color = compute_color(*scene, intersection, r);
      image[i++] = color.x;
      image[i++] = color.y;
      image[i++] = color.z;
    }
  }
  pthread_exit(NULL);
}

const int THREADS = 8;

int main(int argc, char *argv[]) {
  // Sample usage for reading an XML scene file
  parser::Scene scene;

  scene.loadFromXml(argv[1]);

  pthread_t threads[THREADS];
  struct ThreadData thread_data[THREADS];
  int nx, ny;

  for (parser::Camera cam : scene.cameras) {
    nx = cam.image_width;
    ny = cam.image_height;
    unsigned char *image = new unsigned char[nx * ny * 3];
    float pixel_width = (cam.near_plane.y - cam.near_plane.x) / nx;
    float pixel_height = (cam.near_plane.w - cam.near_plane.z) / ny;

    int rows_per_thread = ny / THREADS;
    int remaining_rows = ny % THREADS;
    int start = 0;
    int i = 0;

    for (int t = 0; t < THREADS; t++) {
      int end = start + rows_per_thread;
      if (t == THREADS - 1) {
        end += remaining_rows;
      }
      i = start * nx * 3;

      thread_data[t] = {start,       end,          nx,     &cam, image,
                        pixel_width, pixel_height, &scene, i};
      pthread_create(&threads[t], NULL, render, &thread_data[t]);

      start = end;
    }

    for (int t = 0; t < THREADS; t++) {
      pthread_join(threads[t], NULL);
    }

    write_ppm(cam.image_name.c_str(), image, nx, ny);
    delete[] image;
  }

  return 0;
}
