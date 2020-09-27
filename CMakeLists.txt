cmake_minimum_required(VERSION 3.12)
project(solution)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(SpamDetector SpamDetector.cpp HashMap.hpp)
target_link_libraries(SpamDetector ${Boost_LIBRARIES})