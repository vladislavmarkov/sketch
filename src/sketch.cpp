#include <cstdint>
#include <iostream>

#include "sketch_grammar.hpp"

int
main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Please, specify file to parse\n";
        std::exit(EXIT_FAILURE);
    }

    sk::create_sketch(argv[1]);

    return EXIT_SUCCESS;
}
