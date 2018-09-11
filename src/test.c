/*
 * test.c
 *
 * Copyright (c) 2018 Felix Baumann
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 */

#include "global.h"

#ifdef JSON

/* This function compares two char arrays on equality.
 * Returns 1 if equal and 0 else.
 */
int equal_chars(char* first, char* second)
{
    if (strlen(first) != strlen(second)) { return 0; }
    
    int max = strlen(first);
    
    for (int i = 0; i < max; i++)
    {
        if (first[i] != second[i]) { return 0; }
    }
    
    return 1;
}

void test_equal_chars()
{
    assert(equal_chars("", ""));
    assert(equal_chars("string", "string"));
    assert(equal_chars(" \n", " \n"));

    assert(!equal_chars(" ", ""));
    assert(!equal_chars("string", "strin"));
    assert(!equal_chars("tring", "string"));
    assert(!equal_chars(" \n", " "));
}

/* Main function responsible for tests.
 * Global.
 */
void test()
{
    test_equal_chars();
    
    test_amiga();
    
    test_cdaccess();
    
    test_vpc();
    
    test_json();
    
    test_string();
    
    /* call tests here */
}

#endif

/* EOF */