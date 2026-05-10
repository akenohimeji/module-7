#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <random>
#include <stdexcept>

std::mutex console_mtx;

void draw_progress_bar(int thread_num, std::thread::id tid, int length) {
    auto start_time = std::chrono::high_resolution_clock::now();

    // Генератор случайных чисел для ошибок (вероятность 15%)
    std::mt19937 gen(static_cast<unsigned int>(std::hash<std::thread::id>{}(tid)));
    std::uniform_int_distribution<> dis(1, 100);

    {
        std::lock_guard<std::mutex> lock(console_mtx);
        std::cout << "\033[" << thread_num + 1 << ";1H"
            << thread_num << " [" << tid << "]: ";
    }

    for (int i = 0; i < length; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(150 + (dis(gen) % 100)));

        std::string segment;
        try {
            if (dis(gen) <= 15) { // 15% шанс ошибки
                throw std::runtime_error("Error");
            }
            segment = "\033[32m█\033[0m"; // Зеленый блок
        }
        catch (const std::exception& e) {
            segment = "\033[31m█\033[0m"; // Красный блок при ошибке
        }

        {
            std::lock_guard<std::mutex> lock(console_mtx);
            // Позиционируемся: 25 - начало бара + текущий шаг i
            std::cout << "\033[" << thread_num + 1 << ";" << 25 + i << "H"
                << segment << std::flush;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    {
        std::lock_guard<std::mutex> lock(console_mtx);
        std::cout << "\033[" << thread_num + 1 << ";" << 25 + length + 2 << "H"
            << std::fixed << std::setprecision(2) << elapsed.count() << "s";
    }
}

int main() {
    const int num_threads = 5;
    const int bar_length = 20;

    std::cout << "\033[2J" << std::flush; // Очистка экрана

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(draw_progress_bar, i, std::this_thread::get_id(), bar_length);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\033[" << num_threads + 2 << ";1H" << "Готово! (Красный = ошибка)" << std::endl;

    return 0;
}
