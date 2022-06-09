# disktype_json

This is a modification of the disktype tool that yields results in JSON
instead of plain text. 
Like the original disktype tool, its purpose is to analyse a given file,
or disk and to detect content like file systems, partitions, boot loaders etc.



# Usage

Install disktype using gnu make.

Call the disktype tool with the file to be analysed as argument.
Use | json_pp for a formated output.

Check misc/file-system-sampler/ for some example images.

See web/doc/ and web/index.html for documentation about the original disktype
tool by Christoph Pfisterer.



# JSON output

disktype will yield a single JSON object, containing some general information
about the given file as well as a content list.

Each object in the content list (content object) represents a detected
structure. Each of those have a name and refer to a wikidata entry.
Furthermore, they have a set of properties and another content list,
containing content objects found within this structure and so on.

A list of all potentially detected content objects is found in
"list_of_content_objects".



# Properties

Each content object may have properties like the size of a file system
or the number of entries in a partition map.

A property constists of a name and a value of a specified domain.

Each content object type has some individual list of properties for all its
instances. Depending on disktypes discoveries not all of the properties will
actually be filled with values though.

To ease the use of properties, they will be inherited according to wikidata's
"instance of" and "subclass of" statements.

That says, a "ReiserFS" content object according to wikidata is an
instance of a "journaling file system" which again is a subclass of 
"file system".
Therefore a "ReiserFS" content object will share all the properties of 
"journaling file system" as well as "file system".
On top of that, it may have some individual properties.

A list of all potential properties for each content object type 
will also be provided.



# Comparison between JSON and plain text output

The global header file contains a statement "#define JSON".
Commenting this out and rebuilding the tool will make disktype yield 
its results in the old way, using plain text instead of JSON.

