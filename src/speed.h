#pragma once
#include <ver5.h> // include latest working version
                  // change to BigInt.h
#include <functional>
#include <Windows.h>

#ifndef SIZE_TEST_USER // user defined test for size
#define SIZE_TEST 10000 // number of base_10 digits to test
#endif

#ifdef SIZE_TEST_USER
#define SIZE_TEST SIZE_TEST_USER
#endif

using arr_elem_ty = unsigned int;
using arr_sz_ty = unsigned int;
using num_lg = BigUnsigned; 

// return time taken in seconds
template<typename... Args, typename... Args2>
double time(std::function<void(Args...)> func, Args2... args) {

    LARGE_INTEGER frequency; // ticks per second
    LARGE_INTEGER t1, t2; // ticks
    LARGE_INTEGER elapsedMicroSecond;
    QueryPerformanceFrequency(&frequency); // get ticks per second
    QueryPerformanceCounter(&t1); // start timer

    func(args...);

    QueryPerformanceCounter(&t2); // stop timer
    // compute and print the elapsed time in seconds
    elapsedMicroSecond.QuadPart = (t2.QuadPart - t1.QuadPart);
    elapsedMicroSecond.QuadPart *= 1000000;
    elapsedMicroSecond.QuadPart /= frequency.QuadPart;

    return elapsedMicroSecond.QuadPart / 1000000.0;

}

template<arr_sz_ty sz>
void init_time(arr_elem_ty(&arr)[sz]) {

    num_lg num(arr);

}

// call to check speed on current hardware
void speed_test() {

    const arr_sz_ty size = SIZE_TEST;
    arr_elem_ty a[size];
    arr_elem_ty b[size];

    *a = 9; // gaurentee a > b
    *b = 1;
    for (arr_sz_ty i = 1; i != size; ++i) {
        *(a + i) = i % 7;
        *(b + i) = i % 3;
    }

    num_lg num_1(a);
    num_lg num_2(b);

    num_lg* adr_1 = std::addressof(num_1);
    num_lg* adr_2 = std::addressof(num_2);

    // std::function<void(arr_elem_ty(&)[size])> func2 = init_time<size>;
    auto t_init = time(std::function(init_time<size>), std::reference_wrapper(a));

    auto t_add = time(std::function([](num_lg* a, num_lg* b){auto res = *a + *b;}), adr_1, adr_2);
    auto t_sub = time(std::function([](num_lg* a, num_lg* b){auto res = *a - *b;}), adr_1, adr_2);
    auto t_mul = time(std::function([](num_lg* a, num_lg* b){auto res = *a * *b;}), adr_1, adr_2);
    auto t_div = time(std::function([](num_lg* a, num_lg* b){auto res = *a / *b;}), adr_1, adr_2);

    std::cout << "All measurement are for " << size << " base 10 digits" << '\n' <<
    "Init time: " << t_init << "s" << '\n' <<
    "Add time: " << t_add << "s" << '\n' <<
    "Subtract time: " << t_sub << "s" << '\n' <<
    "Multiply time: " << t_mul << "s" << '\n' <<
    "Divide time: " << t_div << "s" << '\n' << std::endl;

}