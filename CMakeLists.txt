cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

project(sketch)

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp)

add_custom_target(clangformat
COMMAND
	/usr/bin/clang-format-3.8 -style=file -i ${ALL_SOURCE_FILES}
)

add_definitions(
-std=gnu++1z
-Wall
-Werror
-Wextra
-pedantic
-Wconversion
-Winit-self
-Woverloaded-virtual
-Wunreachable-code
-Wold-style-cast
-Wsign-promo
-Wshadow
)

find_package(Boost 1.65 REQUIRED system)

add_executable(sketch
	src/sketch.cpp
	src/sketch_grammar.cpp
	src/sketch_grammar.hpp
	src/window.hpp)

target_include_directories(sketch
PRIVATE
	${Boost_INCLUDE_DIRS})

target_link_libraries(sketch
PRIVATE
	${Boost_LIBRARIES})
