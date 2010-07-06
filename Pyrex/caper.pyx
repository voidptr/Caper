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
        destroy Destroy

    void del_reads_at_index "delete" (c_reads_at_index * reads_at_index)

    # vector<ReadsAtIndex*> class
    # declared as c_reads_at_indexes (vector of ReadsAtIndex*)

    ctypedef c_reads_at_index * (*at_reads)(int)
    ctypedef struct c_reads_at_indexes "vector<ReadsAtIndex*>":
        size size
        at_reads at

    void del_reads_at_indexes "delete" (c_reads_at_indexes * reads_at_indexes)

    # MappingEngine::iterator class
    # declared as c_mapping_engine_iterator

    ctypedef void (*next)()
    ctypedef void (*previous)()
    ctypedef void (*end)()
    ctypedef c_reads_at_index * (*get_reads)()
    ctypedef int (*get_index)()
    ctypedef c_reads_at_indexes * (*intersect_flat)()
    ctypedef struct c_mapping_engine_iterator "MappingEngine::iterator":
        next Next
        previous Previous
        end End
        get_index GetIndex
        get_reads GetReads
        intersect_flat IntersectFlat

    void del_mapping_engine_iterator "delete" (c_mapping_engine_iterator * iterator)

    # MappingEngine class
    # declared as c_mapping_engine (only the bundle version :/)

    ctypedef void (*Initialize)()
    ctypedef c_mapping_engine_iterator (*begin)(char *)
    ctypedef c_mapping_engine_iterator * (*at_iter)(char *, int)
    ctypedef c_mapping_engine_iterator (*end_iter)(char *)
    ctypedef c_reads_at_index * (*GetReads)(char *, int)
    ctypedef c_reads_at_indexes * (*GetIntersectionFlat)(char *, int, int)
    ctypedef int (*GetReadLength)()

    ctypedef struct c_mapping_engine "MappingEngine":
        Initialize Initialize
        begin Begin
        at_iter At
        end_iter End
        GetReads GetReads
        GetIntersectionFlat GetIntersectionFlat
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

cdef class mappings:
    """List of mappings at a particular index"""
    cdef c_reads_at_index * mappings ## a simple collection of mappings, but they identify themselves by their index.
    cdef public char * seqname ## the name of the contig
    cdef public int index ## the index that I am

    cdef int current ## hey, I'm the current index. :P

    def __cinit__(self, seqname, index): #, c_reads_at_index * reads_at_index): ## __cinit__ can only take python parameters. :(
        self.seqname = seqname
        self.index = index

    def __dealloc__(self):
        self.mappings.Destroy()

    def __repr__(self): ## what does this do?
        return "mappings('%s', %d)" % (self.seqname, self.index)

    def __len__(self): ## this probably doesn't work.
        """Return number of overlapping mappings at this point."""
        return self.mappings.size() ##

    def __getitem__(self, i):
        """Return (start, sequence) of overlapping mapping."""
        if i < 0 or i >= self.mappings.size():
            raise IndexError

        cdef c_mapping * item
        item = self.mappings.at(i) ## don't know if this still works. Does "at" still work?

        name = item.NameP()
        index = item.Index

        return index, name # we don't know anything about slicing in this context.

cdef class mappingsinterval:
    """Two-dimensional dictionary of mappings (the collection of mappings at those indexes)"""
    cdef c_reads_at_indexes * mappings ## the set of mappings that overlap the given index
    cdef public char * seqname ## name of the contig
    cdef public int slice_start, slice_stop ## beginning and end of the interval (umm, does this make sense?)

    cdef int current ## hey, I'm the current index. :P

    def __cinit__(self, seqname, slice_start, slice_stop): #, indexed_mappings): ## populating the info stuff
        self.seqname = seqname
        self.slice_start = slice_start
        self.slice_stop = slice_stop

    def __dealloc__(self):
        cdef c_reads_at_indexes * mappings
        mappings = self.mappings
        del_reads_at_indexes(mappings) ## this should work, since it should call the right destructor... maybe.

    def __repr__(self): ## what does this do?
        return "mappingsinterval('%s', %d, %d)" % (self.seqname, self.start, self.stop)

    def __len__(self): ## this probably doesn't work.
        """Return number of overlapping indexes at this point."""
        return self.mappings.size() ##

    def __getitem__(self, i):
        """Return (start, sequence) of overlapping mapping."""
        if i < 0 or i >= self.mappings.size():
            raise IndexError

        cdef c_reads_at_index * item
        item = self.mappings.at(i)

        cdef mappings reads
        reads = mappings(self.seqname, i)
        reads.mappings = item

        return reads # we don't know anything about slicing in this context.


# cdef class slice_iterator:
#     """Iterates through a slice of the genome."""
#     cdef c_mapping_engine_iterator thisiterator
#     cdef public int start, current
#
#     def __cinit__(self, start):
#         self.start = start
#         self.current = start
#
#     cdef init(self, c_mapping_engine_iterator iterator):
#         self.thisiterator = iterator;
#
#     def next(self):
#         self.thisiterator.Next()
#         self.current += self.thisiterator.GetIndex()
#
#     def get_reads(self):
#         cdef c_indexed_mappings * reads
#         if self.current != self.start:
#             reads = self.thisiterator.Intersect()
#         else:
#             reads = self.thisiterator.GetReadsIndexed()
#
#         cdef mappingsinterval x
#         x.mappings = reads
#
#         return x
#
#cdef class iterator:
#    """Iterates through the genome, one index at a time."""
#    cdef c_mapping_engine_iterator * thisiterator
#    cdef public char * seqname
#    cdef public int start

#    def __cinit__(self, seqname, start):
#        self.start = start
 #       self.seqname = seqname
#
#     cdef init(self, c_mapping_engine_iterator iterator):
#         self.thisiterator = iterator;
#
#     def next(self):
#         self.thisiterator.Next()
#         self.current += self.thisiterator.GetIndex()
#
#     def get_reads(self):
#         cdef c_reads_at_index * reads
#         reads = self.thisiterator.GetReads()
#
#         cdef mappings x ## this class init is missing some crucial bits...
#         x.mappings = reads
#
#         return x
#
#     def get_intersection(self):
#         cdef c_indexed_mappings * reads
#         reads = self.thisiterator.Intersect()
#
#         cdef mappingsinterval x
#         x.mappings = reads
#
#         return x

cdef class mapping_container:
    cdef c_mapping_engine *thismap

    def __cinit__(self, bundle_path):
        self.thismap = NULL
        self.thismap = new_mapping_engine(bundle_path)
        self.thismap.Initialize()

    def __dealloc__(self):
        del_mapping_engine(self.thismap)

    def get_read_length(self):
        length = self.thismap.GetReadLength()
        return length

#    def get_iterator(self, seqname, start):
#        cdef iterator x
#        x.start = start
#        x.seqname = seqname
#        x.thisiterator = self.thismap.At(seqname, start);
#        return x

# #     def get_slice_iterator(self, seqname, start):
# #         cdef slice_iterator x
# #         x.start = start
# #         x.seqname = seqname
# #         x.init(self.thismap.At(seqname, start))
# #         return x
#
    def get_intersect(self, seqname, index):
        cdef mappingsinterval reads
        reads = mappingsinterval(seqname, index, index)
        reads.mappings = self.thismap.GetIntersectionFlat(seqname, index, index)
        return reads

    def get_slice(self, seqname, left, right ):
        cdef mappingsinterval reads
        reads = mappingsinterval(seqname, left, right)
        reads.mappings = self.thismap.GetIntersectionFlat(seqname, left, right)
        return reads

    def get_reads(self, seqname, index):
        cdef mappings reads
        reads = mappings(seqname, index)
        reads.mappings = self.thismap.GetReads(seqname, index)
        return reads

####### END PYTHON CLASSES
