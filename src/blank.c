/*
 * blank.c
 * Detection of Blank Disk (Formatted)
 *
 * Written By Aaron Geyer of StorageQuest, Inc.
 *
 * Copyright (c) 2003 Aaron Geyer
 * 
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
 *
 */

#include "global.h"

/* The principle used to determine whether or not a disk
   is blank is to search the disk for a byte code that
   does not change over the disk.  This module only scans the first
   2MB of the disk.  This module will need more work. */

#define BLOCK_SIZE (512)
#define MAX_BLOCKS (2048*2)
#define MIN_BLOCKS (64*2)



#ifdef JSON

/* This function adds a "Blank" content object to the json output.
 * 
 * The property "empty_section_size" contains the number of empty 
 * bytes the scanned section holds.
 * 
 * Since not all of the disk/medium is scanned, we can't
 * guarantee it's emptieness, even if we don't find anything.
 * After scanning a reasonably large part we will estimate
 * the disk/medium to be blank though and store this
 * estimation in the property "all_empty_guess".
 */
void add_blank(int level, int blank_blocks, int all_empty_guess)
{
    add_content_object(level, "Blank", "Q543287");
    
    add_property("all_empty_guess", 
                 (all_empty_guess) ? "true" : "false");

    add_property_u8("empty_section_size", (u8) (blank_blocks * BLOCK_SIZE));
}
#endif



void detect_blank(SECTION *section, int level)
{
  unsigned char *buffer;
  int i, j;
  int block_size = BLOCK_SIZE;
  int max_blocks = MAX_BLOCKS;
  int blank_blocks = 0;
  unsigned char code;
  char s[256];

  if (get_buffer(section, 0, 1, (void **)&buffer) < 1)
    return;
  code = buffer[0];

  /* Limit to actual size of partition / disk */
  if (section->size && section->size < max_blocks * block_size) {
    max_blocks = section->size / block_size;
  }

  /* Determine number of blank blocks */
  for (i = 0; i < max_blocks; i++) {
    if (get_buffer(section, i * block_size, 
                   block_size, (void **)&buffer) < block_size)
      break;

    for (j = 0; j < block_size; j++) {
      if (buffer[j] != code)
	break;
    }
    if (j < block_size)
      break;

    blank_blocks = i + 1;
  }

  if (blank_blocks >= max_blocks) {

    #ifdef JSON
    /* We expect the whole disk to be blank, 
     * therefore the third argument is 1. */
    add_blank(level, blank_blocks, 1);
    #endif

    print_line(level, "Blank disk/medium");

  } else if (blank_blocks > MIN_BLOCKS) {


    #ifdef JSON
    /* Not all of the disk is blank,
     * therefore the third argument is 0. */
    add_blank(level, blank_blocks, 0);
    #endif


    format_size(s, blank_blocks * block_size);
    print_line(level, "First %s are blank", s);
  }
}
