cdef extern from "Caper.h":
    # Sequence type -> c_sequence
    ctypedef char * (*ToStringP)()
    ctypedef struct c_sequence "Sequence":
        ToStringP ToStringP

    # Mapping type -> c_mapping
    ctypedef struct c_mapping "Mapping":
        c_sequence * mSequence
        ToStringP NameP
        int Index

    # Mappings type -> c_mappings
    ctypedef int (*size)()
    ctypedef c_mapping * (*at)(int)
    ctypedef struct c_mappings "Mappings":
        size size
        at at

    void del_mapping "delete" (c_mapping * mapping)
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
        cdef int i
        cdef c_mappings * mappings
        
        mappings = self.mappings
        i = 0
        n_mappings = mappings.size()
        
        while i < n_mappings:
            del_mapping(mappings.at(i))
            i += 1
            
        del_mappings(self.mappings)

    def __repr__(self):
        return "mapping('%s', %d, %d)" % (self.seqname, self.start, self.stop)

    def __len__(self):
        """Return number of overlapping mappings at this point."""
        return self.mappings.size()

    def __getitem__(self, i):
        """Return (start, sequence) of overlapping mapping."""
        if i < 0 or i >= self.mappings.size():
            raise IndexError
        
        cdef c_mapping * q
        q = self.mappings.at(i)

        seq = q.mSequence.ToStringP()
        seq_len = len(seq)
        seq_start = q.Index
        seq_stop = seq_start + seq_len
        name = q.NameP()

        slice_start = 0
        align_start = seq_start
        if seq_start < self.start:
            slice_start = self.start - seq_start
            align_start = self.start

        slice_stop = seq_len
        align_stop = seq_stop
        if seq_stop > self.stop:
            slice_stop = seq_len - (seq_stop - self.stop)
            align_stop = self.stop

        return align_start, align_stop, name, slice_start, slice_stop

cdef class mapping_container:
    cdef c_sequence_engine *thisseq
    cdef c_mapping_engine *thismap
    cdef int read_length

    def __cinit__(self, map_path, map_index, sequence_path, sequence_index,
                  read_length):
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

        self.read_length = read_length  # @CTB can retrieve from thismap

    def __dealloc__(self):
        del_mapping_engine(self.thismap)
        del_sequence_engine(self.thisseq)

    def get_reads_at(self, seqname, start, stop):
        cdef mapping x

        x = mapping(seqname, start, stop)
        # @CTB need to figure out how to pass this via mapping __cinit__
        # @CTB hack hack hack - coordinate munging!
        start = max(0, start - self.read_length)
        x.mappings = self.thismap.GetReads(seqname, start, stop - 1)
        return x
