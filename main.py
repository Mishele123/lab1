import numpy as np
import time

def read_matrix_from_file(filename):
    """Считывает матрицу из файла"""
    with open(filename, 'r') as f:
        matrix = []
        for line in f:
            row = list(map(float, line.split()))
            matrix.append(row)
    return np.array(matrix)

def write_matrix_to_file(matrix, filename):
    """Записывает матрицу в файл"""
    with open(filename, 'w') as f:
        for row in matrix:
            f.write(' '.join(map(str, row)) + '\n')

def multiply_matrices(A, B):
    """Перемножает две матрицы"""
    return np.dot(A, B)

def main():
    A = read_matrix_from_file('matrix1.txt')
    B = read_matrix_from_file('matrix2.txt')
    
    start_time = time.time()

    result = multiply_matrices(A, B)

    execution_time = time.time() - start_time

    write_matrix_to_file(result, 'result_matrix.txt')

    print(f"Время выполнения: {execution_time:.6f} секунд")
    print(f"Размерность матрицы A: {A.shape}")
    print(f"Размерность матрицы B: {B.shape}")
    print(f"Размерность результирующей матрицы: {result.shape}")

    expected_result = np.loadtxt('result.txt')
    assert np.allclose(result, expected_result), "Результаты не совпадают!"

if __name__ == "__main__":
    main()