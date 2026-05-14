#include <iostream>
#include <vector>
#include <future>
#include <algorithm>
#include <iterator>

constexpr size_t MIN_ELEMENTS_PER_THREAD = 4;

template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f) {
    size_t length = std::distance(first, last);

    if (length <= MIN_ELEMENTS_PER_THREAD) {
        std::for_each(first, last, f);
        return;
    }

    Iterator midpoint = first;
    std::advance(midpoint, length / 2);

    std::future<void> first_half_future = std::async(
        std::launch::async,
        parallel_for_each<Iterator, Func>,
        first, midpoint, f
    );

    parallel_for_each(midpoint, last, f);

    first_half_future.get();
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

    std::cout << "Исходный массив: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";

    auto multiply_by_two = [](int& n) {
        n *= 2;
        };

    parallel_for_each(numbers.begin(), numbers.end(), multiply_by_two);

    std::cout << "После обработки: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";

    return 0;
}
