cmake_minimum_required (VERSION 3.11)

project (Orbit)

find_package(OpenGL REQUIRED)

add_definitions(-std=c++17)

set(IMGUI_ROOT "${PROJECT_SOURCE_DIR}/deps/imgui" CACHE PATH "imgui top-level directory")
option(IMGUI_SFML_BUILD_EXAMPLES "Build ImGui_SFML examples" OFF)
add_subdirectory(deps/imgui-sfml)

get_directory_property(IMGUI_SOURCES DIRECTORY deps/imgui-sfml DEFINITION IMGUI_SOURCES)
get_directory_property(IMGUI_SFML_SOURCES DIRECTORY deps/imgui-sfml DEFINITION IMGUI_SFML_SOURCES)
get_directory_property(IMGUI_SFML_INCLUDE_DIRS DIRECTORY deps/imgui-sfml DEFINITION IMGUI_SFML_INCLUDE_DIRS)
get_directory_property(IMGUI_SFML_DEPENDENCIES DIRECTORY deps/imgui-sfml DEFINITION IMGUI_SFML_DEPENDENCIES)

add_executable(orbit
	src/main.cpp

	src/common/utils.cpp
	src/common/computations.cpp

	src/simulation/universe.cpp

	src/graphics/fonts.cpp
	src/graphics/transform.cpp
	src/graphics/shapes_routines.cpp
	src/graphics/misc_draw.cpp
	src/graphics/object.cpp

	src/scenes/scene_manager.cpp
	src/scenes/universe.cpp
	src/scenes/utils.cpp

	${IMGUI_SOURCES} ${IMGUI_SFML_SOURCES}
)

option(USE_IWYU "Whether to apply include-what-you-use" OFF)

if (USE_IWYU)
	set(IWYU_PATH "iwyu")
	set(IWYU_PATH_AND_OPTIONS
		${IWYU_PATH}
		-Xiwyu
		--mapping_file=${PROJECT_SOURCE_DIR}/iwyu-mapping.imp
	)
else()
	set(IWYU_PATH_AND_OPTIONS "")
endif()

set_property(TARGET orbit PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH_AND_OPTIONS})


target_compile_definitions(orbit PRIVATE -DIMGUI_DISABLE_INCLUDE_IMCONFIG_H)
target_compile_definitions(orbit PRIVATE -DIMGUI_USER_CONFIG="custom-imconfig.h")
target_include_directories(orbit PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${IMGUI_ROOT}
	${IMGUI_SFML_INCLUDE_DIRS}
	# deps/imgui-sfml
)

target_link_libraries(orbit
	sfml-graphics
	sfml-window
	sfml-system

	${OPENGL_LIBRARIES}
	${IMGUI_SFML_DEPENDENCIES}
)