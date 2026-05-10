#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

std::atomic<int> client_counter(0);

// Варианты упорядочения: 
// std::memory_order_relaxed
// std::memory_order_acquire / release
// std::memory_order_seq_cst (по умолчанию)

void client_stream(int max_clients, std::memory_order order) {
    for (int i = 0; i < max_clients; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Атомарное прибавление с заданным порядком
        client_counter.fetch_add(1, order);

        // Чтение для вывода тоже должно быть атомарным
        std::cout << "[Клиент] Пришел. Всего: " << client_counter.load(order) << std::endl;
    }
}

void operator_stream(int max_clients, std::memory_order load_order, std::memory_order modify_order) {
    int served_count = 0;
    while (served_count < max_clients) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Сначала загружаем текущее значение
        if (client_counter.load(load_order) > 0) {
            client_counter.fetch_sub(1, modify_order);
            served_count++;
            std::cout << "[Оператор] Обслужил. Осталось: " << client_counter.load(load_order) << std::endl;
        }
        else {
            std::cout << "[Оператор] Ждет..." << std::endl;
        }
    }
}

void run_simulation(std::string name, std::memory_order order) {
    client_counter.store(0); // сброс
    std::cout << "\n>>> Тест режима: " << name << " <<<\n";

    std::thread t1(client_stream, 5, order);
    std::thread t2(operator_stream, 5, order, order);

    t1.join();
    t2.join();
}

int main() {
    setlocale(LC_ALL, "Russian");

    // 1. Самый строгий режим (Sequence Consistent)
    run_simulation("memory_order_seq_cst", std::memory_order_seq_cst);

    // 2. Ослабленный режим (Relaxed)
    // В данной задаче он тоже сработает, так как нет зависимости по данным между потоками
    run_simulation("memory_order_relaxed", std::memory_order_relaxed);

    return 0;
}
