#include "ppm.h"
#include "Ray.h"

typedef unsigned char RGB[3];

int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

    for(parser::Camera cam : scene.cameras){
        int nx = cam.image_width;
        int ny = cam.image_height;
        unsigned char* image = new unsigned char [nx * ny * 3];
        int i = 0;

        // x y z w => l r b t

        float pixel_width = (cam.near_plane.y - cam.near_plane.x) / nx; 
        float pixel_height =  (cam.near_plane.w - cam.near_plane.z) / ny; 
        for (int y = 0; y < ny; ++y)
        {
            for (int x = 0; x < nx; ++x)
            {
                Ray r = generate_ray(cam, y, x, pixel_width, pixel_height );

                r.print();
            }
        }

    }
  // write_ppm("test.ppm", image, width, height);

}
