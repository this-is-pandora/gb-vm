#pragma once

namespace utils {
    template <typename t>
    bool utils::test_bit(t data, int pos) {
        t mask = 1 << pos;
        return (data & mask) ? true : false;
    }
};

