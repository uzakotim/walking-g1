cmake_minimum_required(VERSION 3.16)
project(walking-g1)


set(CMAKE_BUILD_TYPE Debug)

include_directories(/usr/local/include/ddscxx)
include_directories(/usr/local/include/iceoryx/v2.0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_RPATH "$ENV{CONDA_PREFIX}/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

list(APPEND CMAKE_PREFIX_PATH "$ENV{CONDA_PREFIX}")

find_package(Torch REQUIRED)

include_directories(${TORCH_INCLUDE_DIRS})

list(APPEND CMAKE_PREFIX_PATH "/opt/unitree_robotics/lib/cmake") 
find_package(unitree_sdk2 REQUIRED)



aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

find_package(yaml-cpp REQUIRED)
target_link_libraries(${PROJECT_NAME} unitree_sdk2 ddsc ddscxx ${TORCH_LIBRARIES} yaml-cpp::yaml-cpp pthread)
