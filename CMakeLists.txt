cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

project(sketch VERSION 0.0.1 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CMAKE_USE_RELATIVE_PATHS TRUE)
set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}-${PROJECT_VERSION}")
set(INSTALL_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake")
set(INSTALL_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake")

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

find_package(Boost 1.65 REQUIRED)

set(SKETCH_HEADERS
public/sketch.hpp
public/sketch/window.hpp)

set(SKETCH_SOURCES
src/sketch.cpp)

set(ALL_SOURCE_FILES ${SKETCH_HEADERS} ${SKETCH_SOURCES})

foreach(SOURCE_FILE ${ALL_SOURCE_FILES})
	set(ALL_ABS_SOURCE_FILES ${ALL_ABS_SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE})
endforeach()

# setting up a format command
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
	add_custom_target(
		format
	COMMAND
		clang-format -style=file -i ${ALL_ABS_SOURCE_FILES}
)
endif()

# setting up a lint command
find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
	add_custom_target(
		lint
	COMMAND
		clang-tidy
		${ALL_ABS_SOURCE_FILES}
		--
		-std=gnu++1z)
endif()

# lib objects
add_library(sketch_objs OBJECT ${ALL_SOURCE_FILES})

target_include_directories(sketch_objs
PRIVATE
	public
	${Boost_INCLUDE_DIRS}
)

set_property(TARGET sketch_objs PROPERTY COMPILE_FLAGS "-fPIC")

add_library(sketch_static STATIC $<TARGET_OBJECTS:sketch_objs>)

set_target_properties(sketch_static PROPERTIES OUTPUT_NAME "sketch")
set_target_properties(sketch_static PROPERTIES VERSION "${PROJECT_VERSION}")
set_target_properties(sketch_static PROPERTIES SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

target_include_directories(sketch_static
INTERFACE
	$<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
	$<BUILD_INTERAFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>)

target_link_libraries(sketch_static
PRIVATE
	${Boost_LIBRARIES})

add_library(sketch SHARED $<TARGET_OBJECTS:sketch_objs>)

set_target_properties(sketch PROPERTIES VERSION "${PROJECT_VERSION}")
set_target_properties(sketch PROPERTIES SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

target_include_directories(sketch
INTERFACE
	$<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
	$<BUILD_INTERAFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>)

target_link_libraries(sketch
PUBLIC
	${Boost_LIBRARIES})

write_basic_package_version_file(
	"${INSTALL_VERSION_FILE}"
VERSION
	${PROJECT_VERSION}
COMPATIBILITY
	SameMajorVersion
)

configure_package_config_file("cmake/${PROJECT_NAME}-config.cmake.in"
	"${INSTALL_CONFIG_FILE}"
INSTALL_DESTINATION
	"${CMAKE_INSTALL_LIBDIR}"
)

install(TARGETS sketch
EXPORT
	"${PROJECT_NAME}"
LIBRARY DESTINATION
	"${CMAKE_INSTALL_LIBDIR}"
COMPONENT
	Runtime
)

install(TARGETS sketch_static
EXPORT
	"${PROJECT_NAME}"
ARCHIVE DESTINATION
	"${CMAKE_INSTALL_LIBDIR}"
COMPONENT
	Devel
)

install(DIRECTORY "public"
DESTINATION
	"${INSTALL_INCLUDE_DIR}"
COMPONENT
	Devel
PATTERN *.hpp
)

INSTALL(FILES
	"${INSTALL_VERSION_FILE}"
	"${INSTALL_CONFIG_FILE}"
DESTINATION
	"${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}"
COMPONENT
	Devel
)

INSTALL(EXPORT "${PROJECT_NAME}"
FILE
	${PROJECT_NAME}-targets.cmake
DESTINATION
	"${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}"
COMPONENT Devel
)
