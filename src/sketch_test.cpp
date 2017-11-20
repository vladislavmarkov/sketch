#include <cstdlib>
#include <iostream>

#include <sketch.hpp>

int
main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "filename is required\n";
        return EXIT_FAILURE;
    }

    sk::application_t app;
    app.add(sk::load_sketch(argv[1]));
    return app.run();
}
