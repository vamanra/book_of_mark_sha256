#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdint>  // Add this line for uint32_t and other fixed-width integers
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cctype>
#define ROTR(x, n) ((x >> n) | (x << (32 - n)))

#define CH(x, y, z) ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define EP0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIG0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ (x >> 3))
#define SIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ (x >> 10))

const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

std::vector<uint32_t> h = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

std::vector<uint8_t> padMessage(const std::string& message) {
    std::vector<uint8_t> padded(message.begin(), message.end());
    padded.push_back(0x80);
    while ((padded.size() * 8) % 512 != 448) {
        padded.push_back(0x00);
    }
    uint64_t length = message.size() * 8;
    for (int i = 7; i >= 0; --i) {
        padded.push_back(static_cast<uint8_t>((length >> (i * 8)) & 0xff));
    }
    return padded;
}

void processChunk(const std::vector<uint8_t>& chunk) {
    std::vector<uint32_t> w(64);
    for (int i = 0; i < 16; ++i) {
        w[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) | chunk[i * 4 + 3];
    }
    for (int i = 16; i < 64; ++i) {
        w[i] = SIG1(w[i - 2]) + w[i - 7] + SIG0(w[i - 15]) + w[i - 16];
    }
    uint32_t a = h[0];
    uint32_t b = h[1];
    uint32_t c = h[2];
    uint32_t d = h[3];
    uint32_t e = h[4];
    uint32_t f = h[5];
    uint32_t g = h[6];
    uint32_t hh = h[7];
    for (int i = 0; i < 64; ++i) {
        uint32_t temp1 = hh + EP1(e) + CH(e, f, g) + k[i] + w[i];
        uint32_t temp2 = EP0(a) + MAJ(a, b, c);
        hh = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }
    h[0] += a;
    h[1] += b;
    h[2] += c;
    h[3] += d;
    h[4] += e;
    h[5] += f;
    h[6] += g;
    h[7] += hh;
}

std::string sha256(const std::string& message) {
    std::vector<uint8_t> paddedMessage = padMessage(message);
    for (size_t i = 0; i < paddedMessage.size(); i += 64) {
        std::vector<uint8_t> chunk(paddedMessage.begin() + i, paddedMessage.begin() + i + 64);
        processChunk(chunk);
    }
    std::stringstream ss;
    for (uint32_t value : h) {
        ss << std::hex << std::setw(8) << std::setfill('0') << value;
    }
    return ss.str();
}

int main() {
    std::ifstream file("book_of_mark_corrected.txt");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();
    std::string hash = sha256(text);
    std::cout << "SHA-256 Hash of the Book of Mark: " << hash << std::endl;
    return 0;
}
