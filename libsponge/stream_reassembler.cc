#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), _segmentIndexList(), _un_assembly(), _un_assembly_size(0), _free_un_assembly_index(), _cur_write_index(0), end_index(0) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const uint64_t index, const bool eof) {
    if (eof) {
        if (end_index == 0) {
            end_index = index + data.length();
        }
        if (_cur_write_index == end_index) _output.end_input();
    }

    if (data.empty()) return;

    if (_cur_write_index >= index + data.length()) {
        return;
    }

    // get start and end [start, end]
    uint64_t left_can_write_capacity = _capacity - _un_assembly_size - _output.buffer_size();
    if (left_can_write_capacity == 0) return;

    uint64_t max_end_index = _capacity + _cur_write_index - _output.buffer_size();
    uint64_t start = max(index, _cur_write_index);

    if (start >= max_end_index) return;
    uint64_t start_write_pos = start - index;
    uint64_t actual_push_len = min(data.length() - start_write_pos, max_end_index - start);
    uint64_t end = start + actual_push_len;
    uint64_t preStart = _cur_write_index;
//    std::cout << "===================================== " << " index " << index << std::endl;
//    std::cout << " start " << start << " end " << end << std::endl;
    auto it = _segmentIndexList.begin();
    while (it != _segmentIndexList.end()) {
        uint64_t preEnd = it->start_;
//        std::cout << "===================================== " << " preStart " << preStart << std::endl;
//        std::cout << "===================================== " << " preEnd " << preEnd << std::endl;
        if (preStart < start && end < preEnd) {
            _segmentIndexList.insert(it, SegmentIndex(start, end, _un_assembly.size()));
            _un_assembly_size += actual_push_len;
            _un_assembly.push_back(data.substr(start_write_pos, actual_push_len));
            return;
        }

        if (preStart >= end) {
            break;
        }
        if (preEnd <= start) {
            preStart = it->end_;
            it ++;
            continue;
        }

        if (preStart >= start && preEnd <= end) {
            uint64_t sectionSize = preEnd - preStart;
            left_can_write_capacity -= sectionSize;
            _segmentIndexList.insert(it, SegmentIndex(preStart, preEnd, _un_assembly.size()));
            _un_assembly_size += sectionSize;
            _un_assembly.push_back(data.substr(start_write_pos + preStart - start, sectionSize));
            if (left_can_write_capacity == 0) {
                break;
            }
            preStart = it->end_;
            it ++;
            continue;
        }

        if (start < preStart) {
            uint64_t sectionSize = end - preStart;
            _segmentIndexList.insert(it, SegmentIndex(preStart, end, _un_assembly.size()));
            _un_assembly_size += sectionSize;
            _un_assembly.push_back(data.substr(start_write_pos + preStart - start, sectionSize));
            break;
        } else {
            uint64_t sectionSize = preEnd - start;
            _segmentIndexList.insert(it, SegmentIndex(start, preEnd, _un_assembly.size()));
            _un_assembly_size += sectionSize;
            _un_assembly.push_back(data.substr(start_write_pos, sectionSize));
            left_can_write_capacity -= sectionSize;
            if (left_can_write_capacity == 0) {
                break;
            }
            preStart = it->end_;
            it ++;
            continue;
        }
    }

    if (!_segmentIndexList.empty()) {
        auto itr = --_segmentIndexList.end();
        if (itr->end_ < end && start <= itr->end_) {
            uint64_t sectionSize = end - itr->end_;
            _segmentIndexList.emplace_back(itr->end_, end, _un_assembly.size());
            _un_assembly_size += sectionSize;
            _un_assembly.push_back(data.substr(start_write_pos + itr->end_ - start, sectionSize));
        } else if (itr->end_ < end) {
            uint64_t sectionSize = end - start;
            _segmentIndexList.emplace_back(SegmentIndex(start, end, _un_assembly.size()));
            _un_assembly_size += sectionSize;
            _un_assembly.push_back(data.substr(start_write_pos, sectionSize));
        }
    }

//    std::cout << "================= mergeSegment pre ==================== " << " index " << index << std::endl;
//    for (auto itrr : _segmentIndexList) {
//        std::cout << itrr.start_ << " " << itrr.end_ << " " << itrr.index_ << " " << std::endl;
//    }
//    std::cout << "=================== mergeSegment pre ==================" << std::endl;

    // merge segment
    mergeSegment();

//    std::cout << "================= mergeSegment after ==================== " << "index " << index << std::endl;
//    for (auto itrr : _segmentIndexList) {
//        std::cout << itrr.start_ << " " << itrr.end_ << " " << itrr.index_ << " " << std::endl;
//    }
//    std::cout << "================== mergeSegment after ===================" << std::endl;

    if (_segmentIndexList.empty()) {
        _segmentIndexList.emplace_back(SegmentIndex(start, start + actual_push_len, _un_assembly.size()));
        _un_assembly.emplace_back(data.substr(start_write_pos, actual_push_len));
        _un_assembly_size += actual_push_len;
    }

    SegmentIndex& segmentIndex = _segmentIndexList.front();
    if (segmentIndex.start_ == _cur_write_index) {
        _un_assembly_size -= _un_assembly[segmentIndex.index_].length();
        _output.write(_un_assembly[segmentIndex.index_]);
        _cur_write_index = segmentIndex.end_;
        _free_un_assembly_index.push_back(segmentIndex.index_);
        _segmentIndexList.pop_front();
        if (_cur_write_index == end_index) _output.end_input();
    }
}

void StreamReassembler::mergeSegment() {
    auto cur = _segmentIndexList.begin();
    std::list<SegmentIndex>::iterator pre = _segmentIndexList.end();
    std::list<SegmentIndex>::iterator temp_end = pre;

    while (cur != _segmentIndexList.end()) {
        if (pre == temp_end) {
            pre = cur;
            cur ++;
            continue;
        }

        if (pre->end_ >= cur->start_) {
            pre->end_ = cur->end_;
            _un_assembly[pre->index_] = _un_assembly[pre->index_] + _un_assembly[cur->index_];
            _free_un_assembly_index.push_back(cur->index_);
            _segmentIndexList.erase(cur ++);
        } else {
            pre = cur;
            cur ++;
        }
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _un_assembly_size; }

bool StreamReassembler::empty() const { return _un_assembly_size == 0; }
