/*
This is a simple C program demonstrating multiple solutions to a question:

    "Write a function that checks whether an integer number is a power of 10."

It contains a set of tests.

Author  : Adam Sawicki, http://asawicki.info, 2017-10-04
License : Public Domain
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int IsLog10_v1(int32_t x)
{
    // Convert x to string.
    char buf[12];
    itoa(x, buf, 10);
    const size_t bufLen = strlen(buf);
    
    // Check if string contains '1' followed by '0'-s.
    if(buf[0] != '1')
        return 0;
    for(size_t i = 1; i < bufLen; ++i)
    {
        if(buf[i] != '0')
            return 0;
    }
    return 1;
}

int IsLog10_v2(int32_t x)
{
    // Convert x to double. Calculate log10 of it.
    double x_d = (double)x;
    double x_log10 = log10(x_d);
    // Check if result is integer number - has zero fractional part.
    return x_log10 - floor(x_log10) == 0.0;
}

int IsLog10_v3(int32_t x)
{
    if(x <= 0)
        return 0;
    if(x == 1)
        return 1;
    if(x % 10 != 0)
        return 0;
    // Recursion.
    return IsLog10_v3(x / 10);
}

int IsLog10_v4(int32_t x)
{
    // Same algorithm as v3, but converted from recursion to a loop.
    if(x <= 0)
        return 0;
    for(;;)
    {
        if(x == 1)
            return 1;
        if(x % 10 != 0)
            return 0;
        x /= 10;
    }
}

int IsLog10_v5(int32_t x)
{
    // Just enumerate all possible values.
    return
        x == 1 ||
        x == 10 ||
        x == 100 ||
        x == 1000 ||
        x == 10000 ||
        x == 100000 ||
        x == 1000000 ||
        x == 10000000 ||
        x == 100000000 ||
        x == 1000000000;
}

int Test(int32_t x)
{
    // Choose version to test.

    //return IsLog10_v1(x);
    //return IsLog10_v2(x);
    //return IsLog10_v3(x);
    //return IsLog10_v4(x);
    return IsLog10_v5(x);
}

int main()
{
    const int32_t trueList[] = {
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
    };
    const size_t trueCount = sizeof(trueList) / sizeof(trueList[0]);

    const int32_t falseList[] = {
        0,
        2,
        3,
        5,
        9,
        11,
        14,
        19,
        20,
        80,
        99,
        101,
        128,
        255,
        256,
        999999999,
        1000000001,
        0x40000000,
        0x40000001,
        0x7FFFFFFE,
        0x7FFFFFFF,
        -1,
        -2,
        -5,
        -10,
        -100,
        -999,
        -1000,
        -1001,
        -999999999,
        -1000000000,
        -1000000001,
        0x80000000,
    };
    const size_t falseCount = sizeof(falseList) / sizeof(falseList[0]);

    int allPassed = 1;
    for(size_t i = 0; i < trueCount; ++i)
    {
        if(!Test(trueList[i]))
        {
            printf("%d: returned false, should return true.\n", trueList[i]);
            allPassed = 0;
        }
    }
    for(size_t i = 0; i < falseCount; ++i)
    {
        if(Test(falseList[i]))
        {
            printf("%d: returned true, should return false.\n", falseList[i]);
            allPassed = 0;
        }
    }
    if(allPassed)
        printf("All tests passed.\n");
}
