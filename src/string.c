/*
 * string.c
 * 
 * A 'String' is supposed to be an array of chars of dynamic size.
 * 
 * Use initialize_String() to create one and afterwards 
 * insert_String() to append chars as often as needed.
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

/* This function initializes a String.
 * It MUST BE CALLED before the first usage of any String structure.
 * 
 * *STR is a pointer to the String structure 
 *      which is supposed to be initialized.
 *
 * INIT_SIZE is the initial size of the string and has to be at least 1.
 */
void initialize_String(String *str, size_t init_size)
{
    // Make sure the pointer actually points somewhere.
    assert(str != NULL);
    
    // Make sure there's enough space for the ending character '\0'.
    assert(init_size > 0);
    
    // Allocate memory.
    str->string = (char *)malloc(init_size * sizeof(char));
    
    str->string[0] = '\0';
    str->used_size = 1;
    str->total_size = init_size;
}


/* Appends a single char to the string. 
 * If the string isn't large enough, expands it.
 * Makes sure, that the char array always ends with a '\0'
 * and that there aren't any '\0' elsewhere.
 * It's not possible to add one explicitly.
 * 
 * Note that the String has to be initialized first using
 * 'initialize_string'.
 * 
 * *STR pointer to the String
 * 
 * INSERT char which should be appended to the String pointed at
 * 
 */
void insert_single_char(String *str, char element)
{
    /* Checks */
    assert(str != NULL);
    if (element == '\0') {return;}

    /* If the String is entirely filled, expand it. */
    if(str->used_size == str->total_size)
    {
        str->total_size *= 1.5;
        str->string = (char *)realloc(str->string,
                                      str->total_size * sizeof(char));
    }
    
    /* Replace the '\0' at the end by the new char. */
    str->string[str->used_size-1] = element;
    
    /* Append a new ending char '\0'. */
    str->string[str->used_size] = '\0';

    str->used_size++;
}


/* Appends several chars to the string. 
 * Note that the String has to be initialized first.
 * 
 * *STR pointer to the String
 * 
 * APPEND char array which should be appended to the String pointed at
 */
void insert_chars(String *str, char append[])
{
    for (int i = 0; i < strlen(append); i++)
    {
        insert_single_char(str, append[i]);
    }
}


/* Appends a string to another string. 
 * Note that both Strings have to be initialized first.
 * 
 * STR pointer to the String where something shall be appended
 * 
 * APPEND pointer to the String which should be appended
 */
void insert_string(String *str, String *append)
{
    for (int i = 0; i < append->used_size; i++)
    {
        insert_single_char(str, append->string[i]);
    }
}


/* Extracts chars from a String to a char array.
 *
 * *STR pointer to the String to extract chars from
 * 
 * CHARS the char array where the chars will be stored in.
 * 
 * Make sure, that CHARS is large enough 
 * and that STR points to an initialized String.
 */
void extract_chars(String *str, char chars[])
{
    /* An empty string is not supposed to alter a char array. */
    if (str->string[0] == '\0') { return; }

    strncpy(chars, str->string, str->used_size);
}


/* Remove the string,
 * i.e. delete the references and free the memory.
 * 
 * *STR pointer to the String to be removed.
 */
void free_String(String *str)
{
    free(str->string);
    str->string = NULL;
    str->used_size = str->total_size = 0;
}



// -----------------------------------------------------------
//                             TESTS
// -----------------------------------------------------------

void test_initialize_String()
{
    String str;
    initialize_String(&str, 4);

    /* The string already contains  '\0'. */
    assert(str.used_size == 1);
    assert(str.total_size == 4);
}

void test_insert_single_char()
{
    
    String str;
    initialize_String(&str, 3); 
    insert_single_char(&str, 'a');
    
    assert(str.used_size == 2);
    assert(str.string[0] == 'a');
    assert(str.string[1] == '\0');
    
    insert_single_char(&str, ' ');
    
    assert(str.used_size == 3);
    assert(str.string[1] == ' ');
    assert(str.string[2] == '\0');
    
    insert_single_char(&str, '\0');
    
    assert(str.used_size == 3);
    assert(str.total_size == 3);
    
    insert_single_char(&str, 'b');
    insert_single_char(&str, 'c');
    
    assert(str.used_size == 5);
    assert(str.total_size == 6);
}

void test_insert_chars()
{    
    String str;
    initialize_String(&str, 5);
    insert_chars(&str, "hel");

    assert(str.used_size == 4);
    assert(str.string[0] == 'h');
    assert(str.string[1] == 'e');
    assert(str.string[2] == 'l');
    assert(str.string[3] == '\0');
    
    /* Break the initial size of the string */
    insert_chars(&str, "lo world!");

    assert(str.string[3] == 'l');
    assert(str.string[5] == ' ');
    assert(str.string[8] == 'r');
    assert(str.string[11] == '!');
    assert(str.string[12] == '\0');
}

void test_insert_String()
{
    String str;
    initialize_String(&str, 2);
    insert_chars(&str, " ");
    
    String insert;
    initialize_String(&insert, 6);
    insert_chars(&insert, "hello");

    insert_string(&str, &insert);

    assert(str.used_size == 7);
    assert(str.total_size == 9);
    assert(str.string[0] == ' ');
    assert(str.string[1] == 'h');
    assert(str.string[2] == 'e');
    assert(str.string[3] == 'l');
    assert(str.string[4] == 'l');
    assert(str.string[5] == 'o');
    assert(str.string[6] == '\0');
}

void test_extract_chars()
{
    String str;
    initialize_String(&str, 6);
    char x[] = "abcd";
    
    /* An empty string won't harm a char array. */
    extract_chars(&str, x);
    
    assert(x[0] == 'a');
    assert(x[1] == 'b');
    assert(x[2] == 'c');
    assert(x[3] == 'd');
    assert(x[4] == '\0');
    
    insert_chars(&str, "hello");
    
    /* A non-empty string will overwrite a char array. */
    extract_chars(&str, x);
    
    assert(x[0] == 'h');
    assert(x[1] == 'e');
    assert(x[2] == 'l');
    assert(x[3] == 'l');
    assert(x[4] == 'o');
    assert(x[5] == '\0');   
}

/* Main function responsible for string tests. */
void test_string()
{
    test_initialize_String();

    test_insert_single_char();

    test_insert_chars();

    test_insert_String();

    test_extract_chars();
}

#endif

/* EOF */
