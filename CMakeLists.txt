cmake_minimum_required(VERSION 3.28)
project(dijkstra_2t C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pipe")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -march=native -flto -fomit-frame-pointer")

find_package(OpenMP REQUIRED)

add_executable(${PROJECT_NAME}
    src/adj_matrix.c
    src/get_data_from_file.c
    src/random_data.c
    src/priority_que.c
    src/uint32_arrays.c
    src/dijkstra_bi_1th.c
    src/dijkstra_bi_2th.c
    src/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_C)
