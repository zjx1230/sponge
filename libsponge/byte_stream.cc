#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

#include "byte_stream.hh"
#include <cmath>
#include <algorithm>
#include <cstring>
template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

char * subStr(char* s, int capacity, int start, int len) {
    char * ans = new char [len + 1];
    for (int i = 0; i < len; i ++) {
        ans[i] = s[(i + start) % capacity];
    }
    ans[len] = '\0';
    return ans;
}

ByteStream::ByteStream(const size_t capacity_) : read_pos(0), write_pos(0), capacity(capacity_), read_bytes(0), write_bytes(0), buffer(new char [capacity]), inputEnded(false) {
//    DUMMY_CODE(capacity);
}

size_t ByteStream::write(const string &data) {
//    DUMMY_CODE(data);
    size_t actual_write_size = min(data.length(), remaining_capacity());
    for (size_t i = 0; i < actual_write_size; i ++) {
        buffer[(write_pos + i) % capacity] = data[i];
    }
    write_pos = (write_pos + actual_write_size) % capacity;
    write_bytes += actual_write_size;
    return actual_write_size;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t actualLen = min(buffer_size(), len);
    string ans = "";
    for (size_t i = 0; i < actualLen; i ++) {
        ans += buffer[(i + read_pos) % capacity];
    }
    return ans;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t actualLen = min(buffer_size(), len);
    read_pos = (read_pos + actualLen) % capacity;
    read_bytes += actualLen;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    size_t actualLen = min(buffer_size(), len);
    string ans = "";
    for (size_t i = 0; i < actualLen; i ++) {
        ans += buffer[(i + read_pos) % capacity];
    }
    read_pos = (read_pos + actualLen) % capacity;
    read_bytes += actualLen;
    return ans;
}

void ByteStream::end_input() {
    inputEnded = true;
}

bool ByteStream::input_ended() const { return inputEnded; }

size_t ByteStream::buffer_size() const { return write_bytes - read_bytes; }

bool ByteStream::buffer_empty() const { return (write_bytes - read_bytes) == 0; }

bool ByteStream::eof() const { return inputEnded && read_pos == write_pos; }

size_t ByteStream::bytes_written() const { return write_bytes; }

size_t ByteStream::bytes_read() const { return read_bytes; }

size_t ByteStream::remaining_capacity() const {
    return capacity - buffer_size();
}
