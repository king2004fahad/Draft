cmake_minimum_required(VERSION 3.14)
project(dataviz VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DATAVIZ_BUILD_TESTS "Build the tests" ON)
option(DATAVIZ_BUILD_EXAMPLES "Build the examples" ON)

# ---- the library -----------------------------------------------------------
add_library(dataviz
    src/DataLoader.cpp
    src/DataProcessor.cpp
    src/Statistics.cpp
    src/CountPlot.cpp
    src/Histogram.cpp
    src/KDEPlot.cpp
    src/Axis.cpp
    src/Annotations.cpp
    src/Renderer.cpp
    src/ImageExporter.cpp
)
target_include_directories(dataviz PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(dataviz PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(dataviz PRIVATE /W4)
else()
    target_compile_options(dataviz PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---- tests -----------------------------------------------------------------
if(DATAVIZ_BUILD_TESTS)
    enable_testing()
    foreach(name test_data test_plots test_rendering test_pipeline)
        add_executable(${name} tests/${name}.cpp)
        target_link_libraries(${name} PRIVATE dataviz)
        add_test(NAME ${name} COMMAND ${name})
    endforeach()
endif()

# ---- examples --------------------------------------------------------------
if(DATAVIZ_BUILD_EXAMPLES)
    add_executable(example_pipeline examples/example_pipeline.cpp)
    target_link_libraries(example_pipeline PRIVATE dataviz)
    target_compile_definitions(example_pipeline PRIVATE
        DATAVIZ_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
endif()