cmake_minimum_required(VERSION 3.16)
project(walking-g1)


set(CMAKE_BUILD_TYPE Debug)

include_directories(/usr/local/include/ddscxx)
include_directories(/usr/local/include/iceoryx/v2.0.2)

set(CMAKE_CXX_STANDARD 17)
set(Torch_DIR /path/to/pytorch/torch/share/cmake/Torch)

find_package(Torch REQUIRED)


find_package(Torch REQUIRED)
include_directories(${TORCH_INCLUDE_DIRS})

list(APPEND CMAKE_PREFIX_PATH "/opt/unitree_robotics/lib/cmake") 
find_package(unitree_sdk2 REQUIRED)



aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

find_package(yaml-cpp REQUIRED)
target_link_libraries(${PROJECT_NAME} unitree_sdk2 ddsc ddscxx ${TORCH_LIBRARIES} yaml-cpp::yaml-cpp pthread)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
