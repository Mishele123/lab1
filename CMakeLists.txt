cmake_minimum_required(VERSION 3.10)
project(MatrixMultiplyMPI CXX)


find_package(MPI REQUIRED)


add_executable(matrix_multiply_mpi main.cc)

target_include_directories(matrix_multiply_mpi PRIVATE ${MPI_CXX_INCLUDE_DIRS})
target_link_libraries(matrix_multiply_mpi PRIVATE ${MPI_CXX_LIBRARIES})

target_compile_options(matrix_multiply_mpi PRIVATE ${MPI_CXX_COMPILE_OPTIONS})