#include <iostream>
#include <time.h>
#include <cstdlib>

#include "utility.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include "bvh.h"
#include "pdf.h"

color ray_color(const ray &r, const color &background, const hittable &world, shared_ptr<hittable> &lights, int depth) {
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered
  if (depth <= 0) {
    return color(0, 0, 0);
  }

  // If the ray hits nothing, return the background color
  if (!world.hit(r, 0.001, infinity, rec)) {
    return background;
  }

  scatter_record srec;
  color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
  if (!rec.mat_ptr->scatter(r, rec, srec)) {
    return emitted;
  }

  if (srec.is_specular) {
    return srec.attenuation * ray_color(srec.specular_ray, background, world, lights, depth-1);
  }

  auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
  mixture_pdf p(light_ptr, srec.pdf_ptr);

  ray scattered = ray(rec.p, p.generate(), r.time());
  auto pdf_val = p.value(scattered.direction());

  return emitted
      + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
                         * ray_color(scattered, background, world, lights, depth-1) / pdf_val;
}

hittable_list cornell_box() {
  hittable_list objects;

  auto red   = make_shared<lambertian>(color(.65, .05, .05));
  auto white = make_shared<lambertian>(color(.73, .73, .73));
  auto green = make_shared<lambertian>(color(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color(15, 15, 15));

  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
  objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

  shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
  shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), aluminum);
  // shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3(265,0,295));
  objects.add(box1);

  shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3(130,0,65));
  objects.add(box2);

  return objects;
}

int main() {
  srand(time(NULL));

  // -- Image
  auto aspect_ratio = 16.0 / 9.0;
  int image_width = 900;
  int samples_per_pixel = 100;
  const int max_depth = 50;

  // -- World
  hittable_list world;

  point3 lookfrom;
  point3 lookat;
  auto vfov = 40.0;
  auto aperture = 0.0;
  color background(0, 0, 0);

  switch (0) {
    default:
    case 6:
      world = cornell_box();
      aspect_ratio = 1.0;
      image_width = 600;
      samples_per_pixel = 200;
      background = color(0,0,0);
      lookfrom = point3(278, 278, -800);
      lookat = point3(278, 278, 0);
      vfov = 40.0;
      break;
  }

  shared_ptr<hittable> lights = make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>());

  // auto lights = make_shared<hittable_list>();
  // lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
  // lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));

  // -- Camera
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  const int image_height = static_cast<int>(image_width / aspect_ratio);

  camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  // -- Render
  // Header
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  // Body
  for (int y = image_height-1; y >= 0; --y) {
    std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;
    for (int x = 0; x < image_width; ++x) {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        auto px = (x + random_double()) / (image_width-1);
        auto py = (y + random_double()) / (image_height-1);
        ray r = cam.get_ray(px, py);
        pixel_color += ray_color(r, background, world, lights, max_depth);
      }
      write_color(std::cout, pixel_color, samples_per_pixel);
    }
  }

  std::cerr << "\nDone.\n";
}
