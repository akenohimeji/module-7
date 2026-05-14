#include <iostream>
#include <vector>
#include <future>
#include <algorithm>
#include <utility>

void find_min_index_async(const std::vector<int>& vec, size_t start_idx, std::promise<size_t> promise_obj) {
    size_t min_idx = start_idx;
    for (size_t i = start_idx + 1; i < vec.size(); ++i) {
        if (vec[i] < vec[min_idx]) {
            min_idx = i;
        }
    }
    promise_obj.set_value(min_idx);
}

void selection_sort(std::vector<int>& vec) {
    size_t n = vec.size();

    for (size_t i = 0; i < n - 1; ++i) {
        std::promise<size_t> min_idx_promise;
        std::future<size_t> min_idx_future = min_idx_promise.get_future();

        std::thread worker(find_min_index_async, std::cref(vec), i, std::move(min_idx_promise));

        size_t min_idx = min_idx_future.get();

        worker.join();

        if (min_idx != i) {
            std::swap(vec[i], vec[min_idx]);
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::vector<int> data = { 64, 25, 12, 22, 11, 90, 4, 75 };

    std::cout << "Исходный массив: ";
    for (int num : data) std::cout << num << " ";
    std::cout << "\n";

    selection_sort(data);

    std::cout << "Отсортированный массив: ";
    for (int num : data) std::cout << num << " ";
    std::cout << "\n";

    return 0;
}
