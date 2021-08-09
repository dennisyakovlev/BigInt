// Author: Dennis Yakovlev
#pragma once
#include <ver5.h> // include latest working version
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

// test_X_Y is function names for testing
// X identifies the message if the test fails
// Y identifies the test number for that failure message

using id_ty = unsigned int;
using result_ty = bool;

// to identifiy test
struct Test_Info {
    Test_Info(id_ty a, id_ty b) : message_id(a), test_id(b) {}
    id_ty message_id;
    id_ty test_id;
};

bool operator== (const Test_Info& l, const Test_Info& r) {
    return (l.message_id == r.message_id) && (l.test_id == r.test_id);
}

bool operator!= (const Test_Info& l, const Test_Info& r) {
    return !(l == r);
}

// messages upon test failure
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
    {16, "two numbers with zero digits throughout can be multiplied"},

    {17, "two numbers can be divided"}
};

// information for failed test
struct Fail_Info {
    Fail_Info(const vec& expected_loc, const vec& got_loc) : expected(expected_loc), got(got_loc) {}
    vec expected;
    vec got;
};

struct Info {
    Info (const Test_Info& t_info_loc, const Fail_Info& f_info_loc) : t_info(t_info_loc), f_info(f_info_loc) {}
    Test_Info t_info;
    Fail_Info f_info;
};

bool operator== (const Info& l, const Info& r) {
    return l.t_info == r.t_info;
}

bool operator!= (const Info& l, const Info& r) {
    return !(l == r);
}

// tests which resulted in failures
std::vector<Info> failures;

// check whether num is equal to num_vec
// allow for leading zeros in num
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

// check whether num is equal to 0
// means going through all the 0 digits results in being
// at the of the digits container
result_ty compare_test_zero(BigUnsigned num) {

    auto iter_n = std::find_if_not(num.digits.cbegin(), num.digits.cend(), [](auto i) {
        return i == 0;
    });
    return iter_n == num.digits.cend();

}

// insert test result as needed
void insert_result(Test_Info info, BigUnsigned result, vec expected) {
    // info - test info
    // result - resultant number from test
    // expected - expected result vector from test

    auto is_same = compare_test(result, expected);

    if (!is_same) {
        vec result_vec;
        std::copy(result.digits.cbegin(), result.digits.cend(), std::back_inserter(result_vec));
        failures.push_back(Info(info, Fail_Info(expected, result_vec)));
    }

}

// instantiation tests

void test_1_1() {

    BigUnsigned num_1(num_normal_1);
    vec vec_1 = arr_normal_1;

    insert_result(Test_Info(1, 1), num_1, vec_1);

}

void test_1_2() {

    BigUnsigned num_1(num_normal_2);
    vec vec_1 = arr_normal_2;

    insert_result(Test_Info(1, 2), num_1, vec_1);

}

void test_1_3() {

    BigUnsigned num_1(num_normal_3);
    vec vec_1 = arr_normal_3;

    insert_result(Test_Info(1, 3), num_1, vec_1);

}

void test_2_1() {

    BigUnsigned num_1(num_nine_nine);
    vec vec_1 = arr_nine_nine;

    insert_result(Test_Info(2, 1), num_1, vec_1);

}

void test_3_1() {

    BigUnsigned num_1(num_nine_digits);
    vec vec_1 = arr_nine_digits;

    insert_result(Test_Info(3, 1), num_1, vec_1);

}

void test_4_1() {

    BigUnsigned num_1(num_zero_middle_multiple);
    vec vec_1 = arr_zero_middle_multiple;

    insert_result(Test_Info(4, 1), num_1, vec_1);

}

void test_5_1() {

    BigUnsigned num_1(num_zero_end);
    vec vec_1 = arr_zero_end;

    insert_result(Test_Info(5, 1), num_1, vec_1);

}

// addition tests

void test_6_1() {

    BigUnsigned num_1 = BigUnsigned(num_normal_1) + BigUnsigned(num_normal_2);
    vec vec_1 = {498273891, 1759, 3472793879, 2947234052, 3959431429, 573532676};

    insert_result(Test_Info(6, 1), num_1, vec_1);

}

void test_7_1() {

    BigUnsigned num_1 = BigUnsigned(num_nine_nine) + BigUnsigned(num_nine_nine);
    vec vec_1 = {1999999998, 1999999998, 1999999998, 1999999998, 1999999998};

    insert_result(Test_Info(7, 1), num_1, vec_1);

}

void test_8_1() {

    BigUnsigned num_1 = BigUnsigned(num_zero_middle_multiple) + BigUnsigned(num_zero_end);
    vec vec_1 = {721, 4358, 0, 0, 15552, 5729124, 74612415, 0, 2934451};

    insert_result(Test_Info(8, 1), num_1, vec_1);

}

void test_8_2() {

    BigUnsigned num_1 = BigUnsigned(num_normal_3) + BigUnsigned(num_zero_middle);
    vec vec_1 = {1413151, 1090, 1222697213};

    insert_result(Test_Info(8, 2), num_1, vec_1);

}

void test_9_1() {

    BigUnsigned num_1 = (BigUnsigned(num_normal_1) + BigUnsigned(num_normal_2)) + BigUnsigned(num_normal_3);
    vec vec_1 = {498273891, 1759, 3472793879, 2947234052, 3959432519, 1791593478};

    insert_result(Test_Info(9, 1), num_1, vec_1);

}

void test_9_2() {

    BigUnsigned num_1 = BigUnsigned(num_normal_1) + (BigUnsigned(num_normal_2) + BigUnsigned(num_normal_3));
    vec vec_1 = {498273891, 1759, 3472793879, 2947234052, 3959432519, 1791593478};

    insert_result(Test_Info(9, 2), num_1, vec_1);

}

void test_9_3() {

    BigUnsigned num_1 = (BigUnsigned(num_normal_2) + BigUnsigned(num_normal_1)) + BigUnsigned(num_normal_3);
    vec vec_1 = {498273891, 1759, 3472793879, 2947234052, 3959432519, 1791593478};

    insert_result(Test_Info(9, 3), num_1, vec_1);

}

void test_9_4() {

    BigUnsigned num_1 = (BigUnsigned(num_normal_3) + BigUnsigned(num_normal_1)) + BigUnsigned(num_normal_2);
    vec vec_1 = {498273891, 1759, 3472793879, 2947234052, 3959432519, 1791593478};

    insert_result(Test_Info(9, 4), num_1, vec_1);

}

void test_10_1() {

    BigUnsigned num_1 = BigUnsigned(num_normal_3) + BigUnsigned(num_zero);
    vec vec_1 = arr_normal_3;

    insert_result(Test_Info(10, 1), num_1, vec_1);

}

void test_11_1() {

    BigUnsigned num("999999999");
    auto res = num + num + num + num + num + num + num + num + num + num + num + num;
    vec vec_1 = {2, 3410065396};

    insert_result(Test_Info(11, 1), res, vec_1);

}

// subtraction tests

void test_12_1() {

    BigUnsigned num_1 = BigUnsigned(num_normal_1) - BigUnsigned(num_normal_2);
    vec vec_1 = {498273891, 1182, 861923198, 1527916071, 499390863, 1261064778};

    insert_result(Test_Info(12, 1), num_1, vec_1);

}

void test_13_1() {

    BigUnsigned num_1 = BigUnsigned(num_nine_nine) - BigUnsigned(num_nine_nine);
    vec vec_1 = {0};

    insert_result(Test_Info(13, 1), num_1, vec_1);

}

void test_14_1() {

    BigUnsigned num_1 = BigUnsigned(num_zero_end) - BigUnsigned(num_zero_middle);
    vec vec_1 = {5729124, 73199263, 4294967295, 4290330885};

    insert_result(Test_Info(14, 1), num_1, vec_1);

}

void test_14_2() {

    BigUnsigned num_1 = BigUnsigned(num_zero_middle_multiple) - BigUnsigned(num_nine_nine);
    vec vec_1 = {721, 4357, 4294967295, 4294967295, 3294982848, 3294967296, 3294967296, 3294967296, 3297901748};

    insert_result(Test_Info(14, 2), num_1, vec_1);

}

void test_14_3() {

    BigUnsigned num_1 = BigUnsigned(num_nine_digits) - BigUnsigned(num_zero_middle_multiple);
    vec vec_1 = {8916900, 23986876, 23598235, 23432552, 0, 9891402, 98174899, 146899004, 486295570};

    insert_result(Test_Info(14, 3), num_1, vec_1);

}

// multiplication tests

void test_15_1() {

    BigUnsigned num_1 = BigUnsigned(num_normal_1) * BigUnsigned(num_normal_2);
    vec vec_1 = {33, 2169544818, 11554571, 2759726770, 2691999543, 3775539610, 3781763666, 3557598463, 1613100007, 4139836877, 2896753835};

    insert_result(Test_Info(15, 1), num_1, vec_1);

}

void test_15_2() {

    BigUnsigned num_1 = BigUnsigned(num_normal_1) * BigUnsigned(num_normal_3);
    vec vec_1 = {126, 2093973306, 2197142442, 960508599, 1429993573, 980322833, 1538751069, 4266715726};

    insert_result(Test_Info(15, 2), num_1, vec_1);

}

void test_16_1() {

    BigUnsigned num_1 = BigUnsigned(num_zero_middle_multiple) * BigUnsigned(num_zero_middle_multiple);
    vec vec_1 = {519841, 6284236, 18992164, 0, 22425984, 135551232, 0, 1, 178375755, 4101838436, 0, 21, 1078850688, 0, 0, 2004, 3888210217};

    insert_result(Test_Info(16, 1), num_1, vec_1);

}

void test_16_2() {

    BigUnsigned num_1 = BigUnsigned(num_zero_end) * BigUnsigned(num_zero_middle);
    vec vec_1 = {1885, 103981313, 1456726345, 2495856043, 4270720837, 0, 0};

    insert_result(Test_Info(16, 2), num_1, vec_1);

}

// division tests

void test_17_1() {

    BigUnsigned num_1 = BigUnsigned("19874981700985638096112098738936281572098713508956") / BigUnsigned("187678164781867");
    vec vec_1 = {1336636, 3328194501, 223895323, 1792054288};

    insert_result(Test_Info(17, 1), num_1, vec_1);

}

void test_17_2() {

    BigUnsigned num_1 = BigUnsigned("19874981700985638096112098738936281572098713508956") / BigUnsigned("15879472941867");
    vec vec_1 = {15797598, 1868090674, 1134720860, 486845289};

    insert_result(Test_Info(17, 2), num_1, vec_1);

}

void test_17_3() {

    BigUnsigned num_1 = BigUnsigned("19874981700985638096") / BigUnsigned("187678164781867");
    vec vec_1 = {105899};

    insert_result(Test_Info(17, 3), num_1, vec_1);

}

void test_17_4() {

    BigUnsigned num_1 = BigUnsigned("19874981700985638096") / BigUnsigned("15879472941867");
    vec vec_1 = {1251614};

    insert_result(Test_Info(17, 4), num_1, vec_1);

}

// ------------------------------------------------------------------------------------------------

// run all tests and output to file_name
void run_tests(std::string file_name = "error.log") {

    test_1_1();
    test_1_2();
    test_1_3();
    test_2_1();
    test_3_1();
    test_4_1();
    test_5_1();
    test_6_1();
    test_7_1();
    test_8_1();
    test_8_2();
    test_9_1();
    test_9_2();
    test_9_3();
    test_9_4();
    test_10_1();
    test_11_1();
    test_12_1();
    test_13_1();
    test_14_1();
    test_14_2();
    test_14_3();
    test_15_1();
    test_15_2();
    test_16_1();
    test_16_2();
    test_17_1();
    test_17_2();
    test_17_3();
    test_17_4();

    std::fstream outf;
    outf.open(file_name, std::ios::out);

    const id_ty num_test = 29;

    outf << "Passed " << (num_test - failures.size()) << " out of " << num_test << " tests" << std::endl;

    for (auto item : failures) {
        outf << "\n\nFAILED TEST FOR\n" << (messages.find(item.t_info.message_id))->second << std::endl; // get fail message
        outf << "find in function name test_" << item.t_info.message_id << "_" << item.t_info.test_id << std::endl; // tell which function to find failure

        outf << "Expected: "; // give expected result
        auto expected = item.f_info.expected;
        std::copy(expected.cbegin(), expected.cend(), std::ostream_iterator<vec::value_type>(outf, ","));
        
        outf << "\nActual:   "; // give actual result
        auto got = item.f_info.got;
        std::copy(got.cbegin(), got.cend(), std::ostream_iterator<vec::value_type>(outf, ","));
    }

    outf.close();

}