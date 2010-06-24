cdef extern from "Caper.h":
    # Sequence class
    # declared as c_sequence

    ctypedef char * (*ToStringP)()

    ctypedef struct c_sequence "Sequence":
        ToStringP ToStringP

    # Mapping class
    # declared as c_mapping

    ctypedef struct c_mapping "Mapping":
        c_sequence * mSequence
        ToStringP NameP
        int Index

    void del_mapping "delete" (c_mapping * mapping)

    # ReadsAtIndex class
    # declared as c_reads_at_index (vector of Mapping*)

    ctypedef int (*size)()
    ctypedef c_mapping * (*at)(int)
    ctypedef void (*destroy)()

    ctypedef struct c_reads_at_index "ReadsAtIndex":
        size size
        at at
        destroy destroy

    void del_reads_at_index "delete" (c_reads_at_index * reads_at_index)

    # IndexedMappings class
    # declared as c_indexed_mappings (dictionary of ReadsAtIndex*)

    ctypedef c_reads_at_index * (*atindex)(int)

    ctypedef struct c_indexed_mappings "IndexedMappings":
       size size
       atindex atindex

    void del_indexed_mappings "delete" (c_indexed_mappings * indexed_mappings)

    # MappingEngine::iterator class
    # declared as c_mapping_engine_iterator

    ctypedef void (*next)()
    ctypedef void (*previous)()
    ctypedef void (*end)()
    ctypedef c_reads_at_index * (*get_reads)()
    ctypedef int (*get_index)()
    ctypedef c_indexed_mappings * (*intersect)()

    ctypedef struct c_mapping_engine_iterator "MappingEngine::iterator":
        next Next
        previous Previous
        end End
        get_index GetIndex
        get_reads GetReads
        intersect Intersect

    # MappingEngine class
    # declared as c_mapping_engine (only the bundle version :/)

    ctypedef void (*Initialize)()
    ctypedef c_mapping_engine_iterator (*begin)(char *)
    ctypedef c_mapping_engine_iterator (*at_iter)(char *, int)
    ctypedef c_mapping_engine_iterator (*end_iter)(char *)
    ctypedef c_reads_at_index * (*GetReads)(char *, int)
    ctypedef c_indexed_mappings * (*GetIntersection)(char *, int, int)
    ctypedef int (*GetReadLength)() 
   
    ctypedef struct c_mapping_engine "MappingEngine":
        Initialize Initialize
        begin Begin
        at_iter At
        end_iter End
        GetReads GetReads
        GetIntersection GetIntersection
        GetReadLength GetReadLength

    # create c_mapping_engine from MappingEngine objects
    c_mapping_engine *new_mapping_engine "new MappingEngine" (char * bundle_path)
    void del_mapping_engine "delete" (c_mapping_engine * engine)

    # MappingIndexer class
    # declared as c_mapping_indexer

    ctypedef enum c_mapping_file_format:
        DEFAULT
        MAPVIEW
        BOWTIE
        SAM

    ctypedef void (*CreateIndex)()
    ctypedef struct c_mapping_indexer "MappingIndexer":
        CreateIndex CreateIndex

    c_mapping_indexer *new_mapping_indexer "new MappingIndexer" (char * source_path, c_mapping_file_format format,  char * save_path, short bundle)
    void del_mappings_indexer "delete" (c_mapping_indexer * indexer)

### END Pyrex Definitions
#############################

###### BEGIN PYTHON CLASSES ######

## @RCK
## no idea what would be useful in this context.
## perhaps something that deals gracefully with the iterator?

####### END PYTHON CLASSES
