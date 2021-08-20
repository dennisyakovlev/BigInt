# BigUnsignedInt library

## Contents

* [Aim](#Project-aim)
* [Usage](#Usage)
* [Algorithms](#Algorithms)

## Project aim

C++ library for larger than natively supported unsigned integer operations. Meant to be easily integrated into any project and operating system easily. Not a replacement for the GNU MP library.

## Usage

Download files and include BigInt.h. Compile along with project.  
To check if working, include test.h and run "run_tests". Tests are for 64-bit operating systems.  
To check speed of operations, include speed.h and run "speed_test". After approximately 1000 digits operations operations become slow.

**Note**: Requires c++17 or higher.

## Algorithms

All algorithms were created from scratch by me, they are not the most efficient out there. The library is meant to work with numbers that are less than 1000 digits long. The following algorithms work fine for that use case.

### Initialization

Convert the base 10 input into a much larger base to store less digits. That base is 2 to the power of half the bit length of ```unsigned long long int```.  
repeatedly divide the input by the base storing the remainder everytime as the most significant digit. Iteratively do this until the input is less than the base.  
A much more efficient algorithm would be a radix conversion.

### Addition

Same as gradeschool addition. If a carry is necessary, then the bit more significant of the maximum base bit will be a one.  
There are ways to optimize the bitwise operations for certain hardware.

Ex: If the base is 256 the maximum digit (255) has bit length 8. Adding two numbers of bit length less than or equal to 8 results in a maximum bit length of 9. If the most significant bit of the 9 is 1, then a carry is needed.  
<p>(85)<sub>256</sub> + (205)<sub>256</sub> = (<strong>1</strong>00100010)<sub>2</sub> = carry + (34)<sub>256</sub></p>
<p>(27)<sub>256</sub> + (43)<sub>256</sub> = (<strong>0</strong>01000110)<sub>70</sub> = (70)<sub>256</sub></p>

### Subtraction

Same as gradeschool subtraction. If a carry is necessary, then the most significant bit of the ```unsigned long long int``` will be a 1 since unsigned subtraction wraps around<sup>§</sup>.   
There are ways to optimize the bitwise operations for certain hardware.

### Multiplication

Same as gradeschool multiplication. If a carry is necessary, then the more significant half of the digits of the ```unsigned long long int``` will be the carry and the less significant half of the digits will be the result.  
A much more efficient algorithm would be some variation of Karatsuba's algorithm.

### Division

The equation *n*/*d* = *q* + *r* will be true of any integer division. Where *n* is the numerator, *d* denominator, *q* quotient, and *r* remainder. Using this an iterative process can be applied using the leading digit of *d* to find *q*.  
The closer *d* is to *n* the less iterations are required.  
A much more efficient algorithm would implement the Newton–Raphson method.

<sup>§</sup> From 6.9.1-4 of the c++17 standard "Unsigned integers shall obey the laws of arithmetic modulo 2<sup>*n*</sup> where *n* is the number of bits in the value representation of that particular size integer"
