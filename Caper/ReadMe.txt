Caper v0.5
Usage:
 caper indexmappings [-b] <-m mappingfile> <-t bowtie|mapview|sam> <-o savepath>
 caper interactive <-m indexedmappingfile -i mappingfileindex|-b compressedmappingbundle>

Modes:
 indexmappings: read and sort a Bowtie, Mapview, or Sam formatted mapping file, and produce an index.

  -b : Produce a compressed bundle containing the mapping file and index.
  -m <path_to_file> : Path to mapping file
  -t <format> : Mapping file format (bowtie, mapview, or sam)
  -o <output_path> : Path to output directory. If path is not a directory, will attempt to use as prefix.

  interactive: interactively query indexed mapping file
  -m <path_to_file> : path to indexed mapping file
  -i <path_to_file> : path mapping file index

     alternatively,

  -b <path_to_file> : path to compressed bundle file.

Interactive Commands:

In interactive mode, the Caper accepts the following commands

interval <contig> <start_index> <end_index>
    - returns the reads that intersect with the interval 

intersect <contig> <index>
    - returns the reads that intersect with this index.

reads <contig> <index>
    - returns the reads whose alignment STARTS at this index. 


TODO
- finish populating Pyrex wrapper
- beef up the test suite
