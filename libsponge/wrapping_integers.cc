#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

uint64_t uint64_diff(uint64_t a, uint64_t b) {
    return a > b ? a - b : b - a;
}

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    uint32_t raw_data = ((isn.raw_value() + uint32_t (n % UINT_MOD)) % UINT_MOD);
    return WrappingInt32{raw_data};
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint64_t ans = (uint64_t (n.raw_value()) + UINT_MOD - isn.raw_value()) % UINT_MOD;
    uint64_t diff = uint64_diff(ans, checkpoint);
    uint64_t temp = ans + max((diff / UINT_MOD), uint64_t (1)) * UINT_MOD;
    while (uint64_diff(temp, checkpoint) < diff) {
        diff = uint64_diff(temp, checkpoint);
        ans = temp;
        temp = ans + UINT_MOD;
    }
    return ans;
}


