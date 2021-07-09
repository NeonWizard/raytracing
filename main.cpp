#include <iostream>

#include "utility.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"

color ray_color(const ray &r, const hittable &world) {
  hit_record rec;
  if (world.hit(r, 0, infinity, rec)) {
    return 0.5 * (rec.normal + color(1, 1, 1));
  }

  // -- Sky
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
  // -- Image
  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 400;
  const int image_height = static_cast<int>(image_width / aspect_ratio);

  // -- World
  hittable_list world;
  world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
  world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

  // -- Camera
  auto viewport_height = 2.0;
  auto viewport_width = aspect_ratio * viewport_height;
  auto focal_length = 1.0;

  auto origin = point3(0, 0, 0);
  auto horizontal = vec3(viewport_width, 0, 0);
  auto vertical = vec3(0, viewport_height, 0);
  auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

  // -- Render
  // Header
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  // Body
  for (int y = image_height-1; y >= 0; --y) {
    std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;
    for (int x = 0; x < image_width; ++x) {
      auto px = double(x) / (image_width-1);
      auto py = double(y) / (image_height-1);
      ray r(origin, lower_left_corner + px*horizontal + py*vertical - origin);
      color pixel_color = ray_color(r, world);
      write_color(std::cout, pixel_color);
    }
  }

  std::cerr << "\nDone.\n";
}
