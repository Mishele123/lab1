cmake_minimum_required(VERSION 3.18)
project(MatrixMultiplicationCUDA LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_language(CUDA)

add_executable(matrix_multiply_cuda main.cu)


set_target_properties(matrix_multiply_cuda PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
)


set_target_properties(matrix_multiply_cuda PROPERTIES CUDA_STANDARD 14 CUDA_STANDARD_REQUIRED ON)
