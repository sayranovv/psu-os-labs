CXX = g++
CXXFLAGS = -std=c++11 -O2
THREAD_FLAGS = -pthread
MATRIX_SIZE ?= 1000

THREAD_EXEC = matrix_threads
PROCESS_EXEC = matrix_processes

THREAD_SRC = matrix_threads.cpp
PROCESS_SRC = matrix_processes.cpp

ANALYSIS_SCRIPT = analyze_results.py
VENV = .venv

.DEFAULT_GOAL := help

.PHONY: help compile test analyze clean all install-deps

help:
	@echo "╔═══════════════════════════════════════════════════════════════╗"
	@echo "║  Лабораторная работа №2: Параллельное умножение матриц        ║"
	@echo "║  Язык: C++, Потоки и Процессы                                 ║"
	@echo "╚═══════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Доступные команды:"
	@echo ""
	@echo "  make help               - показать эту справку"
	@echo "  make compile            - скомпилировать оба исполняемых файла"
	@echo "  make test               - запустить тесты (матрица 1000×1000)"
	@echo "  make test SIZE=800      - запустить тесты с матрицей 800×800"
	@echo "  make analyze            - построить графики анализа результатов"
	@echo "  make all                - compile + test + analyze (полный цикл)"
	@echo "  make clean              - удалить скомпилированные файлы"
	@echo "  make install-deps       - установить зависимости Python"
	@echo "  make run-threads NUM=4  - запустить потоки (матрица 1000, 4 потока)"
	@echo "  make run-processes NUM=4 - запустить процессы (матрица 1000, 4 процесса)"
	@echo ""
	@echo "Примеры:"
	@echo "  make compile                    # Скомпилировать"
	@echo "  make test                       # Тесты (1000×1000)"
	@echo "  make test SIZE=2000             # Тесты (2000×2000)"
	@echo "  make all                        # Полный цикл"
	@echo "  make run-threads NUM=8          # Потоки (8 потоков)"
	@echo "  make run-processes NUM=8        # Процессы (8 процессов)"
	@echo "  make clean                      # Очистить"
	@echo ""

compile: $(THREAD_EXEC) $(PROCESS_EXEC)
	@echo "✓ Компиляция завершена успешно"

$(THREAD_EXEC): $(THREAD_SRC)
	@echo "Компилирую $(THREAD_EXEC)..."
	@$(CXX) $(CXXFLAGS) $(THREAD_FLAGS) -o $@ $<
	@echo "✓ $(THREAD_EXEC) скомпилирован"

$(PROCESS_EXEC): $(PROCESS_SRC)
	@echo "Компилирую $(PROCESS_EXEC)..."
	@$(CXX) $(CXXFLAGS) -o $@ $<
	@echo "✓ $(PROCESS_EXEC) скомпилирован"

test: compile
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════════════╗"
	@echo "║  ЗАПУСК ТЕСТОВ - Размер матрицы: $(MATRIX_SIZE)×$(MATRIX_SIZE)    ║"
	@echo "╚═══════════════════════════════════════════════════════════════════╝"
	@echo ""
	@bash build_and_test.sh $(MATRIX_SIZE)


analyze: 
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════════╗"
	@echo "║  АНАЛИЗ РЕЗУЛЬТАТОВ И ПОСТРОЕНИЕ ГРАФИКОВ                     ║"
	@echo "╚═══════════════════════════════════════════════════════════════╝"
	@echo ""
	@$(VENV)/bin/python $(ANALYSIS_SCRIPT)

all: compile test analyze
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════════╗"
	@echo "║  ✓ ПОЛНЫЙ ЦИКЛ ЗАВЕРШЕН УСПЕШНО!                              ║"
	@echo "╚═══════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Результаты:"
	@echo "  • Графики: performance_analysis.png"
	@echo "  • Данные потоков: threads_results.csv"
	@echo "  • Данные процессов: processes_results.csv"
	@echo ""

run-threads: compile
	@echo ""
	@echo "Запуск: ./$(THREAD_EXEC) 1000 $(NUM)"
	@echo ""
	@./$(THREAD_EXEC) 1000 $(NUM)

run-processes: compile
	@echo ""
	@echo "Запуск: ./$(PROCESS_EXEC) 1000 $(NUM)"
	@echo ""
	@./$(PROCESS_EXEC) 1000 $(NUM)

clean:
	@echo "Очистка..."
	@rm -f $(THREAD_EXEC) $(PROCESS_EXEC)
	@echo "✓ Скомпилированные файлы удалены"

clean-results:
	@echo "Удаление результатов тестов..."
	@rm -f threads_results.csv processes_results.csv performance_analysis.png
	@echo "✓ Результаты удалены"

clean-all: clean clean-results
	@echo "✓ Всё очищено"

install-deps:
	@echo "Установка зависимостей Python..."
	@echo ""
	@echo "Для Linux (Ubuntu/Debian):"
	@echo "  sudo apt-get install python3-pip"
	@echo "  pip3 install pandas matplotlib"
	@echo ""
	@echo "Для macOS:"
	@echo "  brew install python3"
	@echo "  pip3 install pandas matplotlib"
	@echo ""
	@echo "Или для обоих:"
	pip3 install pandas matplotlib
	@echo "✓ Зависимости установлены"

info:
	@echo "╔═══════════════════════════════════════════════════════════════╗"
	@echo "║  ИНФОРМАЦИЯ О СИСТЕМЕ                                         ║"
	@echo "╚═══════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Количество ядер:"
	@getconf _NPROCESSORS_ONLN 2>/dev/null || nproc
	@echo ""
	@echo "Компилятор:"
	@$(CXX) --version | head -1
	@echo ""
	@echo "Python:"
	@python3 --version
	@echo ""

check-deps:
	@echo "Проверка зависимостей..."
	@echo -n "g++: "
	@which g++ > /dev/null && echo "✓" || echo "✗ НЕ НАЙДЕН"
	@echo -n "python3: "
	@which python3 > /dev/null && echo "✓" || echo "✗ НЕ НАЙДЕН"
	@echo -n "pandas: "
	@python3 -c "import pandas" 2>/dev/null && echo "✓" || echo "✗ НЕ УСТАНОВЛЕН"
	@echo -n "matplotlib: "
	@python3 -c "import matplotlib" 2>/dev/null && echo "✓" || echo "✗ НЕ УСТАНОВЛЕН"

quick-test: compile
	@echo ""
	@echo "Быстрый тест с матрицей 500×500..."
	@echo ""
	@bash build_and_test.sh 500

heavy-test: compile
	@echo ""
	@echo "Тест на больших матрицах 1500×1500..."
	@echo ""
	@bash build_and_test.sh 1500

profile-threads: compile
	@echo ""
	@echo "Профилирование потоков (требует perf)..."
	@perf record -g ./$(THREAD_EXEC) 1000 4
	@perf report

size:
	@echo "Размер исполняемых файлов:"
	@ls -lh $(THREAD_EXEC) $(PROCESS_EXEC) 2>/dev/null || echo "Файлы ещё не скомпилированы"

view:
	@echo "Исходные файлы:"
	@echo "  1. matrix_threads.cpp - версия с потоками"
	@echo "  2. matrix_processes.cpp - версия с процессами"
	@echo ""
	@echo "Просмотр matrix_threads.cpp:"
	@wc -l $(THREAD_SRC)
	@echo ""
	@echo "Просмотр matrix_processes.cpp:"
	@wc -l $(PROCESS_SRC)
