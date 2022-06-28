cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option (BUILD_SHARED_LIBS "Build shared libraries" ON)
option(APP_BUILD_SAMPLES "build samples" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#project name
set(PROJECT_NAME cpplogger)

# app directories
set(APP_BOOST_DIR $ENV{BOOST_ROOT})
set(APP_SAMPLES_DIR ./samples/)
set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

message(STATUS ${APP_BOOST_DIR})

# source files
file(GLOB_RECURSE APP_SOURCE_FILES ${APP_SOURCE_DIR}/*.cpp)

# cpplogger project
project(${PROJECT_NAME} VERSION 1.0)
add_library(${PROJECT_NAME} SHARED ${APP_SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE CPPLOGGER_EXPORTS)
target_include_directories(${PROJECT_NAME} PRIVATE ${APP_BOOST_DIR})
target_link_directories(${PROJECT_NAME} PRIVATE ${APP_BOOST_DIR}/stage/lib/)

# samples
if(APP_BUILD_SAMPLES)
    project(sample_1 VERSION 1.0)
    add_executable(sample_1 ${APP_SAMPLES_DIR}/sample_1.cpp)
    target_include_directories(sample_1 PRIVATE ./src/)
    target_link_directories(sample_1 PRIVATE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/)
endif(APP_BUILD_SAMPLES)