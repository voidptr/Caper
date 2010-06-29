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

    # IndexedMappings class
    # declared as c_indexed_mappings (dictionary of ReadsAtIndex*)

    ctypedef c_reads_at_index * (*atindex)(int)
    ctypedef struct c_indexed_mappings "IndexedMappings":
       size size
       atindex at

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

# cdef class mappings:
#     """List of mappings at a particular index"""
#     cdef c_reads_at_index * mappings ## a simple collection of mappings, but they identify themselves by their index.
#     cdef public char * seqname ## the name of the contig
#     cdef public int index ## the index that I am
#
#     def __cinit__(self, seqname, index): #, c_reads_at_index * reads_at_index): ## __cinit__ can only take python parameters. :(
#         self.seqname = seqname
#         self.index = index
#         #self.mappings = reads_at_index
#
#     cdef init(self, c_reads_at_index * reads_at_index): ## because of the above, here's a factory method. :(
#         self.mappings = reads_at_index
#
#     def __dealloc__(self):
#         self.mappings.Destroy()
#
#     def __repr__(self): ## what does this do?
#         return "mappings('%s', %d)" % (self.seqname, self.index)
#
#     def __len__(self): ## this probably doesn't work.
#         """Return number of overlapping mappings at this point."""
#         return self.mappings.size() ##
#
#     def __getitem__(self, i):
#         """Return (start, sequence) of overlapping mapping."""
#         if i < 0 or i >= self.mappings.size():
#             raise IndexError
#
#         cdef c_mapping * item
#         item = self.mappings.at(i) ## don't know if this still works. Does "at" still work?
#
#         name = item.NameP()
#         index = item.Index
#
#         return index, name # we don't know anything about slicing in this context.
#
# cdef class mappingsinterval:
#     """Two-dimensional dictionary of mappings (key is index, and the value is the collection of mappings at that index)"""
#     cdef c_indexed_mappings * mappings ## the set of mappings that overlap the given index
#     cdef public char * seqname ## name of the contig
#     cdef public int slice_start, slice_stop ## beginning and end of the interval (umm, does this make sense?)
#
#     def __cinit__(self, seqname, slice_start, slice_stop): #, indexed_mappings): ## populating the info stuff
#         self.seqname = seqname
#         self.start = start
#         self.stop = stop
#
#     cdef init(self, c_indexed_mappings * indexed_mappings):
#         self.mappings = indexed_mappings
#
#     def __dealloc__(self):
#         cdef c_indexed_mappings * mappings
#         mappings = self.mappings
#         del_indexed_mappings(mappings) ## this should work, since it should call the right destructor... maybe.
#
#     def __repr__(self): ## what does this do?
#         return "mappingsinterval('%s', %d, %d)" % (self.seqname, self.start, self.stop)
#
#     def __len__(self): ## this probably doesn't work.
#         """Return number of overlapping mappings at this point."""
#         return self.mappings.size() ##

#    def __getitem__(self, key): ## fetch the set of mappings of a requested item
#        """Return (start, sequence) of overlapping mapping."""
#        if i < 0 or i >= self.mappings.size():
#             raise IndexError
#
#         cdef c_reads_at_index * q ## this is an array
#         q = self.mappings.at(i) ## don't know if this still works. like, what the hell does pyrex do with maps? Does "at" still work?
#
#         seq = q.mSequence.ToStringP()
#         seq_len = len(seq)
#         seq_start = q.Index
#         seq_stop = seq_start + seq_len
#         name = q.NameP()
#
#         slice_start = 0
#         align_start = seq_start
#         if seq_start < self.start:
#             slice_start = self.start - seq_start
#             align_start = self.start
#
#         slice_stop = seq_len
#         align_stop = seq_stop
#         if seq_stop > self.stop:
#             slice_stop = seq_len - (seq_stop - self.stop)
#             align_stop = self.stop
#
#         return align_start, align_stop, name, slice_start, slice_stop
#
# ## the iterator
# cdef class mapping_iterator:
#     cdef c_mapping_engine_iterator *thisiterator
#     cdef public char * seqname
#
#     def __cinit__(self, iterator, seqname):
#         self.thisiterator = NULL
#         self.thisiterator = iterator
#         self.seqname = contig
#
#     #def __dealloc__(self): ## there's no deallocator for the iterator. :/ might want to add something
#
#     def next(self):
#         self.thisiterator.Next()
#
#     def previous(self):
#         self.thisiterator.Previous()
#
#     def end(self):
#         self.thisiterator.End()
#
#     def intersect(self):
#         cdef int index
#         index = self.thisiterator.GetIndex()
#
#         cdef c_indexed_mappings * mappings
#         mappings = self.thisiterator.Intersect()
#
#         cdef mapping x
#         x = mapping(self.seqname, index, index, mappings)
#         return x
#
# cdef class slice_iterator:
#     """Iterates through a slice of the genome."""
#     cdef c_mapping_engine_iterator *thisiterator
#     cdef public int start, current
#
#     def __cinit__(self, start):
#         self.start = start
#         self.current = start
#
#     cdef init(self, c_mapping_engine_iterator * iterator):
#         self.thisiterator = iterator;
#
#     def next(self):
#         self.thisiterator.Next()
#         self.current += self.thisiterator.GetIndex()

#     def get_reads(self):
#         cdef c_indexed_mappings * mappings
#         if self.current != self.start:
#             mappings = self.thisiterator.Intersect()
#         else:
#             mappings = self.thisiterator.GetReads()
#
#         return mappings


cdef class mapping_container:
    cdef c_mapping_engine *thismap

    def __cinit__(self, bundle_path):
        self.thismap = NULL
        self.thismap = new_mapping_engine(bundle_path)
        self.thismap.Initialize()

    def __dealloc__(self):
        del_mapping_engine(self.thismap)

    #def get_slice_iterator(self, seqname, start):



####### END PYTHON CLASSES
