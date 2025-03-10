#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <ctime>

extern "C" int SHA3_224(unsigned char *, const unsigned char *, size_t);

// This is for the bonus question
#define CLOCK_TIMING 0
#if CLOCK_TIMING
#include <chrono>
#define REQUIRED_ZERO_BITS 32
#endif

#define HM_STR "<put h(amt0) for nonce 1, or h(m1) for nonce 2; pad with zeroes>"
#define H_STR "<put m1 for nonce 1, or m2 for nonce 2; pad with zeroes>"
#define HM_BITS 224
#define M_BITS 4104
#define NONCE_BITS 128
#define NON_NONCE_BITS (HM_BITS + M_BITS)
#define PRE_IMG_BITS (NON_NONCE_BITS + NONCE_BITS)
#define NON_NONCE_BYTES (NON_NONCE_BITS / 8)
#define PRE_IMG_BYTES (PRE_IMG_BITS / 8)
#define HASH_BYTES 28

std::string get_randomized_nonce(const int thread_number) {
    auto randomized_nonce = std::string("");
    for (int i = 0; i < NONCE_BITS; i++) {
        randomized_nonce += std::to_string(rand() % 2);
    }
    // Guide the randomization so it does not repeat computations
    randomized_nonce[0] = '0' + thread_number % 2;
    randomized_nonce[1] = '0' + (thread_number / 2) % 2;
    randomized_nonce[2] = '0' + (thread_number / 4) % 2;
    randomized_nonce[3] = '0' + (thread_number / 8) % 2;
    randomized_nonce[4] = '0' + (thread_number / 16) % 2;
    randomized_nonce[5] = '0' + (thread_number / 32) % 2;
    return randomized_nonce;
}

std::string get_input(const int thread_number) {
    auto h_m = std::string(HM_STR);
    auto m = std::string(H_STR);
    if (h_m.size() != HM_BITS || m.size() != M_BITS) {
        std::cerr << "Bad size, h_m = " << h_m.size() << ", m = " << m.size() << std::endl;
        exit(-1);
    }
    auto zeros = get_randomized_nonce(thread_number);
    return h_m + m + zeros;
}

void increase(unsigned char *input_arr) {
    for (unsigned long i = PRE_IMG_BITS - 1; i >= NON_NONCE_BITS; i--) {
        unsigned long byte = i / 8;
        unsigned long offset = 7 - i % 8;
        unsigned int bit = (input_arr[byte] & (1U << offset)) >> offset;
        input_arr[byte] ^= (1U << offset);
        if (!bit) {
            return;
        }
    }
    for (unsigned long i = NON_NONCE_BYTES; i < PRE_IMG_BYTES; i++) {
        input_arr[i] = 0;
    }
}

void output_pre_image_and_terminate(const int thread_number, const unsigned char *input_arr) {
    std::string pre_image("Pre-Image (thread ");
    pre_image += std::to_string(thread_number);
    pre_image += "): ";
    for (int i = 0; i < PRE_IMG_BYTES; i++) {
        pre_image += std::to_string((input_arr[i] & 0b10000000) >> 7);
        pre_image += std::to_string((input_arr[i] & 0b01000000) >> 6);
        pre_image += std::to_string((input_arr[i] & 0b00100000) >> 5);
        pre_image += std::to_string((input_arr[i] & 0b00010000) >> 4);
        pre_image += std::to_string((input_arr[i] & 0b00001000) >> 3);
        pre_image += std::to_string((input_arr[i] & 0b00000100) >> 2);
        pre_image += std::to_string((input_arr[i] & 0b00000010) >> 1);
        pre_image += std::to_string((input_arr[i] & 0b00000001) >> 0);
    }
    pre_image += '\n';
    std::cout << pre_image << std::endl;
    exit(0);
}

[[noreturn]] void thread(const int thread_number) {
    auto input_str = get_input(thread_number);
    unsigned char input_arr[PRE_IMG_BYTES + 1];
    input_arr[PRE_IMG_BYTES] = '\0';
    for (int i = 0; i < PRE_IMG_BYTES; i++) {
        unsigned char value = 0;
        for (int j = 0; j < 8; j++) {
            value |= (input_str.at(8 * i + j) - '0') << (7 - j);
        }
        input_arr[i] = value;
    }
    unsigned char buffer[HASH_BYTES + 1];
    memset(buffer, 0, HASH_BYTES + 1);
#if CLOCK_TIMING
    auto begin = std::chrono::steady_clock::now();
#endif
    while (true) {
        increase(input_arr);
        SHA3_224(buffer, input_arr, PRE_IMG_BYTES);
#if !CLOCK_TIMING
        if (buffer[0] == '\0' && buffer[1] == '\0' && buffer[2] == '\0' && buffer[3] == '\0') {
            output_pre_image_and_terminate(thread_number, input_arr);
        }
#else
        bool done = true;
        for (unsigned long i = 0; i < REQUIRED_ZERO_BITS; i++) {
            unsigned long byte = i / 8;
            unsigned long offset = 7 - i % 8;
            unsigned int bit = (buffer[byte] & (1U << offset)) >> offset;
            if (bit) {
                done = false;
                break;
            }
        }
        if (done) {
            auto end = std::chrono::steady_clock::now();
            int diff = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
            int minutes = diff / 60;
            int seconds = diff % 60;
            std::cout << "Time elapsed: " << minutes << "min " << seconds << "sec" << std::endl;
            output_pre_image_and_terminate(thread_number, input_arr);
        }
#endif
    }
}

void startThreads(const unsigned int thread_count) {
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    srand(time(0));
    for (unsigned int i = 0; i < thread_count; i++) {
        threads.emplace_back(std::thread(thread, i));
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
#if CLOCK_TIMING
    std::cout << "Computing for " << REQUIRED_ZERO_BITS << " leading zeros" << std::endl;
#endif
    std::cout << "Using " << thread_count << " threads" << std::endl;
    startThreads(thread_count);
    return 0;
}
