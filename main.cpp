#include <iostream>
#include <vector>
#include <thread>

extern "C" int SHA3_224(unsigned char *, const unsigned char *, size_t);

std::string get_input(const int thread_number) {
    auto h_m = std::string(
            "10001011011011110101110111000101101101000000010101011001111101111000010001010100011100010110101100100000100011111010001110001010011110001001011101000011010101011010110110110010001011000110111000110100011110000111110110011100");
    auto m = std::string(
            "000111011110111101011111000011100100011101001101100111111010001011010000111001111011011001011001011001101111101101000000100100001001100011000001111111101111100001101111000110111011110111101011001000000100011100101111110110010010111010011111110101101111000100000101111111101110100011000010111010110001100001110001011110000011110000111101000100100111100011100101100101001000000111100110001101001100100110110001100110010101110000101000110101001100111101001110101001101000100110100110010010110011001110110000010010011101111011111010001010110110011110101000100110100101000010010110000111001110000110001101010110110101011101100001110000111000100011111100010010110110000010010001000001001101101011100110100011111111000010101111010001111001010111010010001001100000110011011011010000001010000010110000011101000100010100111001111001111001110000111000010101111011111000101001111000000100011111100111101011101110101000110010000011110001000110001111011101101110110010101001001000000001001101011010110001101100000000111110011100000010111011110101001100111011010010010000000011010010111101001110001111100001100110101001100010110000110111000011011001001101101011010000111010111110110101110011100111100101010010110101100011001000000001010011100011000010010110011000011001010001010111111011110010011011111111111111010100000011111010001010010010111010000001111100010001010011010111011110100110001111010111101100001101111111011000101011000001111001101111111101100001101100000110001101011001011011011001101100101100101000001100000101101100111100010001100111101111100110000010100000100100001100110011000110110101001010101100010000001110110110001001011001111101101100111101100101010011111110101010001101001010001001001010101111011001111111110110100011110100111110111001100011000010110001101100110100000110011011100000010111100000101001111101111011101001011010010101001011011001011011110111010000100101111010011011000111110101111111100000101001001110010001100001001001100110101100111111110000100110111011110100010010001111000010011010110111001101100100101011110101010000000110101111001100110011101011010011011101000111101011000000100111011010111010010001100011010111110001111011101011111101011010100011010001000000111101010101111101011001011100001010100001010001111001001010100101000010001000000110010111000100101001011011010110010001001100100111100010011101100111011110001101110101000100101010011010001011000000111111100101011111010100111111011001011100001011010111101101110110100110110110101010101100111101011110001101100011011100110000110001100101110011110101111010000011010110110000111100111101011010101100000001011101010100001111011110101100000100110101010000111001010010100111100110110111110111110010111011000001100000100000000001100000001000111010010010110101111011010000010111110100001111110101000100111001011110101110011001000011100101100011110100001101011001101100001001100110110010001111101010001101111101100010010001101100000001101000001011100110001101101101000101111000010001100110000110110100001000001011010110010000011011000000110011100011010000101110100100010011111101001110001101001110100011100111111000001110001000001000100010100111110001001010010011111110010011000000101111111011100001001110111111001010010111011111010111001011110001110011000011010100111011110000100110111011110000001100110010000110011010000001110011010111011111111010111000010010011110001100100001000011000101111010100110101001001100100101101111011001100000101001010111011010110110101100111011110110011011101010100010010100101101110101110111100100000001111100001100110000010110110010101100010110111101110111110011011000101101100001001010110101111010100111011101001100100111001110100110011010111001001101001101011100110001010101010101100101000001001100101101000011010100000010110111111111111100100001111110101010011000010000111010010011010100011111011011100011111101101101110110101010110010111110001110001110111010110100010000111011101101011100011100100101101000011111011011001001100111010000111101101101110111100011110011100111101000111111010101001001110011010010010001101111111100100010101101000001111110001100000000011010101111110100000100");
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
        //SHA3_224(pre_image_buffer, input_arr, bytes);
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
