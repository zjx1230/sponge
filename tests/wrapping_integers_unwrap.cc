#include "test_should_be.hh"
#include "wrapping_integers.hh"

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

int main() {
    try {
        cout << "111111\n";
        // Unwrap the first byte after ISN
        test_should_be(unwrap(WrappingInt32(1), WrappingInt32(0), 0), 1ul);
        cout << "222222\n";
        // Unwrap the first byte after the first wrap
        test_should_be(unwrap(WrappingInt32(1), WrappingInt32(0), UINT32_MAX), (1ul << 32) + 1);
        cout << "333333\n";
        // Unwrap the last byte before the third wrap
        test_should_be(unwrap(WrappingInt32(UINT32_MAX - 1), WrappingInt32(0), 3 * (1ul << 32)), 3 * (1ul << 32) - 2);
        cout << "444444\n";
        // Unwrap the 10th from last byte before the third wrap
        test_should_be(unwrap(WrappingInt32(UINT32_MAX - 10), WrappingInt32(0), 3 * (1ul << 32)), 3 * (1ul << 32) - 11);
        cout << "555555\n";
        // Non-zero ISN
        test_should_be(unwrap(WrappingInt32(UINT32_MAX), WrappingInt32(10), 3 * (1ul << 32)), 3 * (1ul << 32) - 11);
        cout << "666666\n";
        // Big unwrap
        test_should_be(unwrap(WrappingInt32(UINT32_MAX), WrappingInt32(0), 0), static_cast<uint64_t>(UINT32_MAX));
        cout << "777777\n";
        // Unwrap a non-zero ISN
        test_should_be(unwrap(WrappingInt32(16), WrappingInt32(16), 0), 0ul);
        cout << "888888\n";
        // Big unwrap with non-zero ISN
        test_should_be(unwrap(WrappingInt32(15), WrappingInt32(16), 0), static_cast<uint64_t>(UINT32_MAX));
        cout << "999999\n";
        // Big unwrap with non-zero ISN
        test_should_be(unwrap(WrappingInt32(0), WrappingInt32(INT32_MAX), 0), static_cast<uint64_t>(INT32_MAX) + 2);
        cout << "1111111111111111\n";
        // Barely big unwrap with non-zero ISN
        test_should_be(unwrap(WrappingInt32(UINT32_MAX), WrappingInt32(INT32_MAX), 0), static_cast<uint64_t>(1) << 31);
        cout << "aaaaaaaaaaaaaaaa\n";
        // Nearly big unwrap with non-zero ISN
        test_should_be(unwrap(WrappingInt32(UINT32_MAX), WrappingInt32(1ul << 31), 0),
                       static_cast<uint64_t>(UINT32_MAX) >> 1);
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return EXIT_SUCCESS;
}
