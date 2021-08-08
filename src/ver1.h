// Author: Dennis Yakovlev
#pragma once
#include <vars.h> // a missing local file
#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <numeric>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <iostream>

start_probability 

// only positive integers
class BigUnsigned {
public:
    using cont = std::list<int>; // must be signed type
    using cont_val = cont::value_type;
    using cont_size = cont::size_type;
    using const_ptr = const BigUnsigned* const;
    using const_cont_ptr = const cont* const;
    using cont_ptr = cont* const;

    explicit BigUnsigned() : digits() {}

    explicit BigUnsigned(const std::string& str) : digits() { // each digit of string is digit in number
        std::for_each(str.cbegin(), str.cend(), [&, this](std::string::value_type digit) {
            digits.push_back(static_cast<cont_val>(digit - '0'));
        });
    }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    explicit BigUnsigned(const T num) {
        T _num = num;
        for (T mod = 10; mod <= num * 10; mod *= 10) {
            digits.push_front((_num % mod) / (mod / 10));
        }
    }

    const_cont_ptr getDigits() const {
        return &digits;
    }

    cont_ptr getDigits() {
        return const_cast<cont_ptr>(static_cast<const BigUnsigned&>(*this).getDigits());
    }

private:

    cont digits;

};

class _BigIntPair {
public:
    using pair_ty = std::pair<BigUnsigned::const_ptr, BigUnsigned::const_ptr>;

    _BigIntPair() = delete;
    _BigIntPair(BigUnsigned::const_ptr a, BigUnsigned::const_ptr b) : pair{a, b} {}

    BigUnsigned::const_ptr longer() {
        return std::max(pair.first, pair.second, [](auto l, auto r) {
            return l->getDigits()->size() < r->getDigits()->size();
        }); 
    }

    BigUnsigned::const_ptr shorter() {
        return std::min(pair.first, pair.second, [](auto l, auto r) {
            return l->getDigits()->size() <= r->getDigits()->size();
        });
    }

    BigUnsigned::const_cont_ptr longerDigits() {
        return longer()->getDigits();
    }

    BigUnsigned::const_cont_ptr shorterDigits() {
        return shorter()->getDigits();
    }

private:

    pair_ty pair;

};

BigUnsigned exp(const BigUnsigned&, const BigUnsigned&);

bool operator== (const BigUnsigned& l, const BigUnsigned& r) {
    auto l_iter = l.getDigits()->crbegin();
    auto r_iter = r.getDigits()->crbegin();
    for (; l_iter != l.getDigits()->crend() && r_iter != r.getDigits()->crend(); ++l_iter, ++r_iter) {
        if (*l_iter != *r_iter) {
            return false;
        }
    }

    return l_iter == l.getDigits()->crend() && r_iter == r.getDigits()->crend();
}

bool operator!= (const BigUnsigned& l, const BigUnsigned& r) {
    return !(l == r);
}

std::ostream& operator<< (std::ostream& out, const BigUnsigned& big) {
    std::copy(big.getDigits()->cbegin(), big.getDigits()->cend(), std::ostream_iterator<BigUnsigned::cont_val>(out, ""));
    return out;
}

bool operator<= (const BigUnsigned& l, const BigUnsigned& r) {
    BigUnsigned::cont_size l_len = l.getDigits()->size();
    BigUnsigned::cont_size r_len = r.getDigits()->size();
    if (l_len < r_len) {
         return true;
    } else if (l_len > r_len) {
        return false;
    } 

    auto l_iter = l.getDigits()->cbegin();
    auto r_iter = r.getDigits()->cbegin();
    for (; l_iter != l.getDigits()->cend() && *l_iter == *r_iter; ++l_iter, ++r_iter) {}
    return *l_iter <= *r_iter;
}

bool operator>= (const BigUnsigned& l, const BigUnsigned& r) {
    BigUnsigned::cont_size l_len = l.getDigits()->size();
    BigUnsigned::cont_size r_len = r.getDigits()->size();
    if (l_len > r_len) {
         return true;
    } else if (l_len < r_len) {
        return false;
    } 

    auto l_iter = l.getDigits()->cbegin();
    auto r_iter = r.getDigits()->cbegin();
    for (; l_iter != l.getDigits()->cend() && *l_iter == *r_iter; ++l_iter, ++r_iter) {}
    return *l_iter >= *r_iter;
}

bool operator< (const BigUnsigned& l, const BigUnsigned& r) {
    BigUnsigned::cont_size l_len = l.getDigits()->size();
    BigUnsigned::cont_size r_len = r.getDigits()->size();
    if (l_len < r_len) {
         return true;
    } else if (l_len > r_len) {
        return false;
    } 

    auto l_iter = l.getDigits()->cbegin();
    auto r_iter = r.getDigits()->cbegin();
    for (; l_iter != l.getDigits()->cend() && *l_iter == *r_iter; ++l_iter, ++r_iter) {}
    return *l_iter < *r_iter;
}

bool operator> (const BigUnsigned& l, const BigUnsigned& r) {
    return r < l;
}

BigUnsigned operator+ (const BigUnsigned& l, const BigUnsigned& r) {

    _BigIntPair pair{&l, &r};

    auto l_iter = pair.longerDigits()->crbegin();
    BigUnsigned::cont_val carry = 0;
    BigUnsigned ret;

    // where digits overlap
    for (auto s_iter = pair.shorterDigits()->crbegin(); s_iter != pair.shorterDigits()->crend(); ++l_iter, ++s_iter) {
        const auto sum = *l_iter + *s_iter + carry;
        carry = sum >= 10 ? 1 : 0;
        ret.getDigits()->push_front(sum % 10);
    }

    // carry over onto longer
    for (; l_iter != pair.longerDigits()->crend() && carry != 0; ++l_iter) {
        const auto sum = *l_iter + carry;
        carry = sum >= 10 ? 1 : 0;
        ret.getDigits()->push_front(sum % 10);
    }

    // if carry is not zero then the full length of the longer number has been added, nothing to copy
    if (carry != 0) {
        ret.getDigits()->push_front(carry);
    } else {
        std::copy(l_iter, pair.longerDigits()->crend(), std::front_inserter(*ret.getDigits()));
    }

    return ret;
}

BigUnsigned& operator+= (BigUnsigned& l, const BigUnsigned& r) {
    l = l + r;
    return l;
}

BigUnsigned operator- (const BigUnsigned& l, const BigUnsigned& r) {

    _BigIntPair pair(&l, &r);

    BigUnsigned ret;
    bool carry = false;
    auto l_iter = l.getDigits()->crbegin();

    // where digits overlap
    for (auto r_iter = r.getDigits()->crbegin(); r_iter != r.getDigits()->crend(); ++l_iter, ++r_iter) {
        const auto carry_int = carry ? 1 : 0;
        const auto diff = *l_iter - *r_iter;
        if (*l_iter - carry_int < *r_iter) {
            if (*l_iter == 0) {
                ret.getDigits()->push_front(diff + 9);
            } else {
                ret.getDigits()->push_front(diff + 10 - carry_int);
            }
            carry = true;
        } else {
            ret.getDigits()->push_front(diff - carry_int);
            carry = false;
        }
    }

    // carry over onto longer
    for (; l_iter != l.getDigits()->crend() && carry; ++l_iter) {
        if (*l_iter == 0) {
            ret.getDigits()->push_front(9);
            carry = true;
        } else {
            ret.getDigits()->push_front(*l_iter - 1);
            carry = false;
        }
    }

    // copy remaining digits
    std::copy(l_iter, l.getDigits()->crend(), std::front_inserter(*ret.getDigits()));

    // delete leading zeros
    auto end_0 = std::find_if(ret.getDigits()->cbegin(), ret.getDigits()->cend(), [](auto elem) {
        return elem != 0;
    });
    if (end_0 == ret.getDigits()->cend()) {
        if (*--ret.getDigits()->cend() == 0) {
            end_0 = --ret.getDigits()->cend();
        } else {
            end_0 = ret.getDigits()->cbegin();
        }
    }

    ret.getDigits()->erase(ret.getDigits()->begin(), end_0);

    return ret;
}

BigUnsigned& operator-= (BigUnsigned& l, const BigUnsigned& r) {
    l = l - r;
    return l;
}

BigUnsigned operator* (const BigUnsigned& l, const BigUnsigned& r) {

    _BigIntPair pair{&l, &r};

    std::vector<BigUnsigned> bigVec;
    BigUnsigned::cont_size pos = 0;
    for (auto r_iter = r.getDigits()->crbegin(); r_iter != r.getDigits()->crend(); ++r_iter) {
        BigUnsigned big;
        BigUnsigned::cont_val carry = 0;
        for (auto l_iter = l.getDigits()->crbegin(); l_iter != l.getDigits()->crend(); ++l_iter) {
            const auto total = *l_iter * *r_iter + carry;
            const auto digit = total % 10;
            carry = (total - digit) / 10;
            big.getDigits()->push_front(digit);
        }
        if (carry != 0) {
            big.getDigits()->push_front(carry);
        }
        big.getDigits()->insert(big.getDigits()->end(), pos, 0);
        bigVec.push_back(big);
        ++pos;
    }

    return std::accumulate(bigVec.begin(), bigVec.end(), BigUnsigned("0"));
}

BigUnsigned& operator *= (BigUnsigned& l, const BigUnsigned& r) {
    l = l * r;
    return l;
}

// BigUnsigned operator% (const BigUnsigned& l, const BigUnsigned& r) {
//     return l - ((l / r) * r);
// }

// should prolly fix this later
BigUnsigned operator/ (const BigUnsigned& l, const BigUnsigned& r) {

    _BigIntPair pair(&l, &r);

    BigUnsigned r_1("0");
    auto iter = r.getDigits()->cbegin();
    for (BigUnsigned::cont_size i = 0; i != r.getDigits()->size() - 1; ++i) {
        r_1 += static_cast<BigUnsigned>(*iter) * exp(static_cast<BigUnsigned>(10), static_cast<BigUnsigned>(r.getDigits()->size() - 2 - i));
        std::cout << i << std::endl;
    }

    std::cout << r_1 << std::endl;
    // need expoenent function
    // allow conversion from native type to bigint and vice versa

    // auto start_iter = l.getDigits()->cbegin();
    // std::advance(start_iter, r.getDigits()->size() - 1);
    // for (BigUnsigned::cont_size i = 0; i != l.getDigits()->size() - r.getDigits()->size() + 1; ++i) {
    //     BigUnsigned::cont_size a = *start_iter;
    // }
}

// should prolly fix this later
BigUnsigned exp(const BigUnsigned& base, const BigUnsigned& pow) {
    BigUnsigned res("1");
    for (BigUnsigned j("0"); j != pow; j += BigUnsigned("1")) {
        res *= base;
    }
    return res;
}

end_probability