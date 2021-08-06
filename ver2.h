#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <list>
#include <stdlib.h>
#include <string>
#include <type_traits>

template<typename Val, typename Container = std::list<Val>>
class BigUnsigned {

private:

    template<typename Input>
    void _create(Input first, Input last) {
        std::copy(first, last, std::back_inserter(digits));
    }

public:

    using value_type = Val;
    using container = Container;
    using ptr_container = container*;
    using const_ptr_container = const container*;

    static_assert(std::is_same_v<value_type, typename container::value_type>, "value and container value type must be same");
    // assert iterator is atleast biderectional

    explicit BigUnsigned() : digits() {}

    template<unsigned int N>
    explicit BigUnsigned(const value_type(&arr)[N]) {
        _create(arr, arr + N);
    }

    // if valid iterator
    template<typename Input>
    explicit BigUnsigned(Input first, Input last) {
        _create(first, last);
    }

    explicit BigUnsigned(const std::string& s) {
        std::for_each(s.begin(), s.end(), [this](auto digit) {
            digits.push_back(static_cast<value_type>(digit - '0'));
        });
    }

    const_ptr_container getDigits() const {
        return const_cast<const_ptr_container>(&digits);
    }

    ptr_container getDigits() {
        return const_cast<ptr_container>(static_cast<const BigUnsigned&>(*this).getDigits());
    }

private:

    container digits;

};

template<typename Input>
BigUnsigned(Input, Input)->BigUnsigned<typename Input::value_type>;

template<typename Val, typename Container>
std::ostream& operator<< (std::ostream& out, const BigUnsigned<Val, Container>& t) {
    auto digits = t.getDigits();
    std::copy(digits->cbegin(), digits->cend(), static_cast<std::ostream_iterator<Val>>(out));
    return out;
}

template<typename Val, typename Container>
auto _longer(const BigUnsigned<Val, Container>* const l, const BigUnsigned<Val, Container>* const r) {
    return std::max(l, r, [](auto l, auto r) {
        return l->getDigits()->size() < r->getDigits()->size();
        });
}

template<typename Val, typename Container>
auto _shorter(const BigUnsigned<Val, Container>* const l, const BigUnsigned<Val, Container>* const r) {
    return std::min(l, r, [](auto l, auto r) {
        return l->getDigits()->size() <= r->getDigits()->size();
        });
}

template<typename Val, typename Container>
auto _longer_digits(const BigUnsigned<Val, Container>* const l, const BigUnsigned<Val, Container>* const r) {
    return ::_longer(l, r)->getDigits();
}

template<typename Val, typename Container>
auto _shorter_digits(const BigUnsigned<Val, Container>* const l, const BigUnsigned<Val, Container>* const r) {
    return ::_shorter(l, r)->getDigits();
}

template<typename Val, typename Container>
BigUnsigned<Val, Container> add(const BigUnsigned<Val, Container>* const l, const BigUnsigned<Val, Container>* const r) {

    auto longer = ::_longer_digits(l, r);
    auto shorter = ::_shorter_digits(l, r);

    auto l_iter = longer->crbegin();
    typename BigUnsigned<Val, Container>::value_type carry = 0;
    BigUnsigned<Val, Container> ret;

    // where digits overlap
    for (auto s_iter = shorter->crbegin(); s_iter != shorter->crend(); ++l_iter, ++s_iter) {
        // std::cout << "longer:" << *l_iter << ", shorter:" << *s_iter << " carry:" << carry << ", res:" << (*l_iter ^ *s_iter ^ carry) << std::endl;
        ret.getDigits()->push_front(*l_iter ^ *s_iter ^ carry);
        carry = (*l_iter & *s_iter) | ((*l_iter ^ *s_iter) & carry);
    }

    // carry over onto longer
    for (; l_iter != longer->crend(); ++l_iter) {
        ret.getDigits()->push_front(*l_iter ^ carry);
        carry = *l_iter & carry;
    }

    ret.getDigits()->push_front(carry);

    return ret;

}

template<typename Val, typename Container>
void add_modify(BigUnsigned<Val, Container>* const l, const BigUnsigned<Val, Container>* const r) {

    auto longer = ::_longer_digits(l, r);
    auto shorter = ::_shorter_digits(l, r);

    auto into_iter = l->getDigits()->rbegin();
    auto l_iter = longer->crbegin();
    typename BigUnsigned<Val, Container>::value_type carry = 0;

    // where digits overlap
    for (auto s_iter = shorter->crbegin(); s_iter != shorter->crend(); ++l_iter, ++s_iter, ++into_iter) {
        const auto temp = carry;
        carry = (*l_iter & *s_iter) | ((*l_iter ^ *s_iter) & carry);
        *into_iter = *l_iter ^ *s_iter ^ temp;
    }

    // carry over onto longer
    for (; l_iter != longer->crend(); ++l_iter, ++into_iter) {
        const auto temp = carry;
        carry = *l_iter & carry;
        *into_iter = *l_iter ^ temp;
    }

    l->getDigits()->push_front(carry);
}

template<typename Val, typename Container>
BigUnsigned<Val, Container> operator+ (const BigUnsigned<Val, Container>& l, const BigUnsigned<Val, Container>& r) {

    return ::add(&l, &r);

}

template<typename Val, typename Container>
BigUnsigned<Val, Container>& operator+= (BigUnsigned<Val, Container>& l, const BigUnsigned<Val, Container>& r) {

    ::add_modify(&l, &r);
    return l;

}