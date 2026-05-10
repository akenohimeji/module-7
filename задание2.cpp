#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <iomanip>

std::mutex console_mtx; // Для предотвращения перемешивания вывода

void draw_progress_bar(int thread_num, std::thread::id tid, int length) {
    auto start_time = std::chrono::high_resolution_clock::now();

    // Подготовка строки: номер и ID
    {
        std::lock_guard<std::mutex> lock(console_mtx);
        std::cout << "\033[" << thread_num + 1 << ";1H"; // Переход на строку потока
        std::cout << thread_num << " [" << tid << "]: ";
    }

    for (int i = 0; i < length; ++i) {
        // Имитация "расчета"
        std::this_thread::sleep_for(std::chrono::milliseconds(100 + (rand() % 100)));

        {
            std::lock_guard<std::mutex> lock(console_mtx);
            std::cout << "\033[" << thread_num + 1 << ";25H"; // Позиция начала бара
            for (int j = 0; j <= i; ++j) std::cout << "█";
            std::cout << std::flush;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // Вывод времени в конце строки
    {
        std::lock_guard<std::mutex> lock(console_mtx);
        std::cout << "  " << std::fixed << std::setprecision(2) << elapsed.count() << "s" << std::endl;
    }
}

int main() {
    const int num_threads = 5;  // Количество потоков
    const int bar_length = 20;  // Длина прогресс-бара

    // Очистка экрана
    std::cout << "\033[2J" << std::flush;

    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        // Создаем поток. Передаем i как порядковый номер.
        threads.emplace_back([i, bar_length]() {
            draw_progress_bar(i, std::this_thread::get_id(), bar_length);
            });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Перемещаем курсор вниз, чтобы текст после программы не затирал бары
    std::cout << "\033[" << num_threads + 2 << ";1H" << "Расчеты завершены." << std::endl;

    return 0;
}
