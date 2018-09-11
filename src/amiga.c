/*
 * amiga.c
 * Detection of Amiga partition maps and file systems
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

#define MAXIMUM_PROPERTIES (3)

/* indicating code added or modified for JSON without which disktype
 * won't work anymore, even if JSON mode is deactivated */
#define J

/*
 * Amiga partition type codes
 *
 * Based on the dostypes list of the Ambient file manager
 * as of Apr 02, 2005.
 */




#ifdef J

/* Like an ordinaty property, an amiga_property contains a key and a value. */
struct amiga_property {
    char *key;
    char *value;
};
#endif


/* A dostype that is recognized by disktype has a corresponding dostype 
 * structur containing it's information.
 */
struct dostype {
  char *typecode;
  int isfs;   /* true = native Amiga filesystem (affects printout when */
              /*  found at the start of the boot sector) */
  
  /* Object type */
  char *name;

  #ifdef J
  char *wikidata;

  int property_count;
  
  struct amiga_property properties[MAXIMUM_PROPERTIES];
  #endif
};

#ifdef J

/* This list will contain all possibly recognized dostypes along with 
 * their properties and all information necessary to add one of those
 * dostypes to the json structures.
 */
struct dostype amiga_dostypes[47];

/* This variable is used by the add_amiga_dostype function to keep track
 * of the number of already added dostypes.
 */
int dostype_counter = 0;

/* Before the use of any amiga detect function, the amiga_dostypes list
 * has to be initialized, i.e. filled with all the dostypes.
 * Once this is done, 'initialized' will be set to 1.
 * detect functions therefore can check for an initialization using this 
 * variable.
 */
int initialized = 0;

/* Before scanning, a list with all dostypes has to be created.
 * This list will contain the typecodes by which a dostype is recognized,
 * as well as the corresponding content object and it's wikidata id.
 * 
 * This function allows to add one single dostype to this list.
 */
void add_amiga_dostype(char *typecode, int isfs, char *name, char *wikidata,
                       int property_count, ...)
{
    /* Add typecode, is filesystem flag, name, wikidata id, 
       number of properties. */
    amiga_dostypes[dostype_counter].typecode = typecode;
    amiga_dostypes[dostype_counter].isfs = isfs;
    amiga_dostypes[dostype_counter].name = name;
    amiga_dostypes[dostype_counter].wikidata = wikidata;
    amiga_dostypes[dostype_counter].property_count = property_count;

    /* Add properties. */
    va_list param;
    va_start(param, property_count);

    for (int i = 0; i < property_count; i++)
    {
        amiga_dostypes[dostype_counter].properties[i] = va_arg(param, struct 
                                                               amiga_property);
    }

    va_end(param);

    dostype_counter++;
}

/* Before the use of any amiga detect function, the amiga_dostypes list
 * has to be initialized, i.e. filled with all the dostypes.
 * 
 */
void init_amiga()
{
    /* If the initialization has been done before, there's nothing to do. */
    if (initialized == 1) {return; }

    /* Properties */
    struct amiga_property intl_t = {"intl", "true"};
    struct amiga_property intl_f = {"intl", "false"};

    struct amiga_property dir_cache_t = {"dir_cache", "true"};
    struct amiga_property dir_cache_f = {"dir_cache", "false"};

    /* muFS */
    struct amiga_property multiuser_t = {"multiuser", "true"};
    struct amiga_property multiuser_f = {"multiuser", "false"};
    
    /* LNFS */
    struct amiga_property longfilenames_t = {"long_file_names", "true"};    

    /* version */
    struct amiga_property version_0 = {"version", "0"};
    struct amiga_property version_1 = {"version", "1"};
    struct amiga_property version_2 = {"version", "2"};
    struct amiga_property version_3 = {"version", "3"};

    /* SCSI direct*/
    /* https://hjohn.home.xs4all.nl/SFS/scsi.htm */
    struct amiga_property scsidirect = {"SCSI_direct", "true"};
    
    /* NetBSD kind */
    struct amiga_property root = {"kind", "root"};
    struct amiga_property swap = {"kind", "swap"};
    struct amiga_property other = {"kind", "other"};
    
    /* Ami-File Safe */
    struct amiga_property experimental = {"experimental", "true"};


    /* Add all dostypes to the list. */

    /* { "DOS\x00", 1, "Amiga OFS file system (non-intl.)" } */
    add_amiga_dostype("DOS\x00", 1, "Amiga Old File System", "Q4746198", 2,
                      intl_f, multiuser_f);

    /* { "DOS\x01", 1, "Amiga FFS file system (non-intl.)" } */
    add_amiga_dostype("DOS\x01", 1, "Amiga Fast File System", "Q370047", 2,
                      intl_f, multiuser_f);

    /* { "DOS\x02", 1, "Amiga OFS file system (intl., no dir cache)" } */
    add_amiga_dostype("DOS\x02", 1, "Amiga Old File System", "Q4746198", 3,
                      intl_t, multiuser_f, dir_cache_f);

    /* { "DOS\x03", 1, "Amiga FFS file system (intl., no dir cache)" } */
    add_amiga_dostype("DOS\x03", 1, "Amiga Fast File System", "Q370047", 3,
                      intl_t, multiuser_f, dir_cache_f);   

    /* { "DOS\x04", 1, "Amiga OFS file system (intl., dir cache)" } */
    add_amiga_dostype("DOS\x04", 1, "Amiga Old File System", "Q4746198", 3,
                      intl_t, multiuser_f, dir_cache_t);

    /*  { "DOS\x05", 1, "Amiga FFS file system (intl., dir cache)" }, */
    add_amiga_dostype("DOS\x05", 1, "Amiga Fast File System", "Q370047 ", 3,
                      intl_t, multiuser_f, dir_cache_t);

    /* { "DOS\x06", 1, "Amiga OFS file system (LNFS)" } */
    add_amiga_dostype("DOS\x06", 1, "Amiga Old File System", "Q4746198", 2,
                      multiuser_f, longfilenames_t);

    /* { "DOS\x07", 1, "Amiga FFS file system (LNFS)" } */
    add_amiga_dostype("DOS\x07", 1, "Amiga Fast File System", "Q370047", 2,
                      multiuser_f, longfilenames_t);



    /* { "muFS",    1, "Amiga muFS FFS file system (intl., no dir cache)" } */
    add_amiga_dostype("muFS", 1, "Amiga Fast File System", "Q370047", 3,
                      intl_t, multiuser_t, dir_cache_f); 

    /* { "muF\x00", 1, "Amiga muFS OFS file system (non-intl.)" } */
    add_amiga_dostype("muF\x00", 1, "Amiga Old File System", "Q4746198", 2,
                      intl_f, multiuser_t);

    /* { "muF\x01", 1, "Amiga muFS FFS file system (non-intl.)" } */
    add_amiga_dostype("muF\x01", 1, "Amiga Fast File System", "Q370047", 2,
                      intl_f, multiuser_t);

    /* { "muF\x02", 1, "Amiga muFS OFS file system (intl., no dir cache)" } */
    add_amiga_dostype("muF\x02", 1, "Amiga Old File System", "Q4746198", 3,
                      intl_t, multiuser_t, dir_cache_f);

    /* { "muF\x03", 1, "Amiga muFS FFS file system (intl., no dir cache)" } */
    add_amiga_dostype("muF\x03", 1, "Amiga Fast File System", "Q370047", 3,
                      intl_t, multiuser_t, dir_cache_f);

    /* { "muF\x04", 1, "Amiga muFS OFS file system (intl., dir cache)" } */
    add_amiga_dostype("muF\x04", 1, "Amiga Old File System", "Q4746198", 3,
                      intl_t, multiuser_t, dir_cache_t);

    /* { "muF\x05", 1, "Amiga muFS FFS file system (intl., dir cache)" } */
    add_amiga_dostype("muF\x05", 1, "Amiga Fast File System", "Q370047", 3,
                      intl_t, multiuser_t, dir_cache_t);



    /* { "SFS\x00", 1, "Amiga Smart File System" } */
    add_amiga_dostype("SFS\x00", 1, "Amiga Smart File System",
                      "Q1054031", 0);

    /* { "PFS\x00", 1, "Amiga PFS file system 0" } */
    add_amiga_dostype("PFS\x00", 1, "Amiga Professional File System",
                      "Q7247965", 1, version_0);

    /* { "PFS\x01", 1, "Amiga PFS file system 1" } */
    add_amiga_dostype("PFS\x01", 1, "Amiga Professional File System",
                      "Q7247965", 1, version_1);

    /* { "PFS\x02", 1, "Amiga PFS file system 2" } */
    add_amiga_dostype("PFS\x02", 1, "Amiga Professional File System",
                      "Q7247965", 1, version_2);

    /* { "PFS\x03", 1, "Amiga PFS file system 3" } */
    add_amiga_dostype("PFS\x03", 1, "Amiga Professional File System",
                      "Q7247965", 1, version_3);

    /* { "PDS\x02", 1, "Amiga PFS file system 2, SCSIdirect" } */
    add_amiga_dostype("PDS\x02", 1, "Amiga Professional File System",
                      "Q7247965", 2, version_2, scsidirect);

    /* { "PDS\x03", 1, "Amiga PFS file system 3, SCSIdirect" } */
    add_amiga_dostype("PDS\x03", 1, "Amiga Professional File System",
                      "Q7247965", 3, version_3, scsidirect); 

    /*   { "muPF",    1, "Amiga PFS file system, multiuser" }, */
    add_amiga_dostype("muPF", 1, "Amiga Professional File System",
                      "Q7247965", 1, multiuser_t);



    /* { "AFS\x00", 1, "Amiga AFS file system" } */
    add_amiga_dostype("AFS\x00", 1, "Amiga Ami-File-Safe",
                      "Q55340903 ", 0);
    
    /* { "AFS\x01", 1, "Amiga AFS file system" } */
    add_amiga_dostype("AFS\x01", 1, "Amiga Ami-File-Safe",
                      "Q55340903 ", 1, experimental);

    /* { "UNI\x00", 0, "Amiga Unix 0" } */
    add_amiga_dostype("UNI\x00", 0, "Amiga Unix", "Q295179", 0);

    /* { "UNI\x01", 0, "Amiga Unix 1" } */
    add_amiga_dostype("UNI\x01", 0, "Amiga Unix", "Q295179", 0);
    
    /* { "KICK",    1, "Amiga Kickstart disk" } */
    add_amiga_dostype("KICK", 1, "Boot disk", "Q893130 ", 0);

    /* { "BOOU",    1, "Amiga generic boot disk" } */
    add_amiga_dostype("BOOU", 1, "Boot disk", "Q893130 ", 0);



    /* { "CD00",    0, "CD-ROM High Sierra format" } */
    add_amiga_dostype("CD00", 0, "High Sierra format", "Q5756978", 0);

    /* { "CD01",    0, "CD-ROM ISO9660 format" } */
    add_amiga_dostype("CD01", 0, "ISO9660", "Q55336682", 0);

    /* { "CDDA",    0, "CD Audio" } */
    add_amiga_dostype("CDDA", 0, "Audio CD", "Q1121020", 0);

    /* { "CDFS",    0, "CD-ROM - Amiga CDrive or AmiCDFS" } */
    add_amiga_dostype("CDFS", 0, "CD-ROM", "Q7982", 0);

    /* { "\x66\x2d\xab\xac", 0, "CD-ROM - AsimCDFS" } */
    add_amiga_dostype("\x66\x2d\xab\xac", 0, "CD-ROM", "Q7982", 0);

    /* { "NBR\x07", 0, "NetBSD root" } */
    add_amiga_dostype("NBR\x07", 0, "NetBSD", "Q34225", 1, root);

    /* { "NBS\x01", 0, "NetBSD swap" } */
    add_amiga_dostype("NBS\x01", 0, "NetBSD", "Q34225", 1, swap);

    /* { "NBU\x07", 0, "NetBSD other" } */
    add_amiga_dostype("NBR\x07", 0, "NetBSD", "Q34225", 1, other);



    /* { "LNX\x00", 0, "Linux native" } */
    add_amiga_dostype("LNX\x00", 0, "Linux", "Q388", 0);

    /* { "EXT2",    0, "Linux ext2" } */
    add_amiga_dostype("EXT2", 0, "Ext2", "Q283527 ", 0);

    /* { "SWAP",    0, "Linux swap" } */
    add_amiga_dostype("SWAP", 0, "Linux swap", "Q779098", 0);

    /* { "SWP\x00", 0, "Linux swap" } */
    add_amiga_dostype("SWP\x00", 0, "Linux swap", "Q779098", 0);

    /* { "MNX\x00", 0, "Linux minix" } */
    add_amiga_dostype("MNX\x00", 0, "MINIX", "Q685924", 0);



    /* { "MAC\x00", 0, "Macintosh HFS" } */
    add_amiga_dostype("MAC\x00", 0, "Apple HFS", "Q1058465", 0);

    /* { "MSD\x00", 0, "MS-DOS disk" } */
    add_amiga_dostype("MSD\x00", 0, "MS-DOS", "Q47604", 0);

    /* { "MSH\x00", 0, "MS-DOS PC-Task hardfile" } */
    add_amiga_dostype("MSH\x00", 0, "MS-DOS", "Q47604", 0);

    /* { "BFFS",    0, "Berkeley Fast Filesystem" } */
    add_amiga_dostype("BFFS", 0, "Berkeley Fast Filesystem", "Q2704864", 0);

    /* { NULL, 0, NULL } */
    add_amiga_dostype(NULL, 0, NULL, NULL, 0);

    
    /* Make sure the initialization is not repeated. */
    initialized = 1;
}

/* This function calculates the index of a dostype. */
int get_dostype(const unsigned char *dostype)
{
  int i;

  for (i = 0; amiga_dostypes[i].name; i++)
  {
    if (memcmp(dostype, amiga_dostypes[i].typecode, 4) == 0)
      return i;
  }
  return -1;
}
#endif

/*
struct dostype amiga_dostypes[] = {

  { "DOS\x00", 1, "Amiga OFS file system (non-intl.)" },
  { "DOS\x01", 1, "Amiga FFS file system (non-intl.)" },
  { "DOS\x02", 1, "Amiga OFS file system (intl., no dir cache)" },
  { "DOS\x03", 1, "Amiga FFS file system (intl., no dir cache)" },
  { "DOS\x04", 1, "Amiga OFS file system (intl., dir cache)" },
  { "DOS\x05", 1, "Amiga FFS file system (intl., dir cache)" },
  { "DOS\x06", 1, "Amiga OFS file system (LNFS)" },
  { "DOS\x07", 1, "Amiga FFS file system (LNFS)" },

  { "muFS",    1, "Amiga muFS FFS file system (intl., no dir cache)" },
  { "muF\x00", 1, "Amiga muFS OFS file system (non-intl.)" },
  { "muF\x01", 1, "Amiga muFS FFS file system (non-intl.)" },
  { "muF\x02", 1, "Amiga muFS OFS file system (intl., no dir cache)" },
  { "muF\x03", 1, "Amiga muFS FFS file system (intl., no dir cache)" },
  { "muF\x04", 1, "Amiga muFS OFS file system (intl., dir cache)" },
  { "muF\x05", 1, "Amiga muFS FFS file system (intl., dir cache)" },

  { "SFS\x00", 1, "Amiga Smart File System" },

  { "PFS\x00", 1, "Amiga PFS file system 0" },
  { "PFS\x01", 1, "Amiga PFS file system 1" },
  { "PFS\x02", 1, "Amiga PFS file system 2" },
  { "PFS\x03", 1, "Amiga PFS file system 3" },
  { "PDS\x02", 1, "Amiga PFS file system 2, SCSIdirect" },
  { "PDS\x03", 1, "Amiga PFS file system 3, SCSIdirect" },
  { "muPF",    1, "Amiga PFS file system, multiuser" },

  { "AFS\x00", 1, "Amiga AFS file system" },
  { "AFS\x01", 1, "Amiga AFS file system (experimental)" },

  { "UNI\x00", 0, "Amiga Amix 0" },
  { "UNI\x01", 0, "Amiga Amix 1" },

  { "KICK",    1, "Amiga Kickstart disk" },
  { "BOOU",    1, "Amiga generic boot disk" },
  { "BAD\x00", 0, "Unreadable disk" },
  { "NDOS",    0, "Not a DOS disk" },
  { "resv",    0, "reserved" },

  { "CD00",    0, "CD-ROM High Sierra format" },
  { "CD01",    0, "CD-ROM ISO9660 format" },
  { "CDDA",    0, "CD Audio" },
  { "CDFS",    0, "CD-ROM - Amiga CDrive or AmiCDFS" },
  { "\x66\x2d\xab\xac", 0, "CD-ROM - AsimCDFS" },

  { "NBR\x07", 0, "NetBSD root" },
  { "NBS\x01", 0, "NetBSD swap" },
  { "NBU\x07", 0, "NetBSD other" },

  { "LNX\x00", 0, "Linux native" },
  { "EXT2",    0, "Linux ext2" },
  { "SWAP",    0, "Linux swap" },
  { "SWP\x00", 0, "Linux swap" },
  { "MNX\x00", 0, "Linux minix" },

  { "MAC\x00", 0, "Macintosh HFS" },
  { "MSD\x00", 0, "MS-DOS disk" },
  { "MSH\x00", 0, "MS-DOS PC-Task hardfile" },
  { "BFFS",    0, "Berkeley Fast Filesystem" },

  { NULL, 0, NULL },
};
*/

static char * get_name_for_dostype(const unsigned char *dostype)
{
  int i;

  for (i = 0; amiga_dostypes[i].name; i++)
    if (memcmp(dostype, amiga_dostypes[i].typecode, 4) == 0)
      return amiga_dostypes[i].name;
  return "Unknown";
}

static void format_dostype(char *buf, const unsigned char *dostype)
{
  int i;
  unsigned char c;
  char *p;

  p = buf;
  for (i = 0; i < 4; i++) {
    c = dostype[i];
    if (c < 10) {
      *p++ = '\\';
      *p++ = '0' + c;
    } else if (c < 32) {
      sprintf(p, "0x%02x", (int)c);
      p = strchr(p, 0);
    } else {
      *p++ = (char)c;
    }
  }
  *p = 0;
}

/*
 * Amiga "Rigid Disk" partition map
 */

void detect_amiga_partmap(SECTION *section, int level)
{
  #ifdef J
  init_amiga();
  #endif


  int i, off, found;
  unsigned char *buf;
  char s[256], append[64];
  u4 blocksize, part_ptr;
  u8 cylsize, start, size;

  for (off = 0, found = 0; off < 16; off++) {
    if (get_buffer(section, off * 512, 512, (void **)&buf) < 512)
      break;

    if (memcmp(buf, "RDSK", 4) == 0) {
      found = 1;
      break;
    }
  }
  if (!found)
    return;

  #ifdef JSON
  add_content_object(level, "Amiga rigid disk partition map", "Q55357472");
  #endif

  if (off == 0)
  {
    print_line(level, "Amiga Rigid Disk partition map");
  }
  else
  {
    #ifdef JSON
    add_property_int("sector", off);
    #endif

    print_line(level, "Amiga Rigid Disk partition map at sector %d", off);
  }


  /* get device block size (?) */
  blocksize = get_be_long(buf + 16);
  
  #ifdef JSON
  add_property_u4("sector_size", blocksize);
  #endif
  
  if (blocksize < 256 || (blocksize & (blocksize-1))) {
    print_line(level+1, "Illegal block size %lu", blocksize);
    return;
  } else if (blocksize != 512) {
    print_line(level+1, "Unusual block size %lu, not sure this will work...",
               blocksize);
  }
  /* TODO: get geometry data for later use */

  /* walk the partition list */
  part_ptr = get_be_long(buf + 28);
  for (i = 1; part_ptr != 0xffffffffUL; i++) {
    if (get_buffer(section, (u8)part_ptr * 512, 256,
		   (void **)&buf) < 256) {

      #ifdef JSON
      add_content_object(level, "Partition", "Q255215");

      add_property("kind", "amiga");
      add_property_int("number", i);
      #endif
  
      print_line(level, "Partition %d: Can't read partition info block");
      break;
    }

    /* check signature */
    if (memcmp(buf, "PART", 4) != 0) {

      #ifdef JSON
      add_content_object(level, "Partition", "Q255215");

      add_property("kind", "amiga");
      add_property_int("number", i);
      #endif

      print_line(level, "Partition %d: Invalid signature");
      break;
    }

    /* get "next" pointer for next iteration */
    part_ptr = get_be_long(buf + 16);

    /* get sizes */
    cylsize = (u8)get_be_long(buf + 140) * (u8)get_be_long(buf + 148);
    start = get_be_long(buf + 164) * cylsize;
    size = (get_be_long(buf + 168) + 1 - get_be_long(buf + 164)) * cylsize;

    snprintf(append, 63, " from %llu", start);
    format_blocky_size(s, size, 512, "sectors", append);

    #ifdef JSON
    add_content_object(level, "Partition", "Q255215");
    add_property("kind", "amiga");
    add_property_int("number", i);
    add_property_u8("size", (u8) (size * 512));
    add_property_int("sector_size", 512);
    #endif  

    /* s seems to be the total size in bytes, i.e. size * 512 */
    print_line(level, "Partition %d: %s",
               i, s);

    /* get name */
    get_pstring(buf + 36, s);
    if (s[0])
    {
      #ifdef JSON
      add_property("drive_name", s);
      #endif

      print_line(level + 1, "Drive name \"%s\"", s);
    }
    
    #ifdef JSON
    /* figure out the correct dostype */
    int index = get_dostype(buf + 192);
    
    add_content_object(level + 1, amiga_dostypes[index].name,
                       amiga_dostypes[index].wikidata);

    /* add properties */
    for (int x = 0; x < amiga_dostypes[index].property_count; x++)
    {
        add_property(amiga_dostypes[index].properties[x].key, 
                     amiga_dostypes[index].properties[x].value);
    }
    #endif

    /* show dos type */
    format_dostype(s, buf + 192);

    print_line(level + 1, "Type \"%s\" (%s)", s,
               get_name_for_dostype(buf + 192));

    /* detect contents */
    if (size > 0 && start > 0) {
      analyze_recursive(section, level + 1,
			start * 512, size * 512, 0);
    }
  }
}

/*
 * Amiga file system
 */

void detect_amiga_fs(SECTION *section, int level)
{
  #ifdef J
  /* Make sure the dostypes are available. */
  init_amiga();
  #endif

  unsigned char *buf;
  int i, isfs;
  char s[256], *typename;

  if (get_buffer(section, 0, 512, (void **)&buf) < 512)
    return;

  /* look for one of the signatures */
  isfs = 0;
  typename = NULL;
  for (i = 0; amiga_dostypes[i].name; i++)
    if (memcmp(buf, amiga_dostypes[i].typecode, 4) == 0) {
      isfs = amiga_dostypes[i].isfs;
      typename = amiga_dostypes[i].name;
      break;
    }
  if (typename == NULL)
    return;

  if (isfs) {
    
    #ifdef JSON
    /* A file system content object has been found. */
    add_content_object(level, amiga_dostypes[i].name, 
                       amiga_dostypes[i].wikidata);
    #endif

    print_line(level, "%s", typename);

    format_dostype(s, buf);
    print_line(level + 1, "Type \"%s\"", s);

    if (section->size == 512*11*2*80) {
      
      #ifdef JSON
      add_property("floppy_size", "DD");
      #endif

      print_line(level+1, "Size matches DD floppy");
    } else if (section->size == 512*22*2*80) {

      #ifdef JSON
      add_property("floppy_size", "HD");
      #endif

      print_line(level+1, "Size matches HD floppy");
    }

  } else {

    #ifdef JSON
    /* A non-file-system content object has been found. */
    add_content_object(level, amiga_dostypes[i].name, 
                       amiga_dostypes[i].wikidata);
    #endif

    format_dostype(s, buf);
    print_line(level, "Amiga type code \"%s\" (%s)", s, typename);

  }
}

#ifdef JSON

/* This function resets the variables in the amiga class.
 * This allows to recreate the initial environment after running
 * some tests.
 */
void reset()
{
    /* Reset amiga_dostypes */
    for (int i = 0; i < dostype_counter; i++)
    {
        /* Reset current dostype */
        amiga_dostypes[i].typecode = NULL;
        amiga_dostypes[i].isfs = -1;
        amiga_dostypes[i].name = NULL;
        amiga_dostypes[i].wikidata = NULL;
        
        /* Reset all properties */
        for (int k = 0; k < amiga_dostypes[i].property_count; k++)
        {
            /* Reset current property */
            amiga_dostypes[i].properties[k].key = NULL;
            amiga_dostypes[i].properties[k].value = NULL;
        }

        amiga_dostypes[i].property_count = 0;
    }
    
    dostype_counter = 0;
    initialized = 0;
}


// -----------------------------------------------------------
//                             TESTS
// -----------------------------------------------------------


void test_add_amiga_dostype()
{
    /* First dostype */
    struct amiga_property intl_f = {"intl", "false"};
    struct amiga_property multiuser_f = {"multiuser", "false"};
    
    add_amiga_dostype("DOS\x00", 1, "Amiga Old File System", "Q4746198", 2,
                      intl_f, multiuser_f);

    /* test assignments */
    assert(dostype_counter == 1);
    assert(equal_chars(amiga_dostypes[0].typecode, "DOS\x00") == 1);
    assert(amiga_dostypes[0].isfs == 1);
    assert(equal_chars(amiga_dostypes[0].name, "Amiga Old File System") == 1);
    assert(equal_chars(amiga_dostypes[0].wikidata, "Q4746198") == 1);
    assert(amiga_dostypes[0].property_count == 2);

    /* test first property */
    assert(equal_chars(amiga_dostypes[0].properties[0].key, "intl") == 1);
    assert(equal_chars(amiga_dostypes[0].properties[0].value, "false") == 1);

    /* test second property */
    assert(equal_chars(amiga_dostypes[0].properties[1].key, "multiuser") == 1);
    assert(equal_chars(amiga_dostypes[0].properties[1].value, "false") == 1);



    /* Test an additional dostype */

    add_amiga_dostype("SFS\x00", 1, "Smart File System", "Q1054031", 0);
    
    /* test assignments */
    assert(dostype_counter == 2);
    assert(equal_chars(amiga_dostypes[1].typecode, "SFS\x00") == 1);
    assert(amiga_dostypes[1].isfs == 1);
    assert(equal_chars(amiga_dostypes[1].name, "Smart File System") == 1);
    assert(equal_chars(amiga_dostypes[1].wikidata, "Q1054031") == 1);
    assert(amiga_dostypes[1].property_count == 0);
    
    reset();
}

void test_init_amiga()
{
    assert(initialized == 0);

    init_amiga();

    assert(initialized == 1);    
    assert(dostype_counter == 47);

    reset();
}

void test_get_dostype()
{
    init_amiga();

    const unsigned char x2[] = "DOS\x02";
    const unsigned char *y2 = x2;

    assert(get_dostype(y2) == 2);
    
    const unsigned char x0[] = "DOS\x00";
    const unsigned char *y0 = x0;

    assert(get_dostype(y0) == 0);

    const unsigned char x45[] = "BFFS";
    const unsigned char *y45 = x45;

    assert(get_dostype(y45) == 45);

    reset();
}

/* Main function responsible for tests in this class. */
void test_amiga()
{
    test_add_amiga_dostype();
    test_init_amiga();
    test_get_dostype();
}
#endif

/* EOF */
