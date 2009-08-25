cdef extern from "Caper.h":
    # Sequence type -> c_sequence
    ctypedef char * (*ToStringP)()
    ctypedef struct c_sequence "Sequence":
        ToStringP ToStringP

    # Mapping type -> c_mapping
    ctypedef struct c_mapping "Mapping":
        c_sequence * mSequence
        int Index

    # Mappings type -> c_mappings
    ctypedef int (*size)()
    ctypedef c_mapping * (*at)(int)
    ctypedef struct c_mappings "Mappings":
        size size
        at at

    void del_mappings "delete" (c_mappings * mappings)

    # SequenceEngine type -> c_sequence_engine
    ctypedef void (*Initialize)(char *)
    ctypedef struct c_sequence_engine "SequenceEngine":
        Initialize Initialize

    c_sequence_engine *new_sequence_engine "new FASequenceEngine" (char * path)
    void del_sequence_engine "delete" (c_sequence_engine * engine)

    # MappingEngine type -> c_mapping_engine
    ctypedef c_mappings * (*GetReads)(char *, int, int)
    ctypedef struct c_mapping_engine "MappingEngine":
        Initialize Initialize
        GetReads GetReads

    # create c_mapping_engine from MapviewMappingEngine objects.
    c_mapping_engine *new_mapping_engine "new MapviewMappingEngine" (char * path, c_sequence_engine * sequence_engine)
    void del_mapping_engine "delete" (c_mapping_engine * engine)

    # MapviewMappingsPreparer type -> c_mappings_preparer
    ctypedef char * (*PrepareMappingsPy)()
    ctypedef struct c_mappings_preparer "MapviewMappingsPreparer":
        PrepareMappingsPy PrepareMappingsPy

    c_mappings_preparer *new_mappings_preparer "new MapviewMappingsPreparer" (char * path)
    void del_mappings_preparer "delete" (c_mappings_preparer * prep)

# END of annoying Pyrex definitions.

###

cdef class mapping:
    """Class to contain set of mappings overlapping a particular point.

    Attributes: seqname, start, stop.

    __len__ returns number of mappings.
    __getitem__ returns (start, sequence) of overlapping mapping.
    """
    cdef c_mappings * mappings          # @CTB is this deallocated?
    cdef public char * seqname
    cdef public int start, stop

    def __cinit__(self, seqname, start, stop):
        self.seqname = seqname
        self.start = start
        self.stop = stop

    def __dealloc__(self):
        del_mappings(self.mappings)

    def __len__(self):
        """Return number of overlapping mappings at this point."""
        return self.mappings.size()

    def __getitem__(self, i):
        """Return (start, sequence) of overlapping mapping."""
        if i < 0 or i >= self.mappings.size():
            raise IndexError
        
        cdef c_mapping * q
        q = self.mappings.at(i)
        return q.Index, q.mSequence.ToStringP()

cdef class mapping_container:
    cdef c_sequence_engine *thisseq
    cdef c_mapping_engine *thismap

    def __cinit__(self, map_path, map_index, sequence_path, sequence_index):
        self.thisseq = NULL
        self.thismap = NULL

        self.thisseq = new_sequence_engine(sequence_path)
        self.thisseq.Initialize(sequence_index)

#        cdef c_mappings_preparer * map_prepper
#        map_prepper = new_mappings_preparer(map_path)
#        prep_path = map_prepper.PrepareMappingsPy()
#        del_mappings_preparer(map_prepper)

        self.thismap = new_mapping_engine(map_path, self.thisseq)
        self.thismap.Initialize(map_index)

    def get_reads_at(self, seqname, start, stop):
        cdef mapping x

        x = mapping(seqname, start, stop)
        # @CTB need to figure out how to pass this via mapping __cinit__
        x.mappings = self.thismap.GetReads(seqname, start, stop)
        return x
