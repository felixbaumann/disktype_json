/*
 * global.h
 * Common header file.
 *
 * Copyright (c) 2003 Christoph Pfisterer
 * Copyright (c) 2018 Felix Baumann on modifications
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

#define PROGNAME "disktype"

/* if defined, disktype will output its results in JSON instead
 * of plain text
 */
#define JSON

/* if defined, disktype will run some tests at each execution */
#define RUN_TESTS


/* global includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <assert.h>
#include <stdarg.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>


/* constants */

#define FLAG_IN_DISKLABEL (0x0001)

/* types */

typedef signed char s1;
typedef unsigned char u1;
typedef short int s2;
typedef unsigned short int u2;
typedef long int s4;
typedef unsigned long int u4;
typedef long long int s8;
typedef unsigned long long int u8;

typedef struct source {
  u8 size;
  int size_known;
  void *cache_head;

  int sequential;
  u8 seq_pos;
  int blocksize;
  struct source *foundation;

  int (*analyze)(struct source *s, int level);
  u8 (*read_bytes)(struct source *s, u8 pos, u8 len, void *buf);
  int (*read_block)(struct source *s, u8 pos, void *buf);
  void (*close)(struct source *s);

  /* private data may follow */
} SOURCE;

typedef struct section {
  u8 pos, size;
  int flags;
  SOURCE *source;
} SECTION;

typedef void (*DETECTOR)(SECTION *section, int level);


#ifdef JSON

/* String as array of chars with a dynamic size. */
typedef struct {
    size_t used_size;
    size_t total_size;
    char *string;  
} String;

/* String functions */
void initialize_String(String *s, size_t init_size);
void insert_chars(String *str, char insert[]);
void insert_single_char(String *str, char element);
void insert_string(String *str, String *insert);
void extract_chars(String *str, char chars[]);
void free_String(String *s);


/* Specific information about a detected object is stored in its properties.
 * For a file system that may be it's volume name, for a disklabel the 
 * number of entries.
 *
 * KEY the name of the property
 *
 * VALUE the value this property holds
 */
struct property
{
  String key;

  String value;
};


/* Content object corresponding to a wikidata entity.
 * May represent a file system, partition, archive, ...
 * You can find a complete list of possible objects in the documentation.
 *
 * ID is an internal id of the object, used in the process of converting
 *    the collected information to JSON.
 *
 * LEVEL is an internal value corresponding to the indent of the former
 *       print statement. It allows the identification of the parent object.
 *
 * PARENT_ID is an internal id, pointing to the next higher object in the
 *           structure of the given file.
 *           It is used in the process of converting the collected 
 *           information to JSON in order to keep the relations between 
 *           objects intact.
 *           A parent_id of -1 indicates no existing parent.
 *
 * OBJECT_TYPE refers to the name of this type of object. 
 *             Say 'FAT12', 'ReiserFS' or 'FreeBSD boot loader'...
 *
 * WIKIDATA contains the wikidata id of the object type.
 *          E.g. 'Q3063042' for FAT12.
 * 
 * NUMBER_OF_PROPERTIES is the number of properties assigned to this object.
 *                      It may not be larger than 100.
 */
struct content_object
{
  int id;
  
  int level;
  
  int parent_id;

  String object_type;

  String wikidata;
  
  int number_of_properties;
  
  struct property properties[100];

};



/* Main structure storing the information formerly printed. 
 *
 * FILE_KIND contains the kind of file handed to disktype.
 *           It's domain is:
 *           {Regular file, Block device, Character device, Unknown kind}
 *
 * PATH is the current location of the file.
 *
 * SIZE is the size of the file in bytes.
 *      Since files larger than 4GB are quite common, even an unsigned 'long' 
 *      is not enough.
 *
 * NUMBER_OF_OBJECTS contains the number of content objects found in content.
 * 
 * CONTENT contains all the objects found in the file.
 *         Those may be file systems, partitions, boot loaders, ...
 *
 */
extern struct file_info 
{
  String file_kind;
  
  String path;
  
  unsigned long long int size;
  
  int number_of_objects;
  
  struct content_object content[500];

}given_file;


/* There's an option to interpret textual properties
 * of the given disk as latin1 using a command line argument. 
 * This variable stores the decision.
 * 1 for using latin1, 0 otherwise.
 */
extern int latin1;



/* json functions */

void add_file_path(char* path);

void add_file_characteristics(char file_kind[], unsigned long long *size);

void add_content_object(int level, char object_type[], 
                        char wikidata[]);
void add_property(char key[], char value[]);

void add_property_int(char key[], int value);

void add_property_u4(char key[], u4 value);

void add_property_u8(char key[], u8 value);

void add_property_endianness(int endianness);


void convert_to_json();

void reset_json();

extern char json_output[];


/* test.c */
void test();

int equal_chars(char* first, char* second);

/* amiga.c */
void test_amiga();

/* cdaccess.c */
void test_cdaccess();

/* vpc.c */
void test_vpc();

/* json.c */
void test_json();

/* string.c */
void test_string();


#endif




/* detection dispatching functions */

void analyze_source(SOURCE *s, int level);
void analyze_source_special(SOURCE *s, int level, u8 pos, u8 size);
void analyze_recursive(SECTION *section, int level,
		       u8 rel_pos, u8 size, int flags);
void stop_detect(void);

/* file source functions */

SOURCE *init_file_source(int fd, int filekind);

int analyze_cdaccess(int fd, SOURCE *s, int level);

/* buffer functions */

u8 get_buffer(SECTION *section, u8 pos, u8 len, void **buf);
u8 get_buffer_real(SOURCE *s, u8 pos, u8 len, void *inbuf, void **outbuf);
void close_source(SOURCE *s);

/* output functions */

void print_line(int level, const char *fmt, ...);
void start_line(const char *fmt, ...);
void continue_line(const char *fmt, ...);
void finish_line(int level);

/* formatting functions */

void format_blocky_size(char *buf, u8 count, u4 blocksize,
                        const char *blockname, const char *append);
void format_size(char *buf, u8 size);
void format_size_verbose(char *buf, u8 size);

void format_ascii(void *from, char *to);
void format_utf16_be(void *from, u4 len, char *to);
void format_utf16_le(void *from, u4 len, char *to);

void format_uuid(void *from, char *to);
void format_uuid_lvm(void *uuid, char *to);
void format_guid(void *guid, char *to);

/* endian-aware data access */

u2 get_be_short(void *from);
u4 get_be_long(void *from);
u8 get_be_quad(void *from);

u2 get_le_short(void *from);
u4 get_le_long(void *from);
u8 get_le_quad(void *from);

u2 get_ve_short(int endianness, void *from);
u4 get_ve_long(int endianness, void *from);
u8 get_ve_quad(int endianness, void *from);

const char * get_ve_name(int endianness);

/* more data access */

void get_string(void *from, int len, char *to);
void get_pstring(void *from, char *to);
void get_padded_string(void *from, int len, char pad, char *to);

int find_memory(void *haystack, int haystack_len,
		void *needle, int needle_len);

/* name table lookups */

char * get_name_for_mbrtype(int type);

/* error functions */

void error(const char *msg, ...);
void errore(const char *msg, ...);
void bailout(const char *msg, ...);
void bailoute(const char *msg, ...);

/* EOF */
