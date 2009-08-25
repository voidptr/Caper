cdef extern from "Caper.h":
    ctypedef void (*Initialize)()
    ctypedef struct c_sequence_engine "SequenceEngine":
        Initialize Initialize

    c_sequence_engine *new_sequence_engine "new FASequenceEngine" (char * path)
    void del_sequence_engine "delete" (c_sequence_engine * engine)

    ###

    ctypedef struct c_mapping_engine "MappingEngine":
        Initialize Initialize

    c_mapping_engine *new_mapping_engine "new MapviewMappingEngine" (char * path, c_sequence_engine * sequence_engine)
    void del_mapping_engine "delete" (c_mapping_engine * engine)

cdef class genome:
    cdef c_sequence_engine *thisptr

    def __cinit__(self, filename):
        self.thisptr = new_sequence_engine(filename)

    def initialize(self):
        self.thisptr.Initialize()

cdef class mapping:
    cdef c_sequence_engine *thisseq
    cdef c_mapping_engine *thismap

    def __cinit__(self, map_path, sequence_path):
        self.thisseq = new_sequence_engine(sequence_path)
        self.thisseq.Initialize()
        self.thismap = new_mapping_engine(map_path, self.thisseq)
        self.thismap.Initialize()
