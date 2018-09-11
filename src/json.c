/*
 * json.c
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

#define MAX(A, B) (((A) > (B)) ? (A) : (B))

// ---------------------------------------------------------------------
// ARRANGE DATA
// ---------------------------------------------------------------------

/* Counter providing the next content object id. */
int id = 0;

/* Counter for the number of properties the latest content object has. */
int property_counter = 0;

char *clean_char(unsigned char value[]);

/* This function stores directory and name of the given file.
 * 
 * PATH is the current location of the file including it's name.
 */
void add_file_path(char path[])
{
    /* If the latin1 option is disabled clean_char() will just return 'path'. */
    char *u_path = clean_char((unsigned char *) path);
    
    int len = strlen(u_path);
    initialize_String(&given_file.path, len+1);

    /* Copy the string char by char including the ending char '\0'. */
    for (int i = 0; i < len+1; i++)
    {
        given_file.path.string[i] = u_path[i];
    }

    /* Note that the string is full. */
    given_file.path.used_size = len+1;
}


/* This function is supposed to store file kind and size of the given file.
 * 
 * FILE_KIND contains the kind of file handed to disktype.
 *           It's domain is:
 *           {Regular file, Block device, Character device, Unknown kind}
 * 
 * SIZE is the size of the file in bytes.
 * 
 */
void add_file_characteristics(char file_kind[], unsigned long long int *size)
{
    initialize_String(&given_file.file_kind, 3);
    insert_chars(&given_file.file_kind, file_kind);
    given_file.size = *size;
}


/* Local function that identifies the parent of an object using it's
 * level and the list of content objects.
 * If there's no parent, the function returns -1.
 *
 * LEVEL references the former indent of the print statement of this object.
 */
int identify_parent_id(int level)
{
  /* The first object can't have a parent. */
  if (id == 0 || level == 0) {return -1;}
 
  /* Iterate backwards through the content list */
  for (int i = id - 1; i > -1; i--)
  {
      /* the first object with a level lower than this one's is the parent. */
      if (given_file.content[i].level < level) 
      {
          return i;
      }
  }
  /* No parent has been found. */
  return -1;
}


/* Given a char array this function will clean it by creating a new array by
 * copying the given one and escaping illegal chars like quotation marks,
 * backslashs, control sequences and so on.
 * 
 * The cleaning is only performed if the latin1 assumption is given as command
 * line argument.
 * 
 * JSON requires UTF-8 strings only and also forbids quotation marks
 * inside a string.
 * 
 * VALUE    The char array that is supposed to be cleaned.
 */
char *clean_char(unsigned char value[])
{
    /* Define an array which will last longer than the execution of this function. 
     * Also, make sure it has at least a size of 1. 
     * This will store the output.
     * Escaping a char takes up to 7 chars (\\u0000) 
     * That says an array 7 times as large as the given one will definitely
     * be large enough to contain the cleaned one.
     */
    char* clean = malloc(MAX(1, ((int) strlen((char *) value) * 7)) 
                         * sizeof(char));
    
    /* If the latin1 assumption is deactivated, we can't clean anything. */
    if (!latin1)
    {
        return (char*) value;
    }
    
    /* Since we will escape illegal chars, the index of the clean (output)
     * array is not identical to the one of the input. */
    int clean_index = 0;
    
    /* Add only allowed chars to the output and escape the other ones. */
    for (int index = 0; value[index]; index++)
    {
        /* Escape percentage sign. 
         * This is required for the print statement. */
        if (value[index] == '%')
        {
            clean[clean_index] = '%';
            clean[clean_index+1] = '%';
            clean_index += 2;
            continue;
        }
        
        /* Escape backslash aka reverse solidus, quotes,
         * control sequences and forbidden chars. */
        if (value[index] == 0x5C || value[index] == 0x22
            || value[index] <= 0x1F || value[index] >= 0x80)
        {
            /* Escape syntax: \\u0000 with the zeros replaced by the unicode 
             *                        code point. 
             * There are two backslashs, since one is consumed by the print 
             * statement. 
             */

            // clean[clean_index] = '\\';
            // clean_index++;

            clean[clean_index] = '\\';
            clean_index++;
            clean[clean_index] = 'u';
            clean_index++;
            
            /* Convert the char to the hexadecimal representation
             * of it's value. */
            char hexa[5];
            sprintf(hexa, "%04X", value[index]);

            /* Add this hexadecimal representation which is 
             * the unicode code point. */
            clean[clean_index] = hexa[0];
            clean_index++;
            clean[clean_index] = hexa[1];
            clean_index++;
            clean[clean_index] = hexa[2];
            clean_index++;
            clean[clean_index] = hexa[3];
            clean_index++;
            continue;
        }

        /* It's an ordinary char. Just keep it. */
        clean[clean_index] = value[index];
        clean_index++;
    }

  /* Finish the char array. */
  clean[clean_index] = '\0';
  return clean;
}


/* This function allows to add a new content object 
 * (file system / boot loader / partition / ...)
 * from anywhere in the code.
 *
 * LEVEL references the former indent of the print statement of this object.
 *
 * OBJECT_TYPE refers to the name of this object's type. 
 *             Say 'FAT12', 'ReiserFS' or 'FreeBSD boot loader'...
 *
 * WIKIDATA contains the wikidata id of the object type.
 *
 */
void add_content_object(int level, char object_type[], char wikidata[])
{
  /* Create a new content object with the given values. */
  given_file.content[id].id = id;
  given_file.content[id].level = level;
  given_file.content[id].parent_id = identify_parent_id(level);
  
  /* object type */
  initialize_String(&given_file.content[id].object_type, 25);
  insert_chars(&given_file.content[id].object_type, object_type);
  
  /* wikidata */
  initialize_String(&given_file.content[id].wikidata, 25);
  insert_chars(&given_file.content[id].wikidata, wikidata);
  
  /* Reset property counter. */
  property_counter = 0;
  given_file.content[id].number_of_properties = 0;
  given_file.number_of_objects++;
  
  /* Increment content object counter for a new id for the next object. */
  id += 1;
}


/* This function allows to add a property to the latest content object
 * from anywhere in the code.
 * 
 * Obviously, there actually has to be an object in the content list.
 * Otherwise, there's nothing to assign the property to and an assertion 
 * will be violated.
 * 
 * The handed in char array doesn't have to be filled completely.
 * The function will only consider actual values, 
 * i.e. values before the first '\0' ending char.
 *
 * KEY the name of the property
 *
 * VALUE the value this property holds
 */
void add_property(char key[], char value[])
{
  /* Make sure, the object even exists. */
  assert(id > 0);

  /* There can't and won't be more than 100 properties for a single object. */
  assert(property_counter < 100);
  
  /* Make sure the property doesn't exist already. */
  for (int index = 0; index < property_counter; index++)
  {
      /* If there already is an identical key, 
       * we'll skip this one since we don't like duplicates.
       *
       * Note that strcmp stops comparing at \0 so the actual length of
       * the arrays don't matter*/
      if (strcmp(given_file.content[id-1].properties[index].key.string, 
          key) == 0)
      { 
          return;
      }
  }

  char *clean_value = clean_char((unsigned char *) value);

  /* id-1 is the latest content object, where this property belongs to */
  
  /* Add property key */
  initialize_String(&given_file.content[id-1]
                    .properties[property_counter].key, 4);
  insert_chars(&given_file.content[id-1]
               .properties[property_counter].key, key);

  /* Add property value */
  initialize_String(&given_file.content[id-1]
                    .properties[property_counter].value, 4);
  insert_chars(&given_file.content[id-1]
               .properties[property_counter].value, clean_value);

  property_counter += 1;
  given_file.content[id-1].number_of_properties++;
}

/* This function allows to add a property to the latest content object
 * from anywhere in the code.
 * 
 * The value of the property is passed as an integer.
 * The conversion to a char array is done by the function.
 * See also "add_property()" which is used here.
 * 
 * KEY  the name of the property
 * 
 * VALUE the int value the property will hold
 */
void add_property_int(char key[], int value)
{
    /* Define a char array which will contain the given property value. */
    char value_as_chars[12];
    
    /* Copy the int value into the char array. */
    sprintf(value_as_chars, "%d", value);
    
    /* Add the property. */
    add_property(key, value_as_chars);
}

/* This function allows to add a property to the latest content object
 * from anywhere in the code.
 * 
 * The value of the property is passed as an u4 (long unsigned int).
 * The conversion to a char array is done by the function.
 * See also "add_property()" which is used here.
 * 
 * KEY  the name of the property
 * 
 * VALUE the u4 value the property will hold
 */
void add_property_u4(char key[], u4 value)
{
    /* Define a char array which will contain the given property value. */
    char value_as_chars[12];
    
    /* Copy the u4 value into the char array. */
    sprintf(value_as_chars, "%lu", value);
    
    /* Add the property. */
    add_property(key, value_as_chars);
}

/* This function allows to add a property to the latest content object
 * from anywhere in the code.
 * 
 * The value of the property is passed as an u8 (long long unsigned).
 * The conversion to a char array is done by the function.
 * See also "add_property()" which is used here.
 * 
 * KEY  the name of the property
 * 
 * VALUE the u8 value the property will hold
 */
void add_property_u8(char key[], u8 value)
{
    /* Define a char array which will contain the given property value. */
    char value_as_chars[21];
    
    /* Copy the u8 value into the char array. */
    sprintf(value_as_chars, "%llu", value);
    
    /* Add the property. */
    add_property(key, value_as_chars);
}

/* This function allows to add the endianness property
 * from anywhere in the code.
 * 
 * ENDIANNESS   0 for big-endian,
 *              anything else for little-endian
 */
void add_property_endianness(int endianness)
{
    add_property("endianness", (endianness) ? "little" : "big");
}


// ---------------------------------------------------------------------
// CONVERT TO JSON
// ---------------------------------------------------------------------

/* The String holding the output text while it's under construction */
String json;

/* The output text once it's finished */
char json_output[] = "";

/* Content objects within a content list are seperated by commas.
 * According to JSON there's no comma neither at the beginning nor
 * at the end of the list.
 * 
 * Therefore, whenever a new list is started, this has to be noted
 * in order to leave out the comma at the very beginning.
 */
//int new_content_list = 1;


/* Add file kind, path and size of the file to the json String. */
void add_file_characteristics_json()
{    
    /* The following statements will add text to the json String. */
    
    /* {"file kind": " */
    insert_chars(&json, "{\"file kind\": \"");

    /* <file_kind> */
    insert_string(&json, &given_file.file_kind);

    /* ", "path": " */
    insert_chars(&json, "\", \"path\": \"");

    /* <path> */
    insert_string(&json, &given_file.path);

    /* ", "size": " */
    insert_chars(&json, "\", \"size\": \"");
    
    /* Calculate number of digits of the size */
    int digits = floor(log10(given_file.size)) + 1;
    
    /* Create a String temporarily storing the size */
    String size;
    initialize_String(&size, 100);
    sprintf(size.string, "%llud", given_file.size);
    size.used_size = digits;
    size.total_size = digits;
    
    /* <size> */
    insert_string(&json, &size);
    
    /* ", "content": [ */
    insert_chars(&json, "\", \"content\": [");
}


/* Add one single property to the json String.
 * 
 * Properties are divided by commas.
 * Brackets are not included.
 * 
 * OBJ_ID the content object where this property belongs to
 * 
 * PROP_ID the id of this property
 */
void add_property_json(int obj_id, int prop_id)
{
    /* Seperate properties with commas */
    if (prop_id != 0)
    {
        insert_chars(&json, ", ");
    }

    /* Syntax: "key": "value" */
    insert_chars(&json, "\"");
    insert_string(&json, 
                  &given_file.content[obj_id].properties[prop_id].key);

    insert_chars(&json, "\": \"");
    insert_string(&json, 
                  &given_file.content[obj_id].properties[prop_id].value);
    insert_chars(&json, "\"");
    
}

/* Add one single content object along with its sub-objects 
 * and properties to the json String.
 * 
 * OBJ_ID the content object which shall be added.
 * 
 * Note that this function should only be called explicitely for 
 * top level (0) objects . The function then calls itself recursively
 * for the sub-objects.
 */
void add_obj_json(int obj_id, int first_in_a_list)
{
    /* add a comma to seperate the object from the one before */
    if (!first_in_a_list) 
    {
        insert_chars(&json, ", ");
    }

    /* type */
    insert_chars(&json, "{\"type\": \"");
    insert_string(&json, &given_file.content[obj_id].object_type);
    insert_chars(&json, "\",");

    /* wikidata */
    insert_chars(&json, " \"wikidata\": \"");
    insert_string(&json, &given_file.content[obj_id].wikidata);
    insert_chars(&json, "\",");

    /* properties */
    insert_chars(&json, " \"properties\": {");

    for (int i = 0; i < given_file.content[obj_id].number_of_properties; i++)
    {
        add_property_json(obj_id, i);
    }

    insert_chars(&json, "}, \"content\": [");

    /* We're starting a new (sub-)content list here. */
    int new_content_list = 1;
    
    /* content sub-objects */
    for (int x = 0; x < given_file.number_of_objects ; x++)
    {
        if (given_file.content[x].parent_id == obj_id)
        {
            /* add a sub-object 
               and inform it about being the first one or not */
            add_obj_json(x, new_content_list);

            /* after a first object, 
               there won't be another first one, you know... */
            new_content_list = 0;
        }
    }

    insert_chars(&json, "]}");
}

/* Once the file is analyzed, the structured data has to be converted 
 * to JSON. The intermediate result will be stored in 'String json'.
 * 
 * Afterwards, the char array in this string will be extracted
 * and stored in the global 'char[] json' for the main function to 'return'.
 */
void convert_to_json()
{
  initialize_String(&json, 1000);

  /* include filekind, path and size */
  add_file_characteristics_json();
  
  /* Consider all top level objects and add them and their sub-objects. */
  for (int i = 0; i < given_file.number_of_objects; i++)
  {
    /* a level of 0 indicates a top level object */
    if (given_file.content[i].level == 0)
    {
        /* This is the first object and therefore the first one in the list. */
        if (i == 0) 
        {
            add_obj_json(i, 1);
        }
        /* if it isn't the first object, it can't be the first one in
         * the highest level list and therefore will need a comma first
         * to seperate it from the one before */
        else
        {
            add_obj_json(i, 0);
        }
    }
  }

  /* Closing brackets for the whole file */
  insert_chars(&json, "]}");

  /* Extract char array. */
  extract_chars(&json, json_output);
}




// ---------------------------------------------------------------------
//                             RESET
// ---------------------------------------------------------------------

/* This function clears all variables storing detected data for json.
 * That is, given_file, json, json_output, property_counter and object id.
 * Calling reset creates an environment comparable to the beginning of the
 * programm's execution.
 * 
 * This allows to first run some tests and delete all test data afterwards
 * before performing the analysis of the actual file given.
 */
void reset_json()
{
    /* Delete json_output */
    memset(json_output, 0, sizeof(json_output));

    /* Delete json String*/
    free_String(&json);

    /* Reset property_counter and object counter */
    property_counter = 0;
    id = 0;

    /* Reset given_file */
    given_file = (const struct file_info) { 0 };
}


// -----------------------------------------------------------
//                             TESTS
// -----------------------------------------------------------

void test_add_file_path()
{
    char *path = "/some/imaginary/path/";
    add_file_path(path);
    char result[22];
    extract_chars(&given_file.path, result);
    
    assert(equal_chars(result, "/some/imaginary/path/"));
    
    reset_json();
    
    char *path_2 = "some\\windows\\path";
    
    /* Store latin1 temporarily here and make sure it's active 
     * for this test. Afterwards reset it to it's actual value. */
    int latin = latin1;
    
    latin1 = 1;
    
    add_file_path(path_2);
    char result_2[31];
    extract_chars(&given_file.path, result_2);
    
    // assert(equal_chars(result_2, "some\\\\u005Cwindows\\\\u005Cpath"));
    assert(equal_chars(result_2, "some\\u005Cwindows\\u005Cpath"));
    
    latin1 = latin;
    reset_json();
    
}

void test_add_file_characteristics()
{
    u8 s = 987654321;
    u8 *size = &s;
    char file_kind[] = "Regular file";
    
    add_file_characteristics(file_kind, size);
    
    char stored_kind[13];
    extract_chars(&given_file.file_kind, stored_kind);
    
    assert(equal_chars(stored_kind, file_kind));
    assert(given_file.size == *size);
    reset_json();
    
}

void test_identify_parent_id()
{
    // current object id is still 0
    assert(identify_parent_id(17) == -1);
    
    add_content_object(5, "some type", "Qxxx");
    
    // an object with level 0 can't have a parent
    assert(identify_parent_id(0) == -1);
    
    // the only other object has a level (5) that is not lower
    assert(identify_parent_id(5) == -1);
    
    // object 0 with level 5 is the parent
    assert(identify_parent_id(9) == 0);
    
    add_content_object(8, "some type", "Qxxx");
    
    // object 1 with level 8 is now the parent
    assert(identify_parent_id(9) == 1);
    
    // object 0 with level 5 is the parent
    assert(identify_parent_id(6) == 0);
    
    //int identify_parent_id(int level)
    reset_json();
}

void test_clean_char()
{
    /* If cleaning is deactivated, the tests would obviously fail. */
    if (!latin1) { return; }
    
    /* Test escaping a backslash. */
    unsigned char a[5] = "a\\bc";
    assert(strcmp("a\\u005Cbc", clean_char(a)) == 0);
    
    /* Test an already clean array. */
    assert(strcmp("abc", clean_char((unsigned char *) "abc")) == 0);
    
    /* Test escaping quotes. */
    assert(strcmp("a\\u0022bc", clean_char((unsigned char *) "a\"bc")) == 0);
    
    /* Test some character > 0x80. */
    unsigned char x[2];
    x[0] = (unsigned char) 255;
    x[1] = (unsigned char) '\0';
    assert(strcmp("\\u00FF", clean_char(x)) == 0);
    
    /* Empty string ok? */
    assert(strcmp("", clean_char((unsigned char *) "")) == 0);
    
    /* A percentage sign is escaped by using two of them. */
    assert(strcmp("%%", clean_char((unsigned char *) "%")) == 0);
    
    /* Test new line. */
    assert(strcmp("xyz\\u000A", clean_char((unsigned char *) "xyz\n")) == 0);
    
    /* Test carriage return. */
    assert(strcmp("abc\\u000Dxyz", clean_char((unsigned char *) "abc\rxyz")) == 0);
    
    /*
    char x[15];
    int i = 0;
    
    FILE * file;
    char c;
    file = fopen( "weird2", "r");
    if (file)
    {
        while((c = getc(file)) != EOF) {
            x[i] = c;
            i++;
        }
        fclose(file);


    //printf("%s\n", clean_char(x));
    assert(strcmp(clean_char(x), 
                  "\\\\u0022CD7BONA\\\\u000A\\\\u0022\\\\u000A") == 0);
    
    }
    */

}

void test_add_content_object()
{
    add_content_object(5, "some type", "Q1234567");
    
    assert(given_file.content[0].id == 0);
    assert(given_file.content[0].level == 5);
    assert(given_file.content[0].parent_id == -1);
    
    char type[10] = "some type";

    char stored_type[10];

    extract_chars(&given_file.content[0].object_type, stored_type);
    
    assert(strlen(stored_type) == strlen(type));
    assert(equal_chars(stored_type, type));
    
    char wikidata[] = "Q1234567";
    assert(wikidata[8] == '\0');

    char stored_wiki[9];
    extract_chars(&given_file.content[0].wikidata, stored_wiki);

    assert(equal_chars(stored_wiki, wikidata));
    assert(given_file.number_of_objects == 1);
    assert(given_file.content[0].number_of_properties == 0);

    add_content_object(7, "different type", "Qyyy");
    assert(given_file.content[1].id == 1);
    assert(given_file.content[1].parent_id == 0);

    reset_json();
}

void test_add_property()
{
    add_content_object(0, "FAT12", "Q3063042");
    
    assert(given_file.content[0].number_of_properties == 0);

    add_property("volume name", "my beautiful FAT12 volume");
    add_property("volume name", "a second volume name");
    
    char property_name[20] = "volume name";
    add_property(property_name, "a second volume name");    
    
    assert(given_file.content[0].number_of_properties == 1);
    
    char key[] = "volume name";
    char value[] = "my beautiful FAT12 volume";
    
    char stored_key[12];
    char stored_value[26];
    
    extract_chars(&given_file.content[0].properties[0].key, stored_key);
    extract_chars(&given_file.content[0].properties[0].value, stored_value);
    
    assert(key[11] == '\0');
    assert(given_file.content[0].properties[0].key.string[11] == '\0');
    assert(stored_key[11] == '\0');

    assert(equal_chars(stored_key, key));
    assert(equal_chars(stored_value, value));


    add_property("volume size", "4000");
    assert(given_file.content[0].number_of_properties == 2);
    
    
    char key2[] = "volume size";
    char value2[] = "4000";
    
    char stored_key2[12];
    char stored_value2[5];
    
    extract_chars(&given_file.content[0].properties[1].key, stored_key2);
    extract_chars(&given_file.content[0].properties[1].value, stored_value2);

    assert(equal_chars(stored_key2, key2));
    assert(equal_chars(stored_value2, value2));
    
    reset_json();
}

void test_convert_to_json()
{
    u8 s = 987654321;
    u8 *size = &s;
    add_file_characteristics("Regular file", size);
    
    char *path = "/some/imaginary/path/";
    add_file_path(path);
    
    convert_to_json();
    //printf("'%s' \n", json_output);
    
    char output[] = "{\"file kind\": \"Regular file\", "
                     "\"path\": \"/some/imaginary/path/"
                     "\", \"size\": \"987654321\", \"co"
                     "ntent\": []}";

    assert(equal_chars(json_output, output));

    // TODO add some more tests
    
    reset_json();
}

/* Main function responsible for json tests. */
void test_json()
{
    test_add_file_path();

    test_add_file_characteristics();

    test_identify_parent_id();
    
    /* test_allowed_char(); */
    
    test_clean_char();
    
    test_add_content_object();
    
    test_add_property();

    test_convert_to_json();   
}

#endif

/* EOF */
