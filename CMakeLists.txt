cmake_minimum_required(VERSION 3.16)

project(MiniRenderer LANGUAGES C CXX)

add_executable(minirenderer
	src/main.cpp
	src/engine.cpp
	src/he_model.cpp
	src/model.cpp
	src/tgaimage.cpp
)

target_compile_features(minirenderer PRIVATE cxx_std_17)

target_include_directories(minirenderer PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/include
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(minirenderer PRIVATE -Wall -Wextra -Wpedantic)
endif()
