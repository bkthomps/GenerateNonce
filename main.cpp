#include <iostream>
#include <vector>
#include <thread>
#include <cstring>

extern "C" int SHA3_224(unsigned char *, const unsigned char *, size_t);

std::string get_input(const int thread_number) {
    auto h_m = std::string("<put h(amt0) for nonce 1, or h(m1) for nonce 2; pad with zeroes>");
    auto m = std::string("<put m1 for nonce 1, or m2 for nonce 2; pad with zeroes>");
    if (h_m.size() != 224 || m.size() != 4104) {
        std::cerr << "Bad size, h_m = " << h_m.size() << ", m = " << m.size() << std::endl;
        exit(-1);
    }
    auto zeros = std::string("");
    for (int i = 0; i < 128; i++) {
        zeros += "0";
    }
    auto input = h_m + m + zeros;
    switch (thread_number) {
        case 0:
            break;
        case 1:
            input[input.size() - 128] = '1';
            break;
        case 2:
            input[input.size() - 127] = '1';
            break;
        case 3:
            input[input.size() - 128] = '1';
            input[input.size() - 127] = '1';
            break;
        case 4:
            input[input.size() - 126] = '1';
            break;
        case 5:
            input[input.size() - 128] = '1';
            input[input.size() - 126] = '1';
            break;
        case 6:
            input[input.size() - 127] = '1';
            input[input.size() - 126] = '1';
            break;
        case 7:
            input[input.size() - 128] = '1';
            input[input.size() - 127] = '1';
            input[input.size() - 126] = '1';
            break;
        default:
            std::cerr << "Maximum of 8 threads" << std::endl;
            exit(-1);
    }
    return input;
}

bool increase(unsigned char *input_arr) {
    for (unsigned long i = 224 + 4104 + 128 - 1; i >= 224 + 4104; i--) {
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

void thread(const int thread_number) {
    auto input_str = get_input(thread_number);
    int bytes = (224 + 4104 + 128) / 8;
    unsigned char input_arr[bytes + 1];
    input_arr[bytes] = '\0';
    for (int i = 0; i < bytes; i++) {
        unsigned char value = 0;
        for (int j = 0; j < 8; j++) {
            value |= (input_str.at(8 * i + j) - '0') << (7 - j);
        }
        input_arr[i] = value;
    }
    unsigned char pre_image_buffer[64];
    memset(pre_image_buffer, 0, 64);
    do {
        SHA3_224(pre_image_buffer, input_arr, bytes);
        if (pre_image_buffer[0] == '\0' && pre_image_buffer[1] == '\0'
            && pre_image_buffer[2] == '\0' && pre_image_buffer[3] == '\0') {
            std::string nonce("Nonce (thread ");
            nonce += std::to_string(thread_number);
            nonce += "): ";
            for (int i = (224 + 4104) / 8; i < (224 + 4104 + 128) / 8; i++) {
                nonce += std::to_string((input_arr[i] & 0b10000000) >> 7);
                nonce += std::to_string((input_arr[i] & 0b01000000) >> 6);
                nonce += std::to_string((input_arr[i] & 0b00100000) >> 5);
                nonce += std::to_string((input_arr[i] & 0b00010000) >> 4);
                nonce += std::to_string((input_arr[i] & 0b00001000) >> 3);
                nonce += std::to_string((input_arr[i] & 0b00000100) >> 2);
                nonce += std::to_string((input_arr[i] & 0b00000010) >> 1);
                nonce += std::to_string((input_arr[i] & 0b00000001) >> 0);
            }
            nonce += '\n';
            std::string pre_image("Pre-Image (thread ");
            pre_image += std::to_string(thread_number);
            pre_image += "): ";
            for (int i = 0; i < 28; i++) {
                pre_image += std::to_string((pre_image_buffer[i] & 0b10000000) >> 7);
                pre_image += std::to_string((pre_image_buffer[i] & 0b01000000) >> 6);
                pre_image += std::to_string((pre_image_buffer[i] & 0b00100000) >> 5);
                pre_image += std::to_string((pre_image_buffer[i] & 0b00010000) >> 4);
                pre_image += std::to_string((pre_image_buffer[i] & 0b00001000) >> 3);
                pre_image += std::to_string((pre_image_buffer[i] & 0b00000100) >> 2);
                pre_image += std::to_string((pre_image_buffer[i] & 0b00000010) >> 1);
                pre_image += std::to_string((pre_image_buffer[i] & 0b00000001) >> 0);
            }
            pre_image += '\n';
            std::cout << nonce << pre_image << std::endl;
            exit(0);
        }
    } while (increase(input_arr));
}

void startThreads(const unsigned int thread_count) {
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (int i = 0; i < thread_count; i++) {
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
