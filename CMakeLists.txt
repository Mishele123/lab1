cmake_minimum_required(VERSION 3.10)
project(MatrixMultiplyMPI CXX)


find_package(MPI REQUIRED)


add_executable(lab3 main.cc)

target_include_directories(lab3 PRIVATE ${MPI_CXX_INCLUDE_DIRS})
target_link_libraries(lab3 PRIVATE ${MPI_CXX_LIBRARIES})

target_compile_options(lab3 PRIVATE ${MPI_CXX_COMPILE_OPTIONS})