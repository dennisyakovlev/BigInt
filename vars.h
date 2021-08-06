#pragma once
#include <climits>
#include <cstdint>
#include <vector>

#define start_probability namespace pathprob {
#define end_probability }

start_probability

    // using declerations
    using size_t = uint32_t; // size type
    using uchar_t = unsigned char; // char size
    
    using size_vec = std::vector<size_t>; // size_t vector
    
    // size variables
    static constexpr size_t size_sz = sizeof(size_t{}); // size of size type
    static constexpr size_t char_sz = sizeof(uchar_t{}); // size of char type

    static constexpr size_t size_max = INT32_MAX; // max size of size tpye
    static constexpr size_t uchar_max = UCHAR_MAX; // max size of char tpye

    // variables
    static constexpr size_t max_grid_sz = 20;
    static constexpr size_t max_grid_digits = 2;
    // constexpr available in c++20
    static const size_vec factorials{1,2,3,4,5};

end_probability