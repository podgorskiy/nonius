cmake_minimum_required(VERSION 3.0)

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

project(nonius LANGUAGES CXX)

option(NONIUS_EXAMPLES "Build nonius examples" OFF)
option(NONIUS_TESTS "Build nonius tests" OFF)

add_library(${PROJECT_NAME} INTERFACE)

if(NOT CMAKE_VERSION VERSION_LESS 3.8)
    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)
endif()

target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

if (NONIUS_TESTS)
    file(GLOB NONIUS_TESTS_SOURCES test/*.c++)
    add_executable(nonius_tests ${NONIUS_TESTS_SOURCES})
    target_link_libraries(nonius_tests nonius ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(nonius_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/catch/include)
    add_test(NAMEnonius_tests COMMAND $<TARGET_FILE:nonius_tests>)
endif()

if (NONIUS_EXAMPLES)
    file(GLOB NONIUS_EXAMPLE_SOURCES examples/*.c++)
    foreach(src ${NONIUS_EXAMPLE_SOURCES})
        get_filename_component(name ${src} NAME_WE)
        add_executable(nonius_${name} ${src})
        target_include_directories(nonius_${name} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/nonius/>)
        target_link_libraries(nonius_${name} nonius pthread ${CMAKE_THREAD_LIBS_INIT})
        add_test(NAME nonius_${name} COMMAND $<TARGET_FILE:${name}>)
    endforeach()
endif()
