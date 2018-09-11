/* DISKTYPES WITH PROPERTIES */

/* This is a list of all potentially detected structures by disktype
 * along with the ids of their corresponding wikidata entities and
 * all their properties.
 * 
 * Note that a property will only appear in the JSON result if disktype
 * found any information about it.
 * 
 * Since there are about 86 different structures with most of them having
 * lots of properties, this file has significant size and is therfore written
 * as comments to allow the use of this beautiful invention called code folding.
 */

/* INTRODUCTION TO PROPERTIES
 * 
 * Each content object may have properties like the size of a file system 
 * or the number of entries in a partition map.
 *
 * A property constists of a name and a value of a specified domain.
 *
 * Each content object type has some individual list of properties for 
 * all its instances. Depending on disktypes discoveries not all of the 
 * properties will actually be filled with values though.
 *
 * To ease the use of properties, they will be inherited according to 
 *  wikidata's "instance of" and "subclass of" statements.
 *
 * That says, a "ReiserFS" content object according to wikidata is an instance 
 * of a "journaling file system" which again is a subclass of "file system". 
 * 
 * Therefore a "ReiserFS" content object will share all the properties of 
 * "journaling file system" as well as "file system". On top of that, 
 * it has some individual properties.
 */

 /* SEMANTICS
  *
  * Each content object is represented with it's name 
  * and references to other content objects of which it's either 
  * an "instance of" or a "subclass of" in parantheses.
  *
  * On the right follows the wikidata id of the corresponding entity.
  * 
  * After that follows a list with all its properties.
  * A "-" indicates an individual property,
  * a "+" is an inherited, used property and
  * a "/" is an inherited property that is not used.
  * 
  * The unused ones are just mentioned to show that there won't be 
  * any conflicts with individual ones.
  * 
  * Behind the name of an individual property, it's domain is found in "{ }".
  * The domain may either be described explicitely (e.g. {little, big})
  * or by reference to a data type (e.g. {#int}).
  * 
  * Behind the name of an inherited property,
  * the content object it comes from is mentioned in parantheses.
  * 
  * Afterwards may be some explanation in parantheses.
  * 
  * In squared brackets the classes are mentioned, where this content
  * object comes from.
  * 
  */







/* File System ()                                   Q174989
  - block_size  	    {#u8}           (size of a single block / sector in bytes)
  - volume_size         {#u8}           (total size of the file system in bytes)
  - volume_name         {#char[256]}
  - endianness          {little, big}
  - version             {#int}
  - UUID                {#char[256]}
  - floppy_size         {DD, HD}        (Does the volume_size match the size of a DD/HD floppy?)
  - last_mounted        {#char[256]}    (location of the last mounting)
*/

/* Partition Table (s_data structure)                 Q614361
  - entries     {#int}      (number of partitions)
  - sector_size {#int}      (size of a single sector)
*/

/* Partition ()                                     Q255215                 {...}
  - kind                {apple, amiga, atari, mbr, gpt, sparc, vtoc, bsd} 
                                (source files dos: mbr, gpt; unix: sparc, vtoc, bsd)
  - name                {#char[256]}
  - number              {#int}  (partitions are enumerated)
  - size                {#u8}   (size in bytes)
  - sector_size         {#int}  (the size in bytes of a single sector)
  - start_sector        {#u8}
  - type_name           {apple: Apple_Free, Apple_HFS, Apple_partition_map, ...
                         amiga: ...
                         atari: Standard GEMDOS, Big GEMDOS, Extended, Unknown
                         mbr:   {#mbr_types}
                         gpt:   {#gpt_types}
                         sparc: -
                         vtoc: {#vtoc_types}
                         bsd: {#bsd_types}
                        }
  - efi_gpt_protective
  - blank_disk
  - bootable            {true, false}
  - disklabel           {true}          (true if this partition contains the disklabel)
  - unused              {true}
  - GUID                {#char[256]}
  - drive_name          {#char[256]}
  - letter              {char}      (instead of an enumeration, partitions may have letters)

  [apple.c, amiga.c, atari.c, unix.c, dos.c]
*/

/* Linux swap ()                                    Q779098                 {implemented}
  - version         {1, 2}
  - sub_version     {#int}
  - endianness      {little, big}
  - page_size       {#int}
  - swap_size       {#u8}

  [linux.c, amiga.c]
*/

/* Disk Image (s_archive file format, s_file format)    Q592312                 {implemented}
  + start_sector (file format)
  - source          {floppy, hard_disk, non-emulated, unknown}
  - floppy_size     {1.2, 1.44, 2.88}   (size in MB)
  - bootable        {true, false}
  - platform        {x86, PowerPC, Macintosh, EFI, unknown}
  - mbr_type        {#mbr_types}

  [cdrom.c]
*/

/* Blank ()                                         Q543287                 {implemented}
    (Blank Disk/Medium)
  - all_empty_guess       {true, false} (true if the disk is expected to be empty.
                                         Just a guess though, since not all of the
                                         disk is scanned.)
  - empty_section_size    {#int}        (size of the empty section in bytes that
                                         has been scanned, of course there may be
                                         even more empty space)

  [blank.c]
*/

/* CD-ROM (s_compact disk)                          Q7982                   {implemented}
  - number_of_tracks    {#int}
  - disk_ID             {#8 digits hexadecimal}     (CDDB disk ID)

  [amiga.c, cdaccess.c]
*/

/* LVM (software)                                   Q6667482                {implemented}
  - version                 {LVM1, LVM2}
  - minor_version           {#int}
  - volume_group_name       {#char[256]}
  - physical_volume_UUID    {#char[256]}
  - physical_volume_number  {#int}
  - useable_size            {#u8}           (total size in bytes)
  - labelone_sector         {#int}          (location of labelone)
  - meta_data_version       {#int}

  [linux.c]
*/

/* RAID Disk ()                                     Q55673155               {implemented}
    (Linux Raid Disk)
  - version         {#u4.#u4.#u4}
  - raid_level      {Multipath, 'HSM', 'translucent', Linear, RAID0, RAID1, RAID4(?), RAID5, Unknown}
  - regular_disks   {#int}          (number of regular disks)
  - spare_disks     {#int}          (number of spare disks)
  - UUID            {#char[256]}    (UUID of the whole RAID set)

  [linux.c]
*/

/* File format ()                                   Q235557
   - start_sector     {#u4}      (sector where the file starts)

*/



/* FAT (file system)                                Q190167
  + volume_name         (File System)
  + volume_size         (File System)
  + block_size  	    (File System)   (If identical to cluster_size, there's only one
                                         sector in each cluster.)
  - cluster_size        {#u8}           (Size of one cluster in bytes)
  - hints_score         {0, ..., 5}     (The higher, the more likely it's actually a FAT.)

  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

*/

/* FAT12 (FAT)                                      Q3063042                {implemented}
  + volume_name     (FAT)
  + volume_size     (FAT)
  + cluster_size    (FAT)
  + block_size      (FAT)
  + hints_score     (FAT)

  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [dos.c]
*/

/* FAT16 (FAT)                                      Q3141148                {implemented}
  + volume_name     (FAT)
  + volume_size     (FAT)
  + cluster_size    (FAT)
  + block_size      (FAT)
  + hints_score     (FAT)

  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [dos.c]
*/

/* FAT32 (FAT)                                      Q2622047                {implemented}
  + volume_name     (FAT)
  + volume_size     (FAT)
  + cluster_size    (FAT)
  + block_size      (FAT)
  + hints_score     (FAT)

  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [dos.c]
*/

/* Linux cramfs (file system)                       Q747406                 {implemented}
  + block_size  	    (File System)          {4096}
  + endianness          (File System)
  + volume_name         (File System)
  - start_sector        {#int}
  - compressed_size     {#u8}
  - data_size           {#u8}

  / volume_size         (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [linux.c]
*/

/* MBR partition table (partition table)            Q55357515               {implemented}
    (DOS/MBR partition map)
  / entries     (Partition Table)
  / sector_size (Partition Table)

  [dos.c]
*/

/* Ext2 (file system)                               Q283527                 {implemented}
  + volume_name         (File System)
  + UUID                (File System)
  + last_mounted        (File System)
  + volume_size         (File System)
  + block_size  	    (File System)

  / endianness          (File System)
  / version             (File System)
  / floppy_size         (File System)

  [linux.c, amiga.c]
*/

/* Ext3 (Journaling file system, file system)       Q283390                 {implemented}
  + volume_name     (Journaling file system)
  + UUID            (Journaling file system)
  + last_mounted    (Journaling file system)
  + volume_size     (Journaling file system)
  + block_size      (Journaling file system)

  / endianness      (Journaling file system)
  / version         (Journaling file system)
  / floppy_size     (Journaling file system)

  [linux.c]
*/

/* Ext3 external journal ()                         Q55505629               {implemented}
  - volume_name             {#char[256]}
  - UUID                    {#char[256]}
  - last_mounted            {#char[256]}     (location where it was last mounted)
  - volume_size             {#u8}
  - block_size              {#u8}

  [linux.c]
*/

/* Ext4 (Journaling file system, file system)        Q283827                {implemented}
  - development_version     {true, false}       (there's a preliminary development 
                                                 version of ext4)
  + volume_name     (Journaling file system)
  + UUID            (Journaling file system)
  + last_mounted    (Journaling file system)
  + volume_size     (Journaling file system)
  + block_size      (Journaling file system)

  / endianness      (Journaling file system)
  / version         (Journaling file system)
  / floppy_size     (Journaling file system)
  
  [linux.c]
*/

/* Journaling file system (file system)             Q579047
  + block_size  	    (File System)
  + volume_size         (File System)
  + volume_name         (File System)
  + UUID                (File System)
  + last_mounted        (File System)

  / endianness          (File System)
  / version             (File System)
  / floppy_size         (File System)
*/

/* Windows NTLDR (boot loader)                      Q1073789                {implemented}

  [dos.c]
 */

/* Windows 9x boot loader (boot loader)             Q55357282               {implemented}
    (Windows 95/98/ME boot loader)

  [dos.c]
*/

/* MS-DOS boot loader (boot loader)                 Q55357335               {implemented}
    (Windows / MS-DOS boot loader)

  [dos.c]
*/

/* FreeBSD boot loader (boot loader)                Q55357194               {implemented}
  - architecture    {i386}
  - boot_number     {0, 1, 2}          (Note that a boot_numer of 0 
                                     indicates the FreeBSD boot manager.)
  - sector          {0,2}               (location of the boot loader)

  [unix.c]
*/

/* BSD disklabel ()                                 Q1228785                {implemented}
  - entries         {#int}
  - sector_size     {#u4}

  [unix.c]
*/

/* Unix File System {UFS} (file system)             Q1046338                {implemented}
  + version             (File System)   {1, 2}
  + endianness          (File System)
  + last_mounted        (File System)
  + volume_name         (File System)   (volume name by FreeBSD convention)
  - volume_name_darwin  {#char[256]}    (volume name by Darwin convention)
  - offset              {#int}
  - long_file_names     {true}
  - fs_featurebits      {true}
  - support_4GB         {true}          (support for sizes larger than 4GB)

  / block_size  	    (File System)
  / volume_size         (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  

  [unix.c]
*/

/* GPT partition map (s_partition table)              Q603889               {implemented}
  + entries     (Partition Table)
  - disk_size   {#u8}   (total disk size in bytes)
  - sector_size {#u4}   (size of a single sector in bytes)
  - disk_GUID   {#char[256]}

  [dos.c]
*/

/* Apple HFS (file system)                          Q1058465                {implemented}
  + volume_name         (File System)   {#char[514]} 
  + volume_size         (File System)
  + block_size  	    (File System)
  - wrapper             {true} (is the fs a wrapper for HFS Plus?)

  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [apple.c]
*/

/* Apple HFS Plus (file system)                     Q1071337                {implemented}
  + volume_name         (File System)   {#char[514]} 
  + volume_size         (File System)
  + block_size  	    (File System)

  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [apple.c]
*/

/* ISO9660 (file system)                            Q55336682               {implemented}
  + volume_name         (File System)
  + volume_size         (File System)
  - publisher                        {#char[256]}
  - preparer                         {#char[256]}
  - application                      {#char[256]}
  - block_size                       {#u8}
  - joliet_extension                 {#char[256]}    (volume name if a joliet extension is recognized)

  - el_torito_boot_record            {#u8}           (catalog position if an El Torito boot record is found)


  - descriptor                       {additional_primary_volume_descriptor, 
                                      volume_partition_descriptor}           (contains the respective 
                                                                              descriptor if any is found)
  / block_size  	    (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [cdrom.c, amiga.c]
*/

/* Journaled File System {JFS} (file system)        Q1455872                {implemented}
  + version             (File System)
  + volume_name         (File System)
  + volume_size         (File System)
  + block_size  	    (File System)

  / endianness          (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [unix.c]
*/

/* MINIX (file system)                              Q685924                 {implemented}
  + version             (File System)
  + volume_size         (File System)
  + block_size  	    (File System)
  - name_size       {#int}

  / volume_name         (File System)
  / endianness          (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)


  [linux.c, amiga.c]
*/

/* Reiser4 (file system)                            Q2293002                {implemented}
  + volume_name         (File System)
  + UUID                (File System)
  + volume_size         (File System)
  + block_size  	    (File System)
  - layout          {4.0, Unknown}

  / endianness          (File System)
  / version             (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [linux.c]
*/

/* ReiserFS (Journaling file system, file system)   Q687074                 {implemented}
  
  + volume_name     (Journaling file system)
  + UUID            (Journaling file system)
  + volume_size     (Journaling file system)
  + block_size      (Journaling file system)
  - format              {3.5, 3.6}
  - standard_journal    {true, false}
  - start_position      {#int}          (start position in KiB)

  / last_mounted    (Journaling file system)
  / endianness      (Journaling file system)
  / version         (Journaling file system)
  / floppy_size     (Journaling file system)

  [linux.c]
*/

/* Linux romfs (file system)                        Q16963448               {implemented}
  + volume_name         (File System)     {#char[301]}
  + volume_size         (File System)

  / block_size  	    (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [linux.c]
*/

/* Linux squashfs (file system)                     Q389314                 {implemented}
  + version             (File System)
  + block_size  	    (File System)
  + endianness          (File System)
  - minor_version       {#int}
  - compressed_size     {#u8}

  / volume_size         (File System)
  / volume_name         (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [linux.c]
*/

/* Apple UDIF disk image (disk image file format)   Q14757791               {implemented}
  / start_sector (file format)

  [apple.c]
*/

/* Apple partition map (partition table)            Q375944                 {implemented}
  + entries (Partition Table)
  - version {old style, new style}

  [apple.c]
*/

/* XFS (file system)                                Q394011                 {implemented}
  + version             (File System)
  + volume_name         (File System)
  + UUID                (File System)
  + volume_size         (File System)
  + block_size  	    (File System)

  / endianness          (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [unix.c] 
*/

/* Macintosh File System (file system)              Q4043554                {implemented}

  / block_size  	    (File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [apple.c]
*/

/* Amiga rigid disk partition map (partition table) Q55357472               {implemented}
  - sector      {#int}      (the sector where the partition map is found)
  - sector_size {#u4}       (the size of a single block)

  [amiga.c]
*/

/* Amiga Old File System (file system)              Q4746198                {implemented}
  - intl                {true, false}
  - dir_cache           {true, false}
  - long_file_names     {true}
  - multiuser           {true, false}
  + floppy_size         (File System)

  / block_size  	    (File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / last_mounted        (File System)

  [amiga.c]
*/

/* Amiga Fast File System (file system)             Q370047                 {implemented}
  - intl                {true, false}
  - dir_cache           {true, false}
  - long_file_names     {true}
  - multiuser           {true, false}
  + floppy_size         (File System)

  / block_size  	    (File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / last_mounted        (File System)

  [amiga.c]
*/

/* Amiga Smart File System (file system)            Q1054031                {implemented}
  + floppy_size         (File System)

  / block_size  	    (File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / last_mounted        (File System)

  [amiga.c]
*/

/* Amiga Professional File System (file system)     Q7247965                {implemented}
  + version             (File System)
  + floppy_size         (File System)
  - SCSI_direct         {true}
  - multiuser           {true}

  / block_size  	    (File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / UUID                (File System)
  / last_mounted        (File System)

  [amiga.c]
*/

/* Amiga Ami-File Safe (file system)                Q55340903               {implemented}
    (Amiga AFS)
  - experimental        {true}
  + floppy_size         (File System)

  / block_size  	    (File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / last_mounted        (File System)

  [amiga.c]
*/

/* Boot disk ()                                     Q893130                 {implemented}
  - floppy_size         {DD, HD}        (size of a DD/HD floppy)

  [amiga.c]
*/

/* Berkeley Fast File System (file system)          Q2704864                {...}
    (actually just a different name for UFS (Unix File System))

  / block_size  	(File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [amiga.c]
*/

/* Audio CD (data format, s_compact disk)           Q1121020                {...}

  [amiga.c]
*/

/* High Sierra format (file system)                 Q5756978                {...}

  / block_size  	    (File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [amiga.c]
*/

/* Atari ST partition map (partition table)         Q55357630               {implemented}
  / entries     (Partition Table)
  / sector_size (Partition Table)

  [atari.c]
*/

/* Cloop image (archive file format, file format)   Q55340914               {implemented}
  - volume_size     {#u8}   (size in bytes)
  / start_sector (file format)

  [cloop.c]
*/

/* Universal Disk Format {UDF} (file system)        Q853645                 {implemented}
  + block_size  	    (File System)
  + volume_name         (File System)
  - version_hex     {#hexadecimal.hexadecimal}

  / volume_size         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [udf.c]
*/

/* BeOS File System {BeFS} (file system)            Q812816                 {implemented}
  - placement       {Apple, Intel}
  + endianness          (File System)
  + volume_name         (File System)
  + volume_size         (File System)
  + block_size  	    (File System)

  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [beos.c]
*/

/* Bootman (software)                               Q4035320                {implemented}
    (BeOS, yellowTab, Haiku boot loader)
  - system          {BeOS, Zeta, Haiku} (Bootman is used in different systems)

  [beos.c]
*/

/* Audio track (track)                              Q7302866                {implemented}
  + number      (Track)
  + size        (Track)

  - seconds         (total length in seconds)       {#int}

  [cdaccess.c]
*/

/* Track ()                                         Q7831478                {implemented}
  - number          (tracks are enumerated)         {#int}
  - size            (size in bytes)                 {#u8}

  [cdaccess.c]
 */

/* tar archive (archive file format)                Q283579                 {implemented}
  - version {GNU, POSIX, Pre-POSIX}
  / start_sector    (file format)

 [archives.c]
*/

/* Cpio archive (file format)                       Q285296                 {implemented}
  / start_sector (file format)
  - encoding {binary, ascii}
  - endianness {little, big}

 [archives.c]
*/

/* Broker archive {bar} (archive file format)       Q55357721               {implemented}
  / start_sector (file format)

 [archives.c]
*/

/* Raw CD image (s_file format)                     Q3930596                {implemented}
  / start_sector (file format)
  - mode    {1, 2}

  [cdimage.c]
*/

/* Opera file system (file system)                  Q7096591                {implemented}
     (3DO CD-ROM)

  / block_size  	    (File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [cdrom.c]
*/

/* FATX (file system)                               Q25397999               {implemented}
    (Xbox DVD file system)

  / block_size  	    (File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [cdrom.c]
*/

/* Compress file (file format)                      Q29209269               {implemented}
  + start_sector (file format)

  [compressed.c]
*/

/* gzip archive (file format)                       Q10287816               {implemented}
  + start_sector (file format)

  [compressed.c]
*/

/* bzip2 archive (file format)                      Q27866052               {implemented}
  + start_sector (file format)

  [compressed.c]
*/

/* Windows virtual PC disk image (disk image file format) Q55357928         {implemented}
   / start_sector (file format)
   - kind           {fixed size, dynamic size, differential, unknown kind}
   - disk_size      {#u8}

  [vpc.c]
*/

/* LILO boot loader (boot loader)                   Q861940                 {implemented}

  [linux.c]
*/

/* SYSLINUX boot loader (boot loader)               Q690646                 {implemented}

  [linux.c]
*/

/* GRUB boot loader (boot loader)                   Q212885                 {implemented}
 - compatibility_version   {#int.#int}
 - boot_drive              {0x#int in hexadecimal}
 - version                 {normal, LBA}

 [linux.c]
*/

/* Linux kernel built-in loader (boot loader)       Q55357412               {implemented}

  [linux.c]
*/

/* Debian split floppy (application)                Q55673179               {implemented}
  - name            {}
  - disk_number     {}      (Enumeration)
  - total_disks     {}      (Total number of split floppies)

  [linux.c]
*/

/* Xenix file system (file system)                  Q55340889               {implemented}
  + endianness          (File System)
  + block_size  	    (File System)

  / volume_size         (File System)
  / volume_name         (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [unix.c]
*/

/* Boot File System {BFS} (file system)             Q4943815                {implemented}
    (Boot file system, System V fs)
  + endianness          (File System)
  + block_size  	    (File System)

  / volume_size         (File System)
  / volume_name         (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [unix.c]
*/

/* Solaris disklabel (BSD disklabel)                Q55505218               {implemented}
  + entries     (BSD disklabel)
  + sector_size (BSD disklabel)

  - kind        {SPARC, vtoc x86}
  - version     {#u4}
  
  - volume_name {#char[256]}

  [unix.c]
*/

/* QNX4 File System (file system)                   Q7265501                {implemented}

  / block_size  	(File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [unix.c]
*/

/* Veritas VxFS (file system)                       Q2064372                {implemented}
  + version             (File System)
  + endianness          (File System)
  + volume_size         (File System)
  + block_size  	    (File System)
  
  / volume_name         (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [unix.c]
*/

/* High Performance File System {HPFS} (file system) Q127319                {implemented}
  + version             (File System)
  + volume_size         (File System)
  + block_size  	    (File System)
  - functional_version  {#int}

  / volume_name         (File System)
  / endianness          (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [dos.c]
*/

/* NTFS (file system)                               Q183205                 {implemented}
  + volume_size         (File System)
  + block_size  	    (File System)

  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [dos.c]
*/

/* ExFAT (file system)                              Q306233                 {implemented}

  / block_size  	(File System)
  / volume_size         (File System)
  / volume_name         (File System)
  / endianness          (File System)
  / version             (File System)
  / UUID                (File System)
  / floppy_size         (File System)
  / last_mounted        (File System)

  [dos.c]
*/

/* Amiga Amix (operating system)                    Q295179                 {implemented}

  [amiga.c]
*/

/* NetBSD (operating system)                        Q34225                  {implemented}
  - kind    {root, swap, other}

  [amiga.c]
*/

/* MS-DOS (operating system)                        Q47604                  {implemented}

  [amiga.c]
*/

/* BSD (Unix-like operating system, operating system) Q34264                {implemented}
  - version {4.1, 4.2}

  [archives.c]
*/

/* Linux (s_Unix-like operating system, s_operating system) Q388            {implemented}

  [amiga.c]
*/






/* DATA TYPES
 u4 = unsigned long int
 u8 = unsigned long long

 bsd_types = {
 "Unused",
"swap",
"Sixth Edition",
"Seventh Edition",
"System V",
"V7 with 1 KiB blocks",
"Eighth Edition, 4 KiB blocks",
"4.2BSD fast file system",
"ext2 or MS-DOS",
"4.4BSD log-structured file system",
"\"Other\"",
"HPFS",
"ISO9660",
bootstrap",
"AmigaDOS fast file system",
"Macintosh HFS",
"Digital Unix AdvFS",
"Unknown"
}

vtoc_types = {
"Unused",
"Boot",
"Root",
"Swap",
"Usr",
"Overlap",
"Stand",
"Var",
"Home",
"Alternate sector",
"Cache",
"Unknown"
}

gpt_types = {
"EFI System (FAT)",
"MBR partition scheme",
"MS Reserved",
"Basic Data",
"MS LDM Metadata",
"MS LDM Data",
"HP/UX Data",
"HP/UX Service",
"Linux RAID",
"Linux Swap",
"Linux LVM",
"Linux Reserved",
"FreeBSD Data",
"FreeBSD Swap",
"FreeBSD UFS",
"FreeBSD Vinum",
"Mac HFS+",
"Unknown"
}

mbr_types = {
"Empty",
"FAT12",
"XENIX root",
"XENIX usr",
"FAT16 <32M",
"Extended",
"FAT16",
"HPFS/NTFS",
"AIX",
"AIX bootable",
"OS/2 Boot Manager",
"Win95 FAT32",
"Win95 FAT32 (LBA)",
"Win95 FAT16 (LBA)",
"Win95 Ext'd (LBA)",
"OPUS",
"Hidden FAT12",
"Compaq diagnostics",
"Hidden FAT16 <32M",
"Hidden FAT16",
"Hidden HPFS/NTFS",
"AST SmartSleep",
"Hidden Win95 FAT32",
"Hidden Win95 FAT32 (LBA)",
"Hidden Win95 FAT16 (LBA)",
"NEC DOS",
"Plan 9",
"PartitionMagic recovery",
"Venix 80286",
"PPC PReP Boot",
"SFS / MS LDM",
"QNX4.x",
"QNX4.x 2nd part",
"QNX4.x 3rd part",
"OnTrack DM",
"OnTrack DM6 Aux1",
"CP/M",
"OnTrack DM6 Aux3",
"OnTrackDM6",
"EZ-Drive",
"Golden Bow",
"Priam Edisk",
"SpeedStor",
"GNU HURD or SysV",
"Novell Netware 286",
"Novell Netware 386",
"DiskSecure Multi-Boot",
"PC/IX",
"XOSL",
"Old Minix",
"Minix / old Linux",
"Linux swap / Solaris",
"Linux",
"OS/2 hidden C: drive",
"Linux extended",
"NTFS volume set",
"NTFS volume set",
"Linux LVM",
"Amoeba",
"Amoeba BBT",
"BSD/OS",
"IBM Thinkpad hibernation",
"FreeBSD",
"OpenBSD",
"NeXTSTEP",
"NetBSD",
"Mac OS X",
"BSDI fs",
"BSDI swap",
"Boot Wizard hidden",
"DRDOS/sec (FAT-12)",
"DRDOS/sec (FAT-16 < 32M)",
"DRDOS/sec (FAT-16)",
"Syrinx",
"Non-FS data",
"CP/M / CTOS / ...",
"Dell Utility",
"BootIt",
"DOS access",
"DOS R/O",
"SpeedStor",
"BeOS fs",
"EFI GPT protective",
"EFI System (FAT)",
"Linux/PA-RISC boot",
"SpeedStor",
"SpeedStor",
"DOS secondary",
"Linux raid autodetect",
"LANstep",
"BBT",
"Unknown"
}
*/



