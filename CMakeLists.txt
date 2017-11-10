cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

project(sketch)

file(GLOB_RECURSE ALL_SOURCE_FILES src/*.cpp src/*.hpp)

find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
	add_custom_target(
		format
	COMMAND
		clang-format -style=file -i ${ALL_SOURCE_FILES}
)
endif()

find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
	add_custom_target(
		lint
	COMMAND
		clang-tidy
		${ALL_SOURCE_FILES}
		--
		-std=gnu++1z)
endif()

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
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON
)

find_package(Boost 1.65 REQUIRED)

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
