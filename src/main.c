/*
 * main.c
 * Main entry point and global utility functions.
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

#include "global.h"
// #include "assert.h"

#ifdef USE_MACOS_TYPE
#include <CoreServices/CoreServices.h>
#endif


#ifdef JSON
/* Structure storing the information formerly printed. */
struct file_info given_file;

/* Assuming latin1 allows to clean strings from undesired characters
 * like quotes and backslashs. */
int latin1 = 0;

#endif



/*
 * local functions
 */

static void analyze_file(const char *filename);
static void print_kind(int filekind, u8 size, int size_known);

#ifdef USE_MACOS_TYPE
static void show_macos_type(const char *filename);
#endif

int test_arg(int argc, char *argv[]);
int latin1_arg(int argc, char *argv[]);
int optional_args(int argc, char *argv[]);


/*
 * entry point
 */

int main(int argc, char *argv[])
{
    
  /* Determine the position of the first argument that is
   * actually a path. */
  int first_path = optional_args(argc, argv);
  
  /* wrong arguments */
  if (first_path == -1) {return 1;}

  #ifdef JSON
  given_file.number_of_objects = 0; 
  #endif

  /* loop over filenames */
  print_line(0, "");
  for (int i = first_path; i < argc; i++) {
    analyze_file(argv[i]);

    #ifdef JSON
    add_file_path(argv[i]);

    convert_to_json();
    
    printf("%s", json_output);

    /* Reset all values to analyze the next file. */
    reset_json(); 
    #endif

    print_line(0, "");
  }

  return 0;
}

/* This function handles optional arguments.
 * 
 * It returns the position of the first argument pointing to a file
 * and -1 if there are wrong arguments.
 */
int optional_args(int argc, char *argv[])
{
  /* argument check */
  
  /* too few arguments */
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [--latin1] [--test] <device/file>...\n", PROGNAME);
    return -1;
  }
  
  int use_latin = latin1_arg(argc, argv);
  int run_tests = test_arg(argc, argv);
  
  if (use_latin == -1 || run_tests == -1) 
  {
      fprintf(stderr, "Usage: %s [--latin1] [--test] <device/file>...\n", PROGNAME);
      return -1;
  }

  return 1 + use_latin + run_tests;
}


/* This function checks whether one of the aruments
 * is --latin1 and the latin1 assumption may be used to
 * clean strings.
 * 
 * It will return 1 if a --latin1 argument is found,
 *                0 if none is found and
 *               -1 if there are wrong arguments.
 * 
 */
int latin1_arg(int argc, char *argv[])
{
    #ifdef JSON
    /* Use latin1 assumption */
    if (strcmp(argv[1], "--latin1") == 0)
    {
        latin1 = 1;
        /* We still need a file path. */
        if (argc < 3) { return -1; }
    }
    else
    {
        latin1 = 0;
    }
    
    return latin1;
    #endif
    return 0;
}

/* This function checks whether one of the arguments
 * is --test and run tests if this is the case.
 * 
 * It will return 1 if a --test argument is found,
 *                0 if none is found and
 *               -1 if there are wrong arguments.
 */
int test_arg(int argc, char *argv[])
{
  /* Test as first argument */
  if (strcmp(argv[1], "--test") == 0)
  {
      /* We still need a file path. */
      if (argc < 3) { return -1; }
      #ifdef JSON
      test();
      #endif

      return 1;
  }

  /* Test as second argument. */
  if (argc < 3) {return 0; }
  
  if (strcmp(argv[2], "--test") == 0)
  {
      /* We still need a file path. */
      if (argc < 4) { return -1; }
      #ifdef JSON
      test();
      #endif

      return 1;
  }  
  return 0;
}


/*
 * Analyze one file
 */

static void analyze_file(const char *filename)
{
  int fd, filekind;
  u8 filesize;
  struct stat sb;
  char *reason;
  SOURCE *s;

  print_line(0, "--- %s", filename);

  /* stat check */
  if (stat(filename, &sb) < 0) {
    errore("Can't stat %.300s", filename);
    return;
  }

  filekind = 0;
  filesize = 0;
  reason = NULL;
  if (S_ISREG(sb.st_mode)) {
    filesize = sb.st_size;
    print_kind(filekind, filesize, 1);
  } else if (S_ISBLK(sb.st_mode))
    filekind = 1;
  else if (S_ISCHR(sb.st_mode))
    filekind = 2;
  else if (S_ISDIR(sb.st_mode))
    reason = "Is a directory";
  else if (S_ISFIFO(sb.st_mode))
    reason = "Is a FIFO";
#ifdef S_ISSOCK
  else if (S_ISSOCK(sb.st_mode))
    reason = "Is a socket";
#endif
  else
    reason = "Is an unknown kind of special file";

  if (reason != NULL) {
    error("%.300s: %s", filename, reason);
    return;
  }

  /* Mac OS type & creator code (if running on Mac OS X) */
#ifdef USE_MACOS_TYPE
  if (filekind == 0)
    show_macos_type(filename);
#endif

  /* empty regular files need no further analysis */
  if (filekind == 0 && filesize == 0)
    return;

  /* open for reading */
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    errore("Can't open %.300s", filename);
    return;
  }

  /* (try to) guard against TTY character devices */
  if (filekind == 2) {
    if (isatty(fd)) {
      error("%.300s: Is a TTY device", filename);
      return;
    }
  }

  /* create a source */
  s = init_file_source(fd, filekind);

  /* tell the user what it is */
  if (filekind != 0)
    print_kind(filekind, s->size, s->size_known);

  /* now analyze it */
  analyze_source(s, 0);

  /* finish it up */
  close_source(s);
}

static void print_kind(int filekind, u8 size, int size_known)
{
  char buf[256], *kindname;

  if (filekind == 0)
    kindname = "Regular file";
  else if (filekind == 1)
    kindname = "Block device";
  else if (filekind == 2)
    kindname = "Character device";
  else
    kindname = "Unknown kind";

  if (size_known) {
    format_size_verbose(buf, size);
    print_line(0, "%s, size %s", kindname, buf);
    
    #ifdef JSON
    /* Store file kind and file size in the file info structure. */
    add_file_characteristics(kindname, &size);
    #endif
    
  } else {
    print_line(0, "%s, unknown size", kindname);
    
    #ifdef JSON
    /* Store file kind in the file info structure. */
    add_file_characteristics(kindname, NULL);
    #endif
  }
}

/*
 * Mac OS type & creator code
 */

#ifdef USE_MACOS_TYPE

static void show_macos_type(const char *filename)
{
  int err;
  FSRef ref;
  FSCatalogInfo info;
  FInfo *finfo;

  err = FSPathMakeRef(filename, &ref, NULL);
  if (err == 0) {
    err = FSGetCatalogInfo(&ref, kFSCatInfoFinderInfo,
			   &info, NULL, NULL, NULL);
  }

  if (err == 0) {
    finfo = (FInfo *)(info.finderInfo);
    if (finfo->fdType != 0 || finfo->fdCreator != 0) {
      char typecode[5], creatorcode[5], s1[256], s2[256];

      memcpy(typecode, &finfo->fdType, 4);
      typecode[4] = 0;
      format_ascii(typecode, s1);

      memcpy(creatorcode, &finfo->fdCreator, 4);
      creatorcode[4] = 0;
      format_ascii(creatorcode, s2);

      print_line(0, "Type code \"%s\", creator code \"%s\"",
		 s1, s2);
    } else {
      print_line(0, "No type and creator code");
    }
  }
  if (err) {
    print_line(0, "Type and creator code unknown (error %d)", err);
  }
}

#endif

/* EOF */
