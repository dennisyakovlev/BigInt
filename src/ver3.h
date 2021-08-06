#pragma once
#include <algorithm>
#include <cmath>
#include <float.h>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <utility>

// constexpr unsigned long long BASE = 16; // base
//                                         // Must be power of 2
//                                         // Note: digits range from [0, base)
// constexpr unsigned long long BASE_DIGITS = 2; // number of digits in base
// constexpr unsigned long long BASE_DIGITS_ALLOW = 1; // maximum number of digits allowed to be stored
// constexpr unsigned long long BASE_BIN_LENGTH = 4; // number of binary digits in the base


constexpr unsigned long long BASE = ULLONG_MAX / 4; // base
                                                    // Must be power of 2
                                                    // Note: digits range from [0, base)
constexpr unsigned long long BASE_DIGITS = 19; // number of digits in base
constexpr unsigned long long BASE_DIGITS_ALLOW = 18; // maximum number of digits allowed to be stored
constexpr unsigned long long BASE_BIN_LENGTH = 62; // number of binary digits in the base

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

class BigUnsigned {
// private:
public:

    using _ull = unsigned long long;
    using _ld = long double;
    using _ui = unsigned int;
    using container = std::list<_ull>; // must have bi-directional iterator at least

    template<typename Input>
    _ull _unchecked_to_num(Input start, Input end) {
        // end - start <= MAX_DIGITS_NUM
        _ld res = 0;
        for (; start != end; ++start) {
            res += static_cast<_ld>(*start) * std::pow(static_cast<_ld>(10), static_cast<_ld>(end - start - 1));
        }
        return static_cast<_ull>(std::llroundl(res));
    }

    template<typename Random>
    void _unchecked_populate_digits(Random start, Random end, std::random_access_iterator_tag) {
        for (Random i = start + BASE_DIGITS_ALLOW; i <= end; i += BASE_DIGITS_ALLOW) {
            digits.emplace_back(_unchecked_to_num(i - BASE_DIGITS_ALLOW, i));
        }
        if ((end - start) % BASE_DIGITS_ALLOW != 0) {
            digits.emplace_back(_unchecked_to_num(end - ((end - start) % BASE_DIGITS_ALLOW), end));
        }
    }

public:

    template<typename Integral, unsigned int N, typename = std::enable_if_t<std::is_integral_v<Integral> && std::is_unsigned_v<Integral>>>
    explicit BigUnsigned(Integral(&arr)[N]) {
        _unchecked_populate_digits(arr, arr + N, std::random_access_iterator_tag());
    }

    template<typename Random>
    explicit BigUnsigned(Random start, Random end) {
        _unchecked_populate_digits(start, end, std::random_access_iterator_tag());
    }

    explicit BigUnsigned(container&& cont) : digits(std::move(cont)) {}

    const container* get_digits() const {
        return const_cast<const container*>(&digits);
    }

    container* get_digits() {
        return const_cast<container*>(static_cast<const BigUnsigned&>(*this).get_digits());
    }

// private:

    container digits;

};

auto _longer(const BigUnsigned* const l, const BigUnsigned* const r) {
    return std::max(l, r, [](auto l, auto r) {
        return l->get_digits()->size() < r->get_digits()->size();
    });
}

auto _shorter(const BigUnsigned* const l, const BigUnsigned* const r) {
    return std::min(l, r, [](auto l, auto r) {
        return l->get_digits()->size() <= r->get_digits()->size();
    });
}

auto _longer_digits(const BigUnsigned::container* const l, const BigUnsigned::container* const r) {
    return std::max(l, r, [](auto l, auto r) {
        return l->size() < r->size();
    });
}

auto _shorter_digits(const BigUnsigned::container* const l, const BigUnsigned::container* const r) {
    return std::min(l, r, [](auto l, auto r) {
        return l->size() <= r->size();
    });
}

BigUnsigned operator+ (const BigUnsigned& l, const BigUnsigned& r) {
    BigUnsigned::container digits;

    auto longer = ::_longer_digits(l.get_digits(), r.get_digits());
    auto shorter = ::_shorter_digits(l.get_digits(), r.get_digits());

    auto l_iter = longer->crbegin();
    BigUnsigned::_ull carry = 0;
    for (auto s_iter = shorter->crbegin(); s_iter != shorter->crend(); ++l_iter, ++s_iter) {
        BigUnsigned::_ull result = *l_iter + *s_iter + carry;
        carry = result >> BASE_BIN_LENGTH;
        digits.emplace_front(result - (BASE * carry));
        result - (BASE * carry);
    }

    // POSSIBLY add third loop which just copies when the carry is over
    // since accessing carry is expensive
    for (; l_iter != longer->crend(); ++l_iter) {
        BigUnsigned::_ull result = *l_iter + carry;
        carry = result >> BASE_BIN_LENGTH;
        digits.emplace_front(result - (BASE * carry));
        result - (BASE * carry);
    }
    
    digits.emplace_front(carry);

    return BigUnsigned(std::move(digits));
}

BigUnsigned operator- (const BigUnsigned& l, const BigUnsigned& r) {

}