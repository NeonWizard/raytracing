#include <iostream>

#include "color.h"
#include "vec3.h"

int main() {
  // -- Image
  const int image_width = 256;
  const int image_height = 256;

  // -- Render
  // Header
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  // Body
  for (int y = image_height-1; y >= 0; --y) {
    std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;
    for (int x = 0; x < image_width; ++x) {
      color pixel_color(
        double(x) / (image_width-1),
        double(y) / (image_height-1),
        0.25
      );
      write_color(std::cout, pixel_color);
    }
  }

  std::cerr << "\nDone.\n";
}
