#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <iomanip>

typedef std::vector<std::vector<int>> Matrix;

struct ThreadData {
    const Matrix* matrixA;
    const Matrix* matrixB;
    Matrix* resultMatrix;
    
    int startRow;
    int endRow;
    int matrixSize;
    
    ThreadData(const Matrix* A, const Matrix* B, Matrix* result, int start, int end, int size)
        : matrixA(A), matrixB(B), resultMatrix(result), startRow(start), endRow(end), matrixSize(size) {}
};

void multiplyMatrixRows(ThreadData* data) {
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = 0; j < data->matrixSize; j++) {
            int sum = 0;
            for (int k = 0; k < data->matrixSize; k++) {
                sum += (*data->matrixA)[i][k] * (*data->matrixB)[k][j];
            }
            (*data->resultMatrix)[i][j] = sum;
        }
    }
}

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

double multiplyMatricesWithThreads(const Matrix& matrixA, const Matrix& matrixB, Matrix& result, int numThreads) {
   int matrixSize = matrixA.size();
    
    auto startTime = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;

    int rowsPerThread = (matrixSize + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; t++) {
        int startRow = t * rowsPerThread;
        int endRow = std::min((t + 1) * rowsPerThread, matrixSize);

        ThreadData* data = new ThreadData(&matrixA, &matrixB, &result, startRow, endRow, matrixSize);

        threads.push_back(std::thread(multiplyMatrixRows, data));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return duration.count();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Использование: " << argv[0] << " <размер матрицы> <количество потоков>" << std::endl;
        std::cerr << "Пример: " << argv[0] << " 1000 4" << std::endl;
        return 1;
    }

    int matrixSize = std::atoi(argv[1]);
    int numThreads = std::atoi(argv[2]);
    
    if (matrixSize <= 0 || numThreads <= 0) {
        std::cerr << "Размер матрицы и количество потоков должны быть положительными!" << std::endl;
        return 1;
    }
    
    std::cout << "=== УМНОЖЕНИЕ МАТРИЦ ЧЕРЕЗ ПОТОКИ ===" << std::endl;
    std::cout << "Размер матрицы: " << matrixSize << " x " << matrixSize << std::endl;
    std::cout << "Количество потоков: " << numThreads << std::endl;
    std::cout << std::endl;

    const int NUM_RUNS = 5;
    std::vector<double> executionTimes;
    
    for (int run = 0; run < NUM_RUNS; run++) {
        std::cout << "Запуск " << (run + 1) << "... ";
        std::cout.flush();
        
        Matrix A = generateRandomMatrix(matrixSize);
        Matrix B = generateRandomMatrix(matrixSize);
        Matrix result(matrixSize, std::vector<int>(matrixSize, 0));
        
        double executionTime = multiplyMatricesWithThreads(A, B, result, numThreads);
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
    
    std::cout << "CSV: " << numThreads << "," << matrixSize << "," << averageTime << std::endl;
    
    return 0;
}
