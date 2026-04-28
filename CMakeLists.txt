cmake_minimum_required(VERSION 3.20)

project(BULLSHIT)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/get_cpm.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/.output)

file(GLOB SOURCES
	src/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

CPMAddPackage(
	NAME SFML
	GITHUB_REPOSITORY SFML/SFML
	GIT_TAG 3.1.0
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ENABLE_OPENMP OFF CACHE BOOL "" FORCE)
set(ENABLE_SAMPLES OFF CACHE BOOL "" FORCE)
CPMAddPackage(
    NAME muparser
    GITHUB_REPOSITORY beltoforion/muparser
    GIT_TAG v2.3.5
)
target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
target_compile_definitions(muparser PRIVATE NOMINMAX)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)
target_link_libraries(${PROJECT_NAME} PRIVATE muparser)