#include <iostream>
#include <mutex>
#include <thread>

class Data {
public:
    int value;
    std::mutex mtx;

    Data(int v) : value(v) {}
};

// --- Вариант 1: std::lock ---
// Используется для одновременного захвата нескольких мьютексов без риска дедлока.
void swap_lock(Data& a, Data& b) {
    if (&a == &b) return; // Защита от самоблокировки

    // std::lock блокирует оба мьютекса атомарно относительно других блокировок
    std::lock(a.mtx, b.mtx);

    // adopt_lock указывает, что мьютексы уже заблокированы и их нужно просто взять под контроль
    std::lock_guard<std::mutex> lockA(a.mtx, std::adopt_lock);
    std::lock_guard<std::mutex> lockB(b.mtx, std::adopt_lock);

    std::swap(a.value, b.value);
    std::cout << "Swap via std::lock done\n";
}

// --- Вариант 2: std::scoped_lock (C++17) ---
// Самый современный и рекомендуемый способ. 
// Автоматически захватывает произвольное количество мьютексов и освобождает их.
void swap_scoped_lock(Data& a, Data& b) {
    if (&a == &b) return;

    // Автоматически использует алгоритм предотвращения дедлоков
    std::scoped_lock lock(a.mtx, b.mtx);

    std::swap(a.value, b.value);
    std::cout << "Swap via std::scoped_lock done\n";
}

// --- Вариант 3: std::unique_lock ---
// Гибкий способ, позволяет отложить блокировку (defer_lock).
void swap_unique_lock(Data& a, Data& b) {
    if (&a == &b) return;

    std::unique_lock<std::mutex> lockA(a.mtx, std::defer_lock);
    std::unique_lock<std::mutex> lockB(b.mtx, std::defer_lock);

    // Блокируем оба сразу через std::lock
    std::lock(lockA, lockB);

    std::swap(a.value, b.value);
    std::cout << "Swap via std::unique_lock done\n";
}

int main() {
    Data obj1(10), obj2(20);

    std::cout << "Before: " << obj1.value << " " << obj2.value << "\n";

    swap_lock(obj1, obj2);
    swap_scoped_lock(obj1, obj2);
    swap_unique_lock(obj1, obj2);

    std::cout << "After (3 swaps): " << obj1.value << " " << obj2.value << "\n";

    return 0;
}
