#include <iostream>
#include <string>
#include <vips/vips8>

using namespace std::string_literals;

int main(int argc, char *argv[]) {
    std::string usage_str;
    usage_str = usage_str + "Usage: " + argv[0] + " <input-filename> <output-filename>\n";

    if (argc == 2 && (argv[1] == "-h"s || argv[1] == "--help"s)) {
        std::cout << usage_str;
        return 0;
    }
    if (argc != 3) {
        std::cout << usage_str;
        return -1;
    }

    if (VIPS_INIT (argv[0])) {
        vips_error_exit (NULL);
    }

    auto input = vips::VImage::new_from_file (argv[1]);

    auto in_copy = input.copy_memory();

    // find the width of the corridor
    size_t corridor_width = 0;
    while (in_copy(corridor_width + 1, corridor_width + 1)[0] != 0) {
        ++corridor_width;

        if (corridor_width + 2 > in_copy.width()
            || corridor_width + 2 > in_copy.height()) {
            std::cout << "Corridor width seems to be greater than image size "
                "allows. Exiting program.\n";
            return -1;
        }
    }

    if (corridor_width % 2 == 0) {
        // can't draw a line in the middle of an even-sized corridor
        std::cout << "Corridor width seems to be even. Exiting program.\n";
        return -1;
    }

    auto entrance_size = corridor_width + 1;

    // "cut open" the entrance and exit to the maze
    in_copy.draw_rect({255}, 0, 0, entrance_size, entrance_size,
        vips::VImage::option()->set("fill", true));
    in_copy.draw_rect({255}, in_copy.width() - entrance_size, in_copy.height() - entrance_size, entrance_size, entrance_size,
        vips::VImage::option()->set("fill", true));

    // mark the top-left and bottom-right wall sections in different colours
    in_copy.draw_flood({255, 255, 0}, entrance_size, 0,
        vips::VImage::option()->set("equal", true));
    in_copy.draw_flood({0, 255, 255}, 0, entrance_size,
        vips::VImage::option()->set("equal", true));

    // invert to get no energy in corridors and single channel energy on walls
    auto inverted = in_copy.invert();

    // convolve with brute-ish kernel to drown out all corridors internal to the
    // maze segments
    auto kernel = vips::VImage::new_matrix(corridor_width + 2, 1);
    kernel.draw_rect({255}, 0, 0, kernel.width(), kernel.height());
    auto convolved = inverted.convsep(kernel);

    // where red and blue overlap is (almost) the path we're looking for
    auto intersection = convolved[0] * convolved[2];
    // colour the path cyan (i.e. "un-red")
    auto cyan_path = intersection.linear({0, 255, 255}, 0);
    // subtract cyan path from the white corridor pixels to get a red trace
    auto solved = input - cyan_path;

    // done
    solved.write_to_file(argv[2]);

    vips_shutdown();

    return 0;
}