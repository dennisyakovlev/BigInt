#pragma once
#include <algorithm>
#include <cmath>
#include <float.h>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

constexpr unsigned long long BASE = 4294967296; // base
                                                // Must be power of 2
                                                // Note: digits range from [0, base)
// should be unsigned int
constexpr unsigned int BASE_DIGITS = 10; // number of digits in base
constexpr unsigned int BASE_DIGITS_ALLOW = 9; // maximum number of digits allowed to be stored
                                                     // store one less than allowed digits to allow initialization
                                                     // without an if statement to see if number fits into digit
constexpr unsigned int BASE_BIN_LENGTH = 32; // number of binary digits in the base

int CHECK() {
    int check = 1;
    if (BASE > std::pow(static_cast<long double>(10), static_cast<long double>(LDBL_MAX_10_EXP - 1))) {
        std::cerr << "maximum digit value exeeds maximum long double power value" << std::endl;
        check = 0;
    }

    if (BASE > LDBL_MAX) {
        std::cerr << "maximum value of digit exceeds long double max" << std::endl;
        check = 0;
    }
    return check;
}

struct _base_tag {};

class BigUnsigned {
    // digits sits at start of initialized range
    // one empty space before digits and none after
public:

    using _ull = unsigned long long;
    using _ld = long double;
    using _ui = unsigned int;
    using container = _ull*; // must have bi-directional iterator at least
    using const_container = const _ull*;

    template<typename Input>
    _ull _unchecked_to_num(Input start, Input end) {
        // end - start <= MAX_DIGITS_NUM
        _ld res = 0;
        for (; start != end; ++start) {
            res += static_cast<_ld>(*start) * std::pow(static_cast<_ld>(10), static_cast<_ld>(end - start - 1));
        }
        return static_cast<_ull>(std::llroundl(res));
    }

    template<typename Input>
    void _create(Input start, Input end) {
        _ui size_local = static_cast<_ui>(std::distance(start, end));
        _ui* digit_local = new _ui[size_local]; // doesnt need to be _ull can be unsigned ints
        std::copy(start, end, digit_local);

        std::list<_ull> digits_local;

        while (size_local >= BASE_DIGITS) { // possibly base_digit_allowed instead of base_digits
            _ull starting_num = _unchecked_to_num(digit_local, digit_local + BASE_DIGITS - 1);
            _ui* end_cont = digit_local + size_local;
            _ui* start_cont = digit_local + BASE_DIGITS - 1;
            for (_ui* j = start_cont; j != end_cont; ++j, ++digit_local) {
                starting_num = (starting_num * 10) + *j;
                *digit_local = starting_num / BASE;
                starting_num -= (starting_num / BASE) * BASE;
            }
            size_local = end_cont - start_cont;
            digit_local -= size_local;
            digits_local.emplace_front(starting_num);
        }
        _ull num = _unchecked_to_num(digit_local, digit_local + size_local);
        if (num != 0) {
            digits_local.emplace_front(num);
        }

        size = digits_local.size();
        alloc_size = size + 1;
        digits = new _ull[alloc_size];
        ++digits;
        std::copy(digits_local.cbegin(), digits_local.cend(), digits);
    }

    template<typename Integral, unsigned int N, typename = std::enable_if_t<std::is_integral_v<Integral> && std::is_unsigned_v<Integral>>>
    explicit BigUnsigned(Integral(&arr)[N]) {
        
        _create(arr, arr + N);

    }

    explicit BigUnsigned(_ui size_local) : size(size_local) {
        
        digits = new _ull[size_local];
    
    }

    template<typename Random>
    explicit BigUnsigned(Random start, Random end) {

        _create(start, end);

    }

    template<typename Random>
    explicit BigUnsigned(Random start, Random end, _base_tag) {

        digits = new _ull[end - start];
        std::copy(start, end, digits);

    }

    // should prolly fix the repition
    explicit BigUnsigned(const std::string& s) {

        _ui size_local = static_cast<_ui>(std::distance(s.cbegin(), s.cend()));
        _ui* digit_local = new _ui[size_local]; // doesnt need to be _ull can unsigned ints
        std::for_each(s.cbegin(), s.cend(), [&digit_local](auto in) {
            *digit_local++ = in - '0';
        });
        digit_local -= size_local;

        std::list<_ull> digits_local;

        while (size_local >= BASE_DIGITS) { // possibly base_digit_allowed instead of base_digits
            _ull starting_num = _unchecked_to_num(digit_local, digit_local + BASE_DIGITS - 1);
            _ui* end_cont = digit_local + size_local;
            _ui* start_cont = digit_local + BASE_DIGITS - 1;
            for (_ui* j = start_cont; j != end_cont; ++j, ++digit_local) {
                starting_num = (starting_num * 10) + *j;
                *digit_local = starting_num / BASE;
                starting_num -= (starting_num / BASE) * BASE;
            }
            size_local = end_cont - start_cont;
            digit_local -= size_local;
            digits_local.emplace_front(starting_num);
        }
        _ull num = _unchecked_to_num(digit_local, digit_local + size_local);
        if (num != 0) {
            digits_local.emplace_front(num);
        }

        size = digits_local.size();
        alloc_size = size + 1;
        digits = new _ull[alloc_size];
        ++digits;
        std::copy(digits_local.cbegin(), digits_local.cend(), digits);

    }

    const container get_digits() const {
        return const_cast<const container>(digits);
    }

    container get_digits() {
        return const_cast<container>(static_cast<const BigUnsigned&>(*this).get_digits());
    }

    // left to right beginning
    const_container cbegin() const {
        return digits;
    }

    const_container begin() const {
        return cbegin();
    }

    container begin() {
        return const_cast<container>(static_cast<const BigUnsigned&>(*this).cbegin());
    }

    // right to left beginning
    const_container crbegin() const {
        return digits + size - 1;
    }

    const_container rbegin() const {
        return crbegin();
    }

    container rbegin() {
        return const_cast<container>(static_cast<const BigUnsigned&>(*this).crbegin());
    }

    // left to right end
    const_container cend() const {
        return digits + size;
    }

    const_container end() const {
        return cend();
    }
    
    container end() {
        return const_cast<container>(static_cast<const BigUnsigned&>(*this).cend());
    }

    // right to left end
    const_container crend() const {
        return digits - 1;
    }

    const_container rend() const {
        return crend();
    }
    
    container rend() {
        return const_cast<container>(static_cast<const BigUnsigned&>(*this).crend());
    }

    container digits;
    _ui size; // number of digits
    _ui alloc_size; // number of allocated spaces

};

std::ostream& operator<< (std::ostream& out, const BigUnsigned& num) {
    std::copy(num.cbegin(), num.cend(), std::ostream_iterator<BigUnsigned::_ull>(out, ", "));
    return out;
}

inline auto _longer(const BigUnsigned* const l, const BigUnsigned* const r) {
    return std::max(l, r, [](auto l, auto r) {
        return l->size < r->size;
    });
}

inline auto _shorter(const BigUnsigned* const l, const BigUnsigned* const r) {
    return std::min(l, r, [](auto l, auto r) {
        return l->size <= r->size;
    });
}

auto _longer_digits(const BigUnsigned* const l, const BigUnsigned* const r) {
    return std::max(l, r, [](auto l, auto r) {
        return l->size < r->size;
    })->digits;
}

inline auto _shorter_digits(const BigUnsigned* const l, const BigUnsigned* const r) {
    return std::min(l, r, [](auto l, auto r) {
        return l->size <= r->size;
    })->digits;
}

bool operator< (const BigUnsigned& l, const BigUnsigned& r) {

    BigUnsigned::_ui l_len = l.size;
    BigUnsigned::_ui r_len = r.size;
    if (l_len < r_len) {
         return true;
    } else if (l_len > r_len) {
        return false;
    } 

    auto l_iter = l.cbegin();
    auto r_iter = r.cbegin();
    while (l_iter != l.cend() && *l_iter == *r_iter) {
        ++l_iter;
        ++r_iter;
    }
    return *l_iter < *r_iter;

}

bool operator> (const BigUnsigned& l, const BigUnsigned& r) {

    return r < l;

}

bool operator<= (const BigUnsigned& l, const BigUnsigned& r) {
    auto l_iter = std::find_if_not(l.cbegin(), l.cend(), [](auto i) {
        return i == 0;
    });
    auto r_iter = std::find_if_not(r.cbegin(), r.cend(), [](auto i) {
        return i == 0;
    });

    BigUnsigned::_ui l_len = l.cend() - l_iter;
    BigUnsigned::_ui r_len = r.cend() - r_iter;
    if (l_len > r_len) {
        return false;
    } else if (l_len < r_len) {
        return true;
    }

    while (l_iter != l.cend() && *l_iter == *r_iter) {
        ++l_iter;
        ++r_iter;
    }
    
    return *l_iter <= *r_iter;
}

bool operator>= (const BigUnsigned& l, const BigUnsigned& r) {
    auto l_iter = std::find_if_not(l.cbegin(), l.cend(), [](auto i) {
        return i == 0;
    });
    auto r_iter = std::find_if_not(r.cbegin(), r.cend(), [](auto i) {
        return i == 0;
    });

    BigUnsigned::_ui l_len = l.cend() - l_iter;
    BigUnsigned::_ui r_len = r.cend() - r_iter;
    if (l_len > r_len) {
        return true;
    } else if (l_len < r_len) {
        return false;
    }

    while (l_iter != l.cend() && *l_iter == *r_iter) {
        ++l_iter;
        ++r_iter;
    }

    return *l_iter >= *r_iter;
}

// NOTE: possible over flow with placing last carry into the front
// if the digits fully fill the allocated size
// theta(max(l.size, r.size))
BigUnsigned operator+ (const BigUnsigned& l, const BigUnsigned& r) {

    auto longer = ::_longer(&l, &r);
    auto shorter = ::_shorter(&l, &r);

    BigUnsigned result(longer->size);
    auto new_digits = result.rbegin();

    auto l_iter = longer->crbegin();
    BigUnsigned::_ull carry = 0;
    for (auto s_iter = shorter->crbegin(); s_iter != shorter->crend(); --l_iter, --s_iter, --new_digits) {
        BigUnsigned::_ull result = *l_iter + *s_iter + carry;
        carry = result >> BASE_BIN_LENGTH;
        *new_digits = result - (BASE * carry);
    }

    // POSSIBLY add third loop which just copies when the carry is over
    // since accessing carry is expensive
    for (; l_iter != longer->crend(); --l_iter, --new_digits) {
        BigUnsigned::_ull result = *l_iter + carry;
        carry = result >> BASE_BIN_LENGTH;
        *new_digits = result - (BASE * carry);
    }
    
    // digits.emplace_front(carry);

    return result;
}

// theta(max(l.size, r.size))
// note: doesnt care about order, the shorter number will be subtracted
// from the longer one, which kind of allows for negatives if youre careful
BigUnsigned operator- (const BigUnsigned& l, const BigUnsigned& r) {

    auto longer = ::_longer(&l, &r);
    auto shorter = ::_shorter(&l, &r);

    BigUnsigned result(longer->size);
    auto new_digits = result.rbegin();

    if (l.size == r.size) { // same size
        auto l_iter = l.crbegin();
        auto r_iter = r.crbegin();

        if (l > r) {
            BigUnsigned::_ull carry = 0;
            for (; l_iter != l.crend(); --l_iter, --r_iter, --new_digits) {
                BigUnsigned::_ull result = (*l_iter - carry) - *r_iter;
                carry = (result >> BASE_BIN_LENGTH) & 1;
                *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
            }
        } else if (r > l) {
            BigUnsigned::_ull carry = 0;
            for (; l_iter != l.crend(); --l_iter, --r_iter, --new_digits) {
                BigUnsigned::_ull result = (*r_iter - carry) - *l_iter;
                carry = (result >> BASE_BIN_LENGTH) & 1;
                *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
            }
        } else {
            return BigUnsigned("0");
        }
    } else {
        auto l_iter = longer->crbegin();
        BigUnsigned::_ull carry = 0;
        for (auto s_iter = shorter->crbegin(); s_iter != shorter->crend(); --l_iter, --s_iter, --new_digits) {
            BigUnsigned::_ull result = (*l_iter - carry) - *s_iter;
            carry = (result >> BASE_BIN_LENGTH) & 1; // why is & 1 here?
            *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
        }

        // instead shift the carry 63 bits right, which will be 1
        // new_digits is just result - carry
        // shift carry after new_digits

        for (; l_iter != longer->crend(); --l_iter, --new_digits) {
            BigUnsigned::_ull result = *l_iter - carry;
            carry = (result >> BASE_BIN_LENGTH) & 1;
            *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
        }
    }

    return result;
}

// theta(2 * l.size * r.size) pre slow
// i dont care enough to use a fast multiplication algorithm since i dont care enough
// if needed will implement
BigUnsigned operator* (const BigUnsigned& l, const BigUnsigned& r) {

    BigUnsigned res("0");

    for (auto r_iter = r.crbegin(); r_iter != r.crend(); --r_iter) {

        BigUnsigned::_ui shift = r.crbegin() - r_iter;

        BigUnsigned curr(l.size + 1 + shift);
        auto curr_iter = curr.rbegin();
        for (BigUnsigned::_ui i = 0; i != shift; ++i, --curr_iter) {
            *curr_iter = 0;
        }
        BigUnsigned::_ull carry = 0;
        for (auto l_iter = l.crbegin(); l_iter != l.crend(); --l_iter, --curr_iter) {
            BigUnsigned::_ull result = *l_iter * *r_iter + carry;
            carry = result >> BASE_BIN_LENGTH;
            *curr_iter = result - (BASE * carry);
        }
        *curr_iter = carry;

        // std::cout << curr << std::endl;

        res = res + curr;
    }

    // umm im not sure how much i approve
    if (*res.cbegin() == 0) {
        ++res.digits;
        --res.size;
    }

    return res;

}

BigUnsigned divide_digit(const BigUnsigned& num, const BigUnsigned::_ull digit) {

    BigUnsigned res(num.size);
    auto iter_res = res.begin();
    auto iter_num = num.cbegin();

    BigUnsigned::_ull carry = 0;

    for (; iter_num != num.cend(); ++iter_num, ++iter_res) {
        *iter_res = (*iter_num + (carry * BASE)) / digit;
        carry = *iter_num - (digit * *iter_res); 
    }

    return res;

}

BigUnsigned divide_digit_temp(BigUnsigned::const_container start, BigUnsigned::const_container end, const BigUnsigned::_ull digit) {

    BigUnsigned res(end - start);
    auto iter_res = res.begin();

    BigUnsigned::_ull carry = 0;

    for (; start != end; ++start, ++iter_res) {
        *iter_res = (*start + (carry * BASE)) / digit;
        carry = *start - (digit * *iter_res); 
    }

    return res;

}

// theta(n.size * operator*) really slow
// i dont care enough to use a fast division algorithm since they all require decimals
// if needed will implement
BigUnsigned operator/ (const BigUnsigned& n, const BigUnsigned& d) {

    BigUnsigned::_ui m = d.size - 1;
    BigUnsigned::_ull a = *d.cbegin();
    BigUnsigned q = divide_digit_temp(n.cbegin(), n.cbegin() + (n.size - d.size + 1), a);
    BigUnsigned r = d;

    // std::cout << m << std::endl;
    // std::cout << a << std::endl;
    // std::cout << q << std::endl;
    // std::cout << r << std::endl << std::endl;

    // int i = 0;

    while (r >= d) {
        r = n - (q * d);
        auto qn = q - (divide_digit_temp(r.cbegin(), r.cbegin() + (r.size - d.size + 1), a) + BigUnsigned("1"));
        auto sum = q + qn;
        q = divide_digit_temp(sum.cbegin(), sum.cend(), 2);
        // std::cout << r << " | " << d << " | " << (r >= d) << std::endl;
        std::cout << "Loop:\n" << r << "\n" << qn << "\n" << q << std::endl << std::endl;
    }

    return q;

}