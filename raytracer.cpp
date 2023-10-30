#include "Ray.h"

#include "intersect.h"

#include "parser.h"

#include "ppm.h"

typedef unsigned char RGB[3];

int main(int argc, char * argv[]) {
   // Sample usage for reading an XML scene file
   parser::Scene scene;

   scene.loadFromXml(argv[1]);

   for (parser::Camera cam: scene.cameras) {
      int nx = cam.image_width;
      int ny = cam.image_height;
      unsigned char * image = new unsigned char[nx * ny * 3];
      // x y z w => l r b t

      int i = 0;
      float pixel_width = (cam.near_plane.y - cam.near_plane.x) / nx;
      float pixel_height = (cam.near_plane.w - cam.near_plane.z) / ny;
      for (int y = 0; y < ny; ++y) {
         for (int x = 0; x < nx; ++x) {
            Ray r = generate_ray(cam, x, y, pixel_width, pixel_height);
            
            for (parser::Sphere sphere: scene.spheres) {
               float t = intersect_sphere(
                  scene.vertex_data[sphere.center_vertex_id - 1],
                  sphere.radius, r);

               if (t > 0) {
                  parser::Material material =
                     scene.materials[sphere.material_id - 1];

                  // Calculate the intersection point
                  parser::Vec3f intersection_point =
                     add_vectors(r.get_origin(),
                        multiply_vector(r.get_direction(), t));

                  parser::Vec3f sphere_diffuse_color = material.diffuse;
                  image[i++] = static_cast < unsigned char > (
                     sphere_diffuse_color.x * 255);
                  image[i++] = static_cast < unsigned char > (
                     sphere_diffuse_color.y * 255);
                  image[i++] = static_cast < unsigned char > (
                     sphere_diffuse_color.z * 255);
               } else {
                  // No intersection with the sphere, set the background
                  // color
                  image[i++] = static_cast < unsigned char > (
                     scene.background_color.x * 255);
                  image[i++] = static_cast < unsigned char > (
                     scene.background_color.y * 255);
                  image[i++] = static_cast < unsigned char > (
                     scene.background_color.z * 255);
               }
            }


            for (parser::Triangle triangle: scene.triangles) {
                const parser::Vec3f &v0 = scene.vertex_data[triangle.indices.v0_id - 1];
                const parser::Vec3f &v1 = scene.vertex_data[triangle.indices.v1_id - 1];
                const parser::Vec3f &v2 = scene.vertex_data[triangle.indices.v2_id - 1];

                float t = intersect_triangle(v0, v1, v2, r);

                if (t > 0) {
                    parser::Material material = scene.materials[triangle.material_id - 1];

                    // Calculate the intersection point
                    parser::Vec3f intersection_point =
                        add_vectors(r.get_origin(), multiply_vector(r.get_direction(), t));

                    parser::Vec3f triangle_diffuse_color = material.diffuse;
                    image[i++] = static_cast<unsigned char>(triangle_diffuse_color.x * 255);
                    image[i++] = static_cast<unsigned char>(triangle_diffuse_color.y * 255);
                    image[i++] = static_cast<unsigned char>(triangle_diffuse_color.z * 255);
                } else {
                    // No intersection with the triangle, set the background color
                    image[i++] = static_cast<unsigned char>(scene.background_color.x * 255);
                    image[i++] = static_cast<unsigned char>(scene.background_color.y * 255);
                    image[i++] = static_cast<unsigned char>(scene.background_color.z * 255);
                }
            }
            
         }
      }
      write_ppm("simple.ppm", image, nx, ny);
   }
}