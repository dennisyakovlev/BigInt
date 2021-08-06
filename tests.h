#pragma once
#include <ver5.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using ull = unsigned long long;
using str = std::string;
using vec = std::vector<ull>;

// NUMBERS

// str num_NAME = base_10
// vec arr_NAME {base_4294967296}

str num_zero_middle = "26068034834506726580338427"; // contains middle 0 digit
vec arr_zero_middle {1413151, 0, 4636411};

str num_zero_end = "453907968712728283077682545561698304"; // contains ending zeros
vec arr_zero_end {5729124, 74612415, 0, 0};

str num_zero_middle_multiple = "83486096457596424475837168416422833316731280789991957049663134837133759163975347"; // two sets of consecutive digits of 0 in middle
vec arr_zero_middle_multiple {721,4358,0,0,15552,0,0,0,2934451};

str num_nine_nine = "340282366659884258995919063107230991601683319295"; // all digits are 999999999
vec arr_nine_nine {999999999, 999999999, 999999999, 999999999, 999999999};

str num_nine_digits = "1032589967263367277216600145372538202202426932138083224628049008519386424572149042885"; // 9 digits long
vec arr_nine_digits {8917621,23991234,23598235,23432552,15552,9891402,98174899,146899004,489230021};

str num_normal_1 = "728228107535740352153541163931469376797745899525714533927"; // just numbers
vec arr_normal_1 {498273891, 1471, 19874891, 90091414, 81927498, 917298727};
str num_normal_2 = "98274890100012835608621800200000296356829";
vec arr_normal_2 {288, 3452918988, 2857142638, 3877503930, 3951201245};
str num_normal_3 = "4682732413442";
vec arr_normal_3 {1090, 1218060802};

str num_zero = "0"; // zero
vec arr_zero {0};

str num_one = "1"; // one
vec arr_one {1};

// ------------------------------------------------------------------------------------------------

// test_INT is function names for testing
// int identifies the test

using id_ty = unsigned int;
using result_ty = bool;
std::map<id_ty, result_ty> test_results;
std::map<id_ty, str> messages {
    {1, "normal number conversion test"},
    {2, "number can be converted when all the digits are filled to the maximum number that can be stored in BASE_DIGITS_ALLOW"},
    {3, "number can be converted when the number of resulting digits is the same as BASE_DIGITS_ALLOW"},
    {4, "number can be converted when multiple consecutive zeros exist in the new base digits"},
    {5, "number can be converted when consecutive zeros exist at end of the new base digits"},

    {6, "two numbers can be added"},
    {7, "two numbers with digits maxed out can be added"},
    {8, "two numbers with zero digits throughout can be added"},
    {9, "test for commutativity and associativity of addition"},
    {10, "test number plus zero is number"},
    {11, "multiple numbers can be added"},

    {12, "two numbers can be subtracted"},
    {13, "the same number subtracted equals zero"},
    {14, "two numbers with zero digits throughout can be subtracted"},

    {15, "two numbers can be multiplied"},
    {16, "two numbers with zero digits throughout can be multiplied"}
};

result_ty compare_test(BigUnsigned num, vec num_vec) {
    auto iter_v = num_vec.cbegin();
    auto iter_n = std::find_if_not(num.digits.cbegin(), num.digits.cend(), [](auto i) {
        return i == 0;
    });
    result_ty res = true;
    for (; iter_v != num_vec.cend() && iter_n != num.digits.cend(); ++iter_v, ++iter_n) {
        res = res && (*iter_v == *iter_n);
    }
    res = res && (iter_v == num_vec.cend() && iter_n == num.digits.cend());
    return res;
}

result_ty compare_test_zero(BigUnsigned num) {
    auto iter_n = std::find_if_not(num.digits.cbegin(), num.digits.cend(), [](auto i) {
        return i == 0;
    });
    return iter_n == num.digits.cend();
}

void insert_to_result(id_ty id, result_ty res) {
    test_results.insert_or_assign(id, res);
}

// instantiation tests

void test_1() {

    BigUnsigned num_1(num_normal_1);
    BigUnsigned num_2(num_normal_2);
    BigUnsigned num_3(num_normal_3);

    result_ty res = compare_test(num_1, arr_normal_1) && compare_test(num_2, arr_normal_2) && compare_test(num_3, arr_normal_3);

    insert_to_result(1, res);

}

void test_2() {

    BigUnsigned num(num_nine_nine);

    insert_to_result(2, compare_test(num, arr_nine_nine));

}

void test_3() {

    BigUnsigned num(num_nine_digits);

    insert_to_result(3, compare_test(num, arr_nine_digits));

}

void test_4() {

    BigUnsigned num(num_zero_middle_multiple);

    insert_to_result(4, compare_test(num, arr_zero_middle_multiple));

}

void test_5() {

    BigUnsigned num(num_zero_end);

    insert_to_result(5, compare_test(num, arr_zero_end));

}

// addition tests

void test_6() {

    BigUnsigned num = BigUnsigned(num_normal_1) + BigUnsigned(num_normal_2);
    vec res_vec = {498273891, 1759, 3472793879, 2947234052, 3959431429, 573532676};
    
    insert_to_result(6, compare_test(num, res_vec));

}

void test_7() {

    BigUnsigned num = BigUnsigned(num_nine_nine) + BigUnsigned(num_nine_nine);
    vec res_vec = {1999999998, 1999999998, 1999999998, 1999999998, 1999999998};
    
    insert_to_result(7, compare_test(num, res_vec));

}

void test_8() {

    BigUnsigned num_1 = BigUnsigned(num_zero_middle_multiple) + BigUnsigned(num_zero_end);
    vec res_vec_1 = {721, 4358, 0, 0, 15552, 5729124, 74612415, 0, 2934451};
    
    BigUnsigned num_2 = BigUnsigned(num_normal_3) + BigUnsigned(num_zero_middle);
    vec res_vec_2 = {1413151, 1090, 1222697213};

    result_ty res = compare_test(num_1, res_vec_1) && compare_test(num_2, res_vec_2);
    
    insert_to_result(8, res);

}

void test_9() {

    BigUnsigned num_1 = (BigUnsigned(num_normal_1) + BigUnsigned(num_normal_2)) + BigUnsigned(num_normal_3);
    BigUnsigned num_2 = BigUnsigned(num_normal_1) + (BigUnsigned(num_normal_2) + BigUnsigned(num_normal_3));
    BigUnsigned num_3 = (BigUnsigned(num_normal_2) + BigUnsigned(num_normal_1)) + BigUnsigned(num_normal_3);
    BigUnsigned num_4 = (BigUnsigned(num_normal_3) + BigUnsigned(num_normal_1)) + BigUnsigned(num_normal_2);

    vec res_vec = {498273891, 1759, 3472793879, 2947234052, 3959432519, 1791593478};

    result_ty res = compare_test(num_1, res_vec) && compare_test(num_2, res_vec) && compare_test(num_3, res_vec) && compare_test(num_4, res_vec);

    insert_to_result(9, res);
}

void test_10() {

    BigUnsigned num = BigUnsigned(num_normal_3) + BigUnsigned(num_zero);

    insert_to_result(10, compare_test(num, arr_normal_3));

}

void test_11() {

    BigUnsigned num("999999999");

    auto res = num + num + num + num + num + num + num + num + num + num + num + num;

    insert_to_result(11, compare_test(res, {2, 3410065396}));

}

// subtraction tests

void test_12() {

    BigUnsigned num = BigUnsigned(num_normal_1) - BigUnsigned(num_normal_2);
    vec res_vec = {498273891, 1182, 861923198, 1527916071, 499390863, 1261064778};
    
    insert_to_result(12, compare_test(num, res_vec));

}

void test_13() {

    BigUnsigned num = BigUnsigned(num_nine_nine) - BigUnsigned(num_nine_nine);
    
    insert_to_result(13, compare_test_zero(num));

}

void test_14() {

    BigUnsigned num_1 = BigUnsigned(num_zero_end) - BigUnsigned(num_zero_middle);
    vec res_vec_1 = {5729124, 73199263, 4294967295, 4290330885};
    
    BigUnsigned num_2 = BigUnsigned(num_zero_middle_multiple) - BigUnsigned(num_nine_nine);
    vec res_vec_2 = {721, 4357, 4294967295, 4294967295, 3294982848, 3294967296, 3294967296, 3294967296, 3297901748};

    BigUnsigned num_3 = BigUnsigned(num_nine_digits) - BigUnsigned(num_zero_middle_multiple);
    vec res_vec_3 = {8916900, 23986876, 23598235, 23432552, 0, 9891402, 98174899, 146899004, 486295570};

    result_ty res = compare_test(num_1, res_vec_1) && compare_test(num_2, res_vec_2) && compare_test(num_3, res_vec_3);

    insert_to_result(14, res);

}

// multiplication tests

void test_15() {

    BigUnsigned num_1 = BigUnsigned(num_normal_1) * BigUnsigned(num_normal_2);
    vec res_vec_1 = {33, 2169544818, 11554571, 2759726770, 2691999543, 3775539610, 3781763666, 3557598463, 1613100007, 4139836877, 2896753835};
    
    BigUnsigned num_2 = BigUnsigned(num_normal_1) * BigUnsigned(num_normal_3);
    vec res_vec_2 = {126, 2093973306, 2197142442, 960508599, 1429993573, 980322833, 1538751069, 4266715726};

    result_ty res = compare_test(num_1, res_vec_1) && compare_test(num_2, res_vec_2);

    insert_to_result(15, res);

}

void test_16() {

    BigUnsigned num_1 = BigUnsigned(num_zero_middle_multiple) * BigUnsigned(num_zero_middle_multiple);
    vec res_vec_1 = {519841, 6284236, 18992164, 0, 22425984, 135551232, 0, 1, 178375755, 4101838436, 0, 21, 1078850688, 0, 0, 2004, 3888210217};
    
    BigUnsigned num_2 = BigUnsigned(num_zero_end) * BigUnsigned(num_zero_middle);
    vec res_vec_2 = {1885, 103981313, 1456726345, 2495856043, 4270720837, 0, 0};

    result_ty res = compare_test(num_1, res_vec_1) && compare_test(num_2, res_vec_2);

    insert_to_result(16, res);

}

// division tests

// ------------------------------------------------------------------------------------------------

void run_tests(std::string file_name = "error.log") {

    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();
    test_7();
    test_8();
    test_9();
    test_10();
    test_11();
    test_12();
    test_13();
    test_14();
    test_15();
    test_16();

    std::fstream outf;
    outf.open(file_name, std::ios::out);

    for (auto item : test_results) {
        if (!item.second) {
            outf << "Failed test for\n" << (messages.find(item.first))->second << std::endl;
        }
    }

}