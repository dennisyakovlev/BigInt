// Author: Dennis Yakovlev
#pragma once
#include <any>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <iterator>
#include <climits>
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
                                                   // Ex: 9999999999 doesnt fit into base 2^32 but 999999999 does


_ui log_const(double num) {
    _ui res = 0;
    while (num > 0) {
        num /= 2.0;
        num = std::floor(num);
        ++res;
    }
    return res;
}
static const _ui DOUBLE_CONVERSION = (log_const(DBL_MAX) / log_const(static_cast<double>(BASE))) - 1; // if the number of digits in the container is less than or
                                                                                                      // or equal to this then a conversion can be used using double

// ty must be integral and unsigned type
template<typename ty, typename = std::enable_if_t<std::is_integral_v<ty> && std::is_unsigned_v<ty>>>
struct _integral_unsigned {};

class BigUnsigned {
    // call resize_to_fit to ensure leading zeros are gone is using sz_ty_ull constructor
    // since all operations assume there are NO leading zeros
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

    // convert from base 10 to desired base
    void _create(cont_ui::iterator digit_local_iter, sz_ty_ui size_local) {

        std::list<_ull> digits_local; // to not have to preallocate required size

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

        resize_to_fit();

    }

public:

    explicit BigUnsigned() : digits() {}

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

    explicit BigUnsigned(sz_ty_ull size) : digits(size, 0) {} // initialize to 0's to allow remove of extra ones

    // remove the leading 0's
    // if all 0's results in empty digits
    void resize_to_fit() {

        auto iter = std::find_if_not(digits.cbegin(), digits.cend(), [](auto i) {
            return i == 0;
        });

        if (iter != digits.cbegin()) {
            cont_ull temp(iter, digits.cend());
            digits = std::move(temp);
        }

    }

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

inline auto _longer_digits(const BigUnsigned* const l, const BigUnsigned* const r) {

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

    _ui l_len = l.digits.size();
    _ui r_len = r.digits.size();
    if (l_len > r_len) {
        return false;
    } else if (l_len < r_len) {
        return true;
    }

    auto l_iter = l.digits.cbegin();
    auto r_iter = r.digits.cbegin();
    while (*l_iter == *r_iter && l_iter != l.digits.cend()) {
        ++l_iter;
        ++r_iter;
    }

    return (*l_iter <= *r_iter) || (l_iter == l.digits.cend());

}

bool operator>= (const BigUnsigned& l, const BigUnsigned& r) {

    _ui l_len = l.digits.size();
    _ui r_len = r.digits.size();
    if (l_len > r_len) {
        return true;
    } else if (l_len < r_len) {
        return false;
    }

    auto l_iter = l.digits.cbegin();
    auto r_iter = r.digits.cbegin();
    while (*l_iter == *r_iter && l_iter != l.digits.cend()) {
        ++l_iter;
        ++r_iter;
    }

    return (*l_iter >= *r_iter) || (l_iter == l.digits.cend());

}

// theta(max(l.size, r.size))
BigUnsigned operator+ (const BigUnsigned& l, const BigUnsigned& r) {

    auto longer = ::_longer(&l, &r);
    auto shorter = ::_shorter(&l, &r);

    BigUnsigned result(longer->digits.size() + 1);
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
        // result.digits.emplace(result.digits.cbegin(), carry);
        result.digits.emplace(std::find_if_not(result.digits.cbegin(), result.digits.cend(), [](auto val) {
            return val == 0;
        }), carry);
    }

    result.resize_to_fit();

    return result;

}

// theta(max(l.size, r.size))
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
            for (; l_iter != l.digits.crend(); ++l_iter, ++r_iter, ++new_digits) { // issue is crend includes the 0's need to get rid of the 0's somehow
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
            carry = (result >> BASE_BIN_LENGTH) & 1; // if carry is needed, then & 1 will always produce 1
            *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
        }

        for (; l_iter != longer->digits.crend(); ++l_iter, ++new_digits) {
            _ull result = *l_iter - carry;
            carry = (result >> BASE_BIN_LENGTH) & 1;
            *new_digits = (result << BASE_BIN_LENGTH) >> BASE_BIN_LENGTH;
        }
    }

    result.resize_to_fit();

    return result;

}

// theta(2 * l.size * r.size) really slow
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
BigUnsigned divide_digit(typename BigUnsigned::cont_ull::const_iterator start, typename BigUnsigned::cont_ull::const_iterator end, const _ull digit) {

    BigUnsigned res(std::distance(start, end));
    auto iter_res = res.digits.begin();

    _ull carry = 0;

    for (; start != end; ++start, ++iter_res) {
        *iter_res = (*start + (carry * BASE)) / digit;
        carry = *start - (digit * *iter_res); 
    }

    res.resize_to_fit();

    return res;

}

BigUnsigned divide(const BigUnsigned* const remainder, const BigUnsigned* const denom, const _ull digit) {

    return divide_digit(remainder->digits.cbegin(), remainder->digits.cbegin() + (remainder->digits.size() - denom->digits.size() + 1), digit);

}

// Meant for negative only
BigUnsigned divide_digit_neg(typename BigUnsigned::cont_ull::const_iterator start, typename BigUnsigned::cont_ull::const_iterator end, const _ull digit) {

    BigUnsigned res(std::distance(start, end));
    auto iter_res = res.digits.begin();

    _ull carry = 0;

    for (; start != end - 1; ++start, ++iter_res) {
        *iter_res = (*start + (carry * BASE)) / digit;
        carry = *start - (digit * *iter_res); 
    }
    
    *iter_res = (*start + digit + (carry * BASE)) / digit; // add digit since when negative integers rounded down is the same as rounding up when positive

    res.resize_to_fit();

    // std::cout << "res: " << res << std::endl;

    return res;

}

BigUnsigned divide_neg(const BigUnsigned* const remainder, const BigUnsigned* const denom, const _ull digit) {

    // std::cout << "hmm2 " << (remainder->digits.size() - denom->digits.size() + 1) << std::endl;
    return divide_digit_neg(remainder->digits.cbegin(), remainder->digits.cbegin() + (remainder->digits.size() - denom->digits.size() + 1), digit);

}

// issue when there is a 1 digit in numerator

// theta(c * n.size * operator*) slow
// c is some constant which is < 1
// i dont care enough to use a fast division algorithm since they all require decimals
// if needed will implement
// Note: To use n >= d
BigUnsigned operator/ (const BigUnsigned& n, const BigUnsigned& d) {

    _ui m = d.digits.size() - 1;
    _ull a = *d.digits.cbegin();
    BigUnsigned q = divide(&n, &d, a);
    q.resize_to_fit();
    BigUnsigned r = d;

    if (n > q * d) { // division is complete
        return q;
    }

    while (r >= d && r.digits.size() > 0) {
        // std::cout << "u" << std::endl;
        r = n - (q * d);     
        if (r.digits.size() >= d.digits.size()) {
            // std::cout << r << std::endl;
            // std::cout << (r.digits.size()) << " " << (d.digits.size()) << std::endl;

            auto qn = q - divide_neg(&r, &d, a); 
            auto sum = q + qn;
            q = divide_digit(sum.digits.cbegin(), sum.digits.cend(), 2);
        }
    }

    // std::cout << "done" << std::endl;


    return q;

}

BigUnsigned pow(const BigUnsigned& base, const BigUnsigned& pow) {

    BigUnsigned res("1");
    auto base_copy = base;
    auto pow_copy = pow;

    while (pow_copy.digits.size() != 0) {
        if (*pow_copy.digits.crbegin() % 2 == 1) {
            res = base_copy * res;
        }
        pow_copy = pow_copy / BigUnsigned("2");
        base_copy = base_copy * base_copy;
    }

    return res;

}

template<typename T>
T _add_10(T* l, T* r) {

    auto longer = std::max(l, r, [](auto l, auto r) {
        return l->size() < r->size();
    });
    
    auto shorter = std::min(l, r, [](auto l, auto r) {
        return l->size() <= r->size();
    });

    T ret(longer->size());
    auto ret_iter = ret.rbegin();
    auto l_iter = longer->crbegin();
    typename T::value_type carry = 0;
    for (auto s_iter = shorter->crbegin(); s_iter != shorter->crend(); ++l_iter, ++s_iter, ++ret_iter) {
        *ret_iter = (carry + *l_iter + *s_iter) % 10;
        carry = (carry + *l_iter + *s_iter) >= 10 ? 1 : 0; 

    }

    for (; l_iter != longer->crend(); ++l_iter, ++ret_iter) {

        *ret_iter = (carry + *l_iter) % 10;
        carry = (carry + *l_iter) >= 10 ? 1 : 0; 

    }

    if (carry != 0) {
        ret.insert(ret.cbegin(), carry);
    }

    return ret;

}

template<typename T>
T _mult_10(T* l, T* r) {

    T ret(1, 0);
    for (auto iter_r = r->rbegin(); iter_r != r->rend(); ++iter_r) {
        _ui shift = std::distance(r->rbegin(), iter_r);

        T curr(l->size() + 1 + shift);
        auto curr_iter = curr.rbegin();
        for (_ui i = 0; i != shift; ++i, ++curr_iter) {
            *curr_iter = 0;
        }

        typename T::value_type carry = 0;
        for (auto iter_l = l->crbegin(); iter_l != l->crend(); ++iter_l, ++curr_iter) {
            *curr_iter = (*iter_l * *iter_r + carry) % 10;
            carry = ((*iter_l * *iter_r + carry) - *curr_iter) / 10;
        }
        *curr_iter = carry;

        ret = _add_10(&ret, &curr);
    }

    return ret;

}

// return base 10 container representation of a bigunsigned
// with most signficant digits in the container being lower index
BigUnsigned::cont_ull BigUnsigned_10(const BigUnsigned& num) {

    BigUnsigned::cont_ull base_vec(BASE_DIGITS);
    BigUnsigned::cont_ull res(1, 0);

    auto base = BASE;
    for (auto iter = base_vec.rbegin(); iter != base_vec.rend(); ++iter) {
        *iter = base % 10;
        base /= 10;
    }
    BigUnsigned::cont_ull power_curr_vec{1};

    using value_ty = BigUnsigned::cont_ull::value_type;

    for (auto iter = num.digits.crbegin(); iter != num.digits.crend(); ++iter) {

        BigUnsigned::cont_ull digit_cont;
        BigUnsigned::cont_ull::value_type digit = *iter;
        while (digit > 0) {
            digit_cont.insert(digit_cont.begin(), digit % 10);
            digit /= 10;
        }

        auto temp = _mult_10(&digit_cont, &power_curr_vec);
        res = _add_10(&res, &temp);

        power_curr_vec = _mult_10(&power_curr_vec, &base_vec);

    }

    auto iter = std::find_if_not(res.cbegin(), res.cend(), [](auto i) {
            return i == 0;
    });

    if (iter != res.cbegin()) {
        return BigUnsigned::cont_ull(iter, res.cend());
    }

    return res;

}

// return base 10 double representation of a bigunsigned if possible
// if not, return -1
double BigUnsigned_10_dbl(const BigUnsigned& num) {
    // if -1 is returned then use BigUnsigned_10
    // Note: double is not accurate, use other function for guarnteed accuracy

    if (num.digits.size() > DOUBLE_CONVERSION) {
        return -1;
    }

    double res = 0.0;
    double i = 0;
    for (auto iter = num.digits.rbegin(); iter != num.digits.rend(); ++iter) {
        res += pow(static_cast<double>(BASE), i) * static_cast<double>(*iter);
        ++i;
    }

    return res;

}