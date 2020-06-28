#include <iostream>
#include <vector>
#include <thread>
#include <cstring>

extern "C" int SHA3_224(unsigned char *, const unsigned char *, size_t);

#define HM_STR "<put h(amt0) for nonce 1, or h(m1) for nonce 2; pad with zeroes>"
#define H_STR "<put m1 for nonce 1, or m2 for nonce 2; pad with zeroes>"
#define HM_BITS 224
#define M_BITS 4104
#define NONCE_BITS 128
#define NON_NONCE_BITS (HM_BITS + M_BITS)
#define PRE_IMG_BITS (NON_NONCE_BITS + NONCE_BITS)
#define PRE_IMG_BYTES (PRE_IMG_BITS / 8)
#define HASH_BYTES 28

std::string get_zeros(const int thread_number) {
    auto zeros = std::string("");
    for (int i = 0; i < NONCE_BITS; i++) {
        zeros += "0";
    }
    switch (thread_number) {
        case 0:
            break;
        case 1:
            zeros[0] = '1';
            break;
        case 2:
            zeros[1] = '1';
            break;
        case 3:
            zeros[0] = '1';
            zeros[1] = '1';
            break;
        case 4:
            zeros[2] = '1';
            break;
        case 5:
            zeros[0] = '1';
            zeros[2] = '1';
            break;
        case 6:
            zeros[1] = '1';
            zeros[2] = '1';
            break;
        case 7:
            zeros[0] = '1';
            zeros[1] = '1';
            zeros[2] = '1';
            break;
        default:
            std::cerr << "Maximum of 8 threads" << std::endl;
            exit(-1);
    }
    return zeros;
}

std::string get_input(const int thread_number) {
    auto h_m = std::string(HM_STR);
    auto m = std::string(H_STR);
    if (h_m.size() != HM_BITS || m.size() != M_BITS) {
        std::cerr << "Bad size, h_m = " << h_m.size() << ", m = " << m.size() << std::endl;
        exit(-1);
    }
    auto zeros = get_zeros(thread_number);
    return h_m + m + zeros;
}

bool increase(unsigned char *input_arr) {
    for (unsigned long i = PRE_IMG_BITS - 1; i >= NON_NONCE_BITS; i--) {
        unsigned long byte = i / 8;
        unsigned long offset = 7 - i % 8;
        unsigned int bit = (input_arr[byte] & (1U << offset)) >> offset;
        input_arr[byte] ^= (1U << offset);
        if (!bit) {
            return true;
        }
    }
    return false;
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

void thread(const int thread_number) {
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
    do {
        SHA3_224(buffer, input_arr, PRE_IMG_BYTES);
        if (buffer[0] == '\0' && buffer[1] == '\0' && buffer[2] == '\0' && buffer[3] == '\0') {
            output_pre_image_and_terminate(thread_number, input_arr);
        }
    } while (increase(input_arr));
}

void startThreads(const unsigned int thread_count) {
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
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
    std::cout << "Using " << thread_count << " threads" << std::endl;
    startThreads(thread_count);
    return 0;
}
