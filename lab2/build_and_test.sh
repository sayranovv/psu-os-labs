#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}=== СБОРКА ПРОЕКТОВ ===${NC}"

MATRIX_SIZE=${1:-1000}
echo "Размер матрицы: $MATRIX_SIZE"
echo ""

echo -e "${YELLOW}Компиляция matrix_threads...${NC}"

g++ -o matrix_threads matrix_threads.cpp -std=c++11 -pthread -O2

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ matrix_threads успешно скомпилирован${NC}"
else
    echo -e "${RED}✗ Ошибка при компиляции matrix_threads${NC}"
    exit 1
fi

echo ""

echo -e "${YELLOW}Компиляция matrix_processes...${NC}"

g++ -o matrix_processes matrix_processes.cpp -std=c++11 -O2

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ matrix_processes успешно скомпилирован${NC}"
else
    echo -e "${RED}✗ Ошибка при компиляции matrix_processes${NC}"
    exit 1
fi

echo ""
echo -e "${YELLOW}=== ЗАПУСК ТЕСТОВ ===${NC}"
echo ""

> threads_results.csv
> processes_results.csv

echo "num_threads,matrix_size,average_time_ms" > threads_results.csv
echo "num_processes,matrix_size,average_time_ms" > processes_results.csv

echo -e "${YELLOW}Тестирование потоков...${NC}"

NUM_CORES=$(getconf _NPROCESSORS_ONLN)
echo "Количество ядер процессора: $NUM_CORES"
echo ""

MAX_THREADS=$((NUM_CORES * 2))

for threads in 1 2 4 8 16; do
    if [ $threads -gt $MAX_THREADS ]; then
        break
    fi
    
    echo "Тестирование с $threads потоком(и)..."

    RESULT=$(./matrix_threads $MATRIX_SIZE $threads | grep "CSV:" | awk '{print $NF}')
    
    echo "$threads,$MATRIX_SIZE,$RESULT" >> threads_results.csv
    
    echo "  Результат: $RESULT мс"
done

echo ""

echo -e "${YELLOW}Тестирование процессов...${NC}"

for processes in 1 2 4 8 16; do
    if [ $processes -gt $MAX_THREADS ]; then
        break
    fi
    
    echo "Тестирование с $processes процесс(ом/ами)..."
    
    RESULT=$(./matrix_processes $MATRIX_SIZE $processes | grep "CSV:" | awk '{print $NF}')
    
    echo "$processes,$MATRIX_SIZE,$RESULT" >> processes_results.csv
    
    echo "  Результат: $RESULT мс"
done

echo ""
echo -e "${GREEN}=== ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ===${NC}"
echo ""
echo "Результаты сохранены в:"
echo "  - threads_results.csv"
echo "  - processes_results.csv"
echo ""
