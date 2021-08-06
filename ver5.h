#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using _ull = unsigned long long;
using _ui = unsigned int;
using _ld = long double;

constexpr _ui BASE_BIN_LENGTH = static_cast<_ui>(std::numeric_limits<_ull>::digits / 2); // number of binary digits in the base
constexpr _ull BASE = (ULLONG_MAX >> BASE_BIN_LENGTH) + 1; // base
                                                           // Must be power of 2
                                                           // Note: Should be half the binary length of ULLONG_MAX
                                                           // Note: digits range from [0, base)
constexpr _ui BASE_DIGITS = static_cast<_ui>((std::numeric_limits<_ull>::digits10 / 2) + 1); // number of digits in base
constexpr _ui BASE_DIGITS_ALLOW = BASE_DIGITS - 1; // maximum number of digits allowed to be stored
                                                   // store one less than allowed digits to allow initialization
                                                   // without an if statement to see if number fits into digit

// ty must be integral and unsigned type
template<typename ty, typename = std::enable_if_t<std::is_integral_v<ty> && std::is_unsigned_v<ty>>>
struct _integral_unsigned {};

class BigUnsigned {
public:

    using cont_ull = std::vector<_ull>; // must have bi-directional iterator at least
    using cont_ui = std::vector<_ui>; // must have random access iterator
    using sz_ty_ull = typename cont_ull::size_type; 
    using sz_ty_ui = typename cont_ui::size_type; 

private:

    // requires end - start <= MAX_DIGITS_NUM
    template<typename Forward>
    _ull _unchecked_to_num(Forward start, Forward end) {

        _ld res = 0;
        for (; start != end; ++start) {
            res += static_cast<_ld>(*start) * std::pow(static_cast<_ld>(10), static_cast<_ld>(end - start - 1));
        }

        return static_cast<_ull>(std::llroundl(res));

    }

    template<typename Forward>
    inline sz_ty_ui _local_size(Forward start, Forward end) {

        return static_cast<sz_ty_ui>(std::distance(start, end));

    }

    void _create(cont_ui::iterator digit_local_iter, sz_ty_ui size_local) {

        std::list<_ull> digits_local;

        while (size_local >= BASE_DIGITS) {
            _ull starting_num = _unchecked_to_num(digit_local_iter, digit_local_iter + BASE_DIGITS - 1);
            auto end_cont = digit_local_iter + size_local;
            auto start_cont = digit_local_iter + BASE_DIGITS - 1;
            for (auto j = start_cont; j != end_cont; ++j, ++digit_local_iter) {
                starting_num = (starting_num * 10) + *j;
                *digit_local_iter = starting_num / BASE;
                starting_num -= (starting_num / BASE) * BASE;
            }
            size_local = end_cont - start_cont;
            digit_local_iter -= size_local;
            digits_local.emplace_front(starting_num);
        }

        _ull num = _unchecked_to_num(digit_local_iter, digit_local_iter + size_local);
        if (num != 0) {
            digits_local.emplace_front(num);
        }

        std::copy(digits_local.cbegin(), digits_local.cend(), std::back_inserter(digits));

    }

public:

    template<typename Integral, unsigned int N, typename = _integral_unsigned<Integral>>
    explicit BigUnsigned(Integral(&arr)[N]) {

        cont_ui digit_local(N);
        std::copy(arr, arr + N, digit_local.begin());

        _create(digit_local.begin(), N);

    }

   // do not rigorously check that Forward is valid forward iterator
   // since not need all the forward iterator requirements are needed, 
   // just ::value_type and that its multipass
   // leave responsibility to user
    template<typename Forward, typename = _integral_unsigned<typename Forward::value_type>>
    explicit BigUnsigned(Forward start, Forward end) {

        auto size_local = _local_size(start, end);
        cont_ui digit_local(size_local);
        std::copy(start, end, digit_local.begin());

        _create(digit_local.begin(), size_local);

    }
    
private:

    void _str_init(const std::string* const s) {

        auto size_local = _local_size(s->cbegin(), s->cend());
        cont_ui digit_local(size_local);
        auto digit_local_iter = digit_local.begin();
        std::for_each(s->cbegin(), s->cend(), [&digit_local_iter](auto in) {
            *digit_local_iter++ = in - '0';
        });

        _create(digit_local.begin(), size_local);

    }

public:

    explicit BigUnsigned(const std::string& s) {

        _str_init(std::addressof(s));

    }

    explicit BigUnsigned(const std::string* const s) {

        _str_init(s);

    }

    explicit BigUnsigned(sz_ty_ull size) : digits(size) {}
    
    cont_ull digits;

};

std::ostream& operator<< (std::ostream& out, const BigUnsigned& num) {

    std::copy(num.digits.cbegin(), num.digits.cend(), std::ostream_iterator<_ull>(out, ", "));
    return out;

}

inline auto _longer(const BigUnsigned* const l, const BigUnsigned* const r) {

    return std::max(l, r, [](auto l, auto r) {
        return l->digits.size() < r->digits.size();
    });

}

inline auto _shorter(const BigUnsigned* const l, const BigUnsigned* const r) {

    return std::min(l, r, [](auto l, auto r) {
        return l->digits.size() <= r->digits.size();
    });

}

auto _longer_digits(const BigUnsigned* const l, const BigUnsigned* const r) {

    return std::max(l, r, [](auto l, auto r) {
        return l->digits.size() < r->digits.size();
    })->digits;

}

inline auto _shorter_digits(const BigUnsigned* const l, const BigUnsigned* const r) {

    return std::min(l, r, [](auto l, auto r) {
        return l->digits.size() <= r->digits.size();
    })->digits;

}

bool operator< (const BigUnsigned& l, const BigUnsigned& r) {

    _ui l_len = l.digits.size();
    _ui r_len = r.digits.size();
    if (l_len < r_len) {
         return true;
    } else if (l_len > r_len) {
        return false;
    } 

    auto l_iter = l.digits.cbegin();
    auto r_iter = r.digits.cbegin();
    while (l_iter != l.digits.cend() && *l_iter == *r_iter) {
        ++l_iter;
        ++r_iter;
    }

    return *l_iter < *r_iter;

}

bool operator> (const BigUnsigned& l, const BigUnsigned& r) {

    return r < l;

}

bool operator<= (const BigUnsigned& l, const BigUnsigned& r) {

    auto l_iter = std::find_if_not(l.digits.cbegin(), l.digits.cend(), [](auto i) {
        return i == 0;
    });
    auto r_iter = std::find_if_not(r.digits.cbegin(), r.digits.cend(), [](auto i) {
        return i == 0;
    });

    _ui l_len = l.digits.cend() - l_iter;
    _ui r_len = r.digits.cend() - r_iter;
    if (l_len > r_len) {
        return false;
    } else if (l_len < r_len) {
        return true;
    }

    while (l_iter != l.digits.cend() && *l_iter == *r_iter) {
        ++l_iter;
        ++r_iter;
    }
    
    return *l_iter <= *r_iter;

}

bool operator>= (const BigUnsigned& l, const BigUnsigned& r) {

    auto l_iter = std::find_if_not(l.digits.cbegin(), l.digits.cend(), [](auto i) {
        return i == 0;
    });
    auto r_iter = std::find_if_not(r.digits.cbegin(), r.digits.cend(), [](auto i) {
        return i == 0;
    });

    _ui l_len = l.digits.cend() - l_iter;
    _ui r_len = r.digits.cend() - r_iter;
    if (l_len > r_len) {
        return true;
    } else if (l_len < r_len) {
        return false;
    }

    while (l_iter != l.digits.cend() && *l_iter == *r_iter) {
        ++l_iter;
        ++r_iter;
    }

    return *l_iter >= *r_iter;

}

// theta(max(l.digits.size(), r.digits.size()))
BigUnsigned operator+ (const BigUnsigned& l, const BigUnsigned& r) {

    auto longer = ::_longer(&l, &r);
    auto shorter = ::_shorter(&l, &r);

    BigUnsigned result(longer->digits.size());
    auto new_digits = result.digits.rbegin();

    auto l_iter = longer->digits.crbegin();
    _ull carry = 0;
    for (auto s_iter = shorter->digits.crbegin(); s_iter != shorter->digits.crend(); ++l_iter, ++s_iter, ++new_digits) {
        _ull result = *l_iter + *s_iter + carry;
        carry = result >> BASE_BIN_LENGTH;
        *new_digits = result - (BASE * carry);
    }

    for (; l_iter != longer->digits.crend(); ++l_iter, ++new_digits) {
        _ull result = *l_iter + carry;
        carry = result >> BASE_BIN_LENGTH;
        *new_digits = result - (BASE * carry);
    }
    
    if (carry != 0) {
        result.digits.emplace(result.digits.cbegin(), carry);
    }

    return result;

}

// theta(max(l.digits.size(), r.digits.size()))
// note: doesnt care about order, the shorter number will be subtracted
// from the longer one, and if same length then smaller from larger
BigUnsigned operator- (const BigUnsigned& l, const BigUnsigned& r) {

    auto longer = ::_longer(&l, &r);
    auto shorter = ::_shorter(&l, &r);

    BigUnsigned result(longer->digits.size());
    auto new_digits = result.digits.rbegin();

    if (l.digits.size() == r.digits.size()) { // same size
        auto l_iter = l.digits.crbegin();
        auto r_iter = r.digits.crbegin();

        if (l > r) {
            _ull carry = 0;
            for (; l_iter != l.digits.crend(); ++l_iter, ++r_iter, ++new_digits) {
                _ull result = (*l_iter - carry) - *r_iter;
                carry = (result >> BASE_BIN_LENGTH) & 1;
                *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
            }
        } else if (r > l) {
            _ull carry = 0;
            for (; l_iter != l.digits.crend(); ++l_iter, ++r_iter, ++new_digits) {
                _ull result = (*r_iter - carry) - *l_iter;
                carry = (result >> BASE_BIN_LENGTH) & 1;
                *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
            }
        } else {
            return BigUnsigned("0");
        }
    } else {
        auto l_iter = longer->digits.crbegin();
        _ull carry = 0;
        for (auto s_iter = shorter->digits.crbegin(); s_iter != shorter->digits.crend(); ++l_iter, ++s_iter, ++new_digits) {
            _ull result = (*l_iter - carry) - *s_iter;
            carry = (result >> BASE_BIN_LENGTH) & 1; // why is & 1 here?
            *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
        }

        // instead shift the carry 63 bits right, which will be 1
        // new_digits is just result - carry
        // shift carry after new_digits

        for (; l_iter != longer->digits.crend(); ++l_iter, ++new_digits) {
            _ull result = *l_iter - carry;
            carry = (result >> BASE_BIN_LENGTH) & 1;
            *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
        }
    }

    return result;

}

// theta(2 * l.digits.size() * r.digits.size()) pre slow
// i dont care enough to use a fast multiplication algorithm since i dont care enough
// if needed will implement
BigUnsigned operator* (const BigUnsigned& l, const BigUnsigned& r) {

    BigUnsigned res("0");

    for (auto r_iter = r.digits.crbegin(); r_iter != r.digits.crend(); ++r_iter) {
        _ui shift = std::distance(r.digits.crbegin(), r_iter);

        BigUnsigned curr(l.digits.size() + 1 + shift);
        auto curr_iter = curr.digits.rbegin();
        for (_ui i = 0; i != shift; ++i, ++curr_iter) {
            *curr_iter = 0;
        }
        _ull carry = 0;
        for (auto l_iter = l.digits.crbegin(); l_iter != l.digits.crend(); ++l_iter, ++curr_iter) {
            _ull result = *l_iter * *r_iter + carry;
            carry = result >> BASE_BIN_LENGTH;
            *curr_iter = result - (BASE * carry);
        }
        *curr_iter = carry;

        res = res + curr;
    }

    return res;

}

// Meant for positive only
// Always returns lowest integer
BigUnsigned divide_digit_temp(typename BigUnsigned::cont_ull::const_iterator start, typename BigUnsigned::cont_ull::const_iterator end, const _ull digit) {

    BigUnsigned res(std::distance(start, end));
    auto iter_res = res.digits.begin();

    _ull carry = 0;

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

    _ui m = d.digits.size() - 1;
    _ull a = *d.digits.cbegin();
    BigUnsigned q = divide_digit_temp(n.digits.cbegin(), n.digits.cbegin() + (n.digits.size() - d.digits.size() + 1), a);
    BigUnsigned r = d;

    while (r >= d) {
        r = n - (q * d);
        auto qn = q - (divide_digit_temp(r.digits.cbegin(), r.digits.cbegin() + (r.digits.size() - d.digits.size() + 1), a) + BigUnsigned("1")); // add 1 since r is technically negative
        auto sum = q + qn;
        q = divide_digit_temp(sum.digits.cbegin(), sum.digits.cend(), 2);
    }

    return q;

}