#include <iostream>
#include <vector>
#include <thread>

void thread(const int min, const int max) {
    std::cout << "Nonce " + std::to_string(min) + ' ' + std::to_string(max) + '\n';
}

void startThreads(const unsigned int thread_count) {
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (int i = 0; i < thread_count; i++) {
        threads.emplace_back(std::thread(thread, i, i));
    }
    for (auto &thread : threads) {
        thread.join();
    }
}

int main() {
    const unsigned int thread_count = std::thread::hardware_concurrency();
    if (!thread_count) {
        std::cerr << "Could not retrieve thread count" << std::endl;
        return -1;
    }
    std::cout << "Using " << thread_count << " threads" << std::endl;
    startThreads(thread_count);
    return 0;
}
