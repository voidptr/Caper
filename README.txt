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

     ~~OR~~,

   -b <path_to_file> : path to compressed bundle file.

Interactive Commands:
  In interactive mode, Caper accepts the following commands

    interval <contig> <start_index> <end_index>
        - returns the reads that intersect with the interval

    intersect <contig> <index>
        - returns the reads that intersect with this index.

    reads <contig> <index>
        - returns the reads whose alignment STARTS at this index.


Build Instructions (Linux):

  Build Executable:
    From the source distribution root, run the following commands:

      %% cd CaperGCC/
      %% make

  Build Pyrex Wrapper:
    Verify that Python (2.6 or later) and Pyrex are installed.
    From the source distribution root, run the following commands:

      %% cd Pyrex
      %% python setup.py build_ext --inplace

  Run Tests:
    Verify that:
      1. Python (2.6 or later) and Pyrex are installed
      2. Screed, Pygr, and Nose are installed.
        2a. Screed is in your Python path.
      3. You have built the Pyrex Wrapper.

    From the source distribution root, run the following commands:

      %% cd Pyrex/
      %% nosetests

Build Instructions (Windows*):

To build under Windows, and Visual Studio 2010:
  Open CaperVS/CaperVS.sln
  Re-build the project.

*Neither the Pyrex wrapper or tests were developed or tested under Windows.
If you feel like being awesome, patches for Windows-based Python wrappers and tests would be greatly appreciated.







TODO
- beef up the test suite
