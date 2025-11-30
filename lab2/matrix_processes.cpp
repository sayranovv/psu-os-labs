#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <chrono>
#include <random>
#include <iomanip>

typedef std::vector<std::vector<int>> Matrix;


int GLOBAL_MATRIX_SIZE = 0;

Matrix GLOBAL_MATRIX_A;
Matrix GLOBAL_MATRIX_B;

Matrix GLOBAL_RESULT_MATRIX;

Matrix generateRandomMatrix(int size) {
    Matrix matrix(size, std::vector<int>(size));
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = dis(gen);
        }
    }
    
    return matrix;
}

void computeMatrixRow(const Matrix& A, const Matrix& B, Matrix& result, int row, int size) {
    for (int j = 0; j < size; j++) {
        int sum = 0;
        for (int k = 0; k < size; k++) {
            sum += A[row][k] * B[k][j];
        }
        result[row][j] = sum;
    }
}

void childProcessWorker(int startRow, int endRow, int fd_write) {
    for (int i = startRow; i < endRow; i++) {
        computeMatrixRow(GLOBAL_MATRIX_A, GLOBAL_MATRIX_B, GLOBAL_RESULT_MATRIX, i, GLOBAL_MATRIX_SIZE);
    }
    
    int data[2] = {startRow, endRow};
    ssize_t written = write(fd_write, data, sizeof(data));
    if (written < 0) {
        std::cerr << "Ошибка при записи в pipe" << std::endl;
        exit(1);
    }
    
    for (int i = startRow; i < endRow; i++) {
        ssize_t result = write(fd_write, GLOBAL_RESULT_MATRIX[i].data(), GLOBAL_MATRIX_SIZE * sizeof(int));
        if (result < 0) {
            std::cerr << "Ошибка при записи данных" << std::endl;
            exit(1);
        }
    }
    
    close(fd_write);
    
    exit(0);
}

double multiplyMatricesWithProcesses(const Matrix& A, const Matrix& B, Matrix& result, int numProcesses) {
    int matrixSize = A.size();
    
    GLOBAL_MATRIX_SIZE = matrixSize;
    GLOBAL_MATRIX_A = A;
    GLOBAL_MATRIX_B = B;
    GLOBAL_RESULT_MATRIX = result;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<int> pipes_data[numProcesses];
    
    for (int i = 0; i < numProcesses; i++) {
        int fd[2];
        if (pipe(fd) < 0) {
            std::cerr << "Ошибка при создании pipe" << std::endl;
            return -1;
        }
        pipes_data[i].push_back(fd[0]);
        pipes_data[i].push_back(fd[1]);
    }

    std::vector<pid_t> childPIDs;
    
    int rowsPerProcess = (matrixSize + numProcesses - 1) / numProcesses;
    
    for (int p = 0; p < numProcesses; p++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            std::cerr << "Ошибка при создании процесса" << std::endl;
            return -1;
        } 
        else if (pid == 0) {
            close(pipes_data[p][0]);
            
            for (int i = 0; i < numProcesses; i++) {
                if (i != p) {
                    close(pipes_data[i][0]);
                    close(pipes_data[i][1]);
                }
            }
            
            int startRow = p * rowsPerProcess;
            int endRow = std::min((p + 1) * rowsPerProcess, matrixSize);
            
            childProcessWorker(startRow, endRow, pipes_data[p][1]);
        }
        else {
            close(pipes_data[p][1]);
            childPIDs.push_back(pid);
        }
    }
    
    for (int p = 0; p < numProcesses; p++) {
        int bounds[2];
        ssize_t read_bytes = read(pipes_data[p][0], bounds, sizeof(bounds));
        
        if (read_bytes < 0) {
            std::cerr << "Ошибка при чтении из pipe" << std::endl;
            return -1;
        }
        
        int startRow = bounds[0];
        int endRow = bounds[1];
        
        for (int i = startRow; i < endRow; i++) {
            ssize_t result_bytes = read(pipes_data[p][0], result[i].data(), matrixSize * sizeof(int));
            if (result_bytes < 0) {
                std::cerr << "Ошибка при чтении результатов" << std::endl;
                return -1;
            }
        }
        
        close(pipes_data[p][0]);
    }
    
    for (int p = 0; p < numProcesses; p++) {
        int status;
        pid_t finished_pid = wait(&status);
        
        if (finished_pid < 0) {
            std::cerr << "Ошибка при ожидании процесса" << std::endl;
            return -1;
        }

        if (!WIFEXITED(status)) {
            std::cerr << "Процесс " << finished_pid << " завершился с ошибкой" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return duration.count();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Использование: " << argv[0] << " <размер матрицы> <количество процессов>" << std::endl;
        std::cerr << "Пример: " << argv[0] << " 1000 4" << std::endl;
        return 1;
    }
    
    int matrixSize = std::atoi(argv[1]);
    int numProcesses = std::atoi(argv[2]);
    
    if (matrixSize <= 0 || numProcesses <= 0) {
        std::cerr << "Размер матрицы и количество процессов должны быть положительными!" << std::endl;
        return 1;
    }
    
    std::cout << "=== УМНОЖЕНИЕ МАТРИЦ ЧЕРЕЗ ПРОЦЕССЫ ===" << std::endl;
    std::cout << "Размер матрицы: " << matrixSize << " x " << matrixSize << std::endl;
    std::cout << "Количество процессов: " << numProcesses << std::endl;
    std::cout << std::endl;
    
    const int NUM_RUNS = 5;
    std::vector<double> executionTimes;
    
    for (int run = 0; run < NUM_RUNS; run++) {
        std::cout << "Запуск " << (run + 1) << "... ";
        std::cout.flush();
        
        Matrix A = generateRandomMatrix(matrixSize);
        Matrix B = generateRandomMatrix(matrixSize);
        Matrix result(matrixSize, std::vector<int>(matrixSize, 0));
        
        double executionTime = multiplyMatricesWithProcesses(A, B, result, numProcesses);
        executionTimes.push_back(executionTime);
        
        std::cout << executionTime << " мс" << std::endl;
    }
    
    double totalTime = 0;
    for (double time : executionTimes) {
        totalTime += time;
    }
    double averageTime = totalTime / executionTimes.size();
    
    std::cout << std::endl;
    std::cout << "Результаты:" << std::endl;
    std::cout << "Среднее время выполнения: " << std::fixed << std::setprecision(2) 
              << averageTime << " мс" << std::endl;
    
    std::cout << "CSV: " << numProcesses << "," << matrixSize << "," << averageTime << std::endl;
    
    return 0;
}
