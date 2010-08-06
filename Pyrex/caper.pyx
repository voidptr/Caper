cdef extern from "Caper.h":
    # Sequence class
    # declared as c_sequence

    ctypedef char * (*ToStringP)()
    ctypedef int (*OrientationI)()
    ctypedef struct c_sequence "Sequence":
        ToStringP ToStringP
        OrientationI OrientationI

    # Mapping class
    # declared as c_mapping

    ctypedef struct c_mapping "Mapping":
        c_sequence * mSequence
        ToStringP NameP
        int Index
        OrientationI GetOrientation

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
    ctypedef struct c_reads_at_indexes "IndexedMappingsFlat":
        size size
        at_reads at

    void del_reads_at_indexes "delete" (c_reads_at_indexes * reads_at_indexes)

    # MappingEngine::iterator class
    # declared as c_mapping_engine_iterator

    ctypedef void (*next)()
    ctypedef void (*previous)()
    ctypedef void (*end)()
    ctypedef c_reads_at_index * (*get_reads)()
    ctypedef c_reads_at_indexes * (*get_reads_indexed_flat)()
    ctypedef int (*get_index)()
    ctypedef c_reads_at_indexes * (*intersect_flat)()
    ctypedef struct c_mapping_engine_iterator "MappingEngine::iterator":
        next Next
        previous Previous
        end End
        get_index GetIndex
        get_reads GetReads
        intersect_flat IntersectFlat
        get_reads_indexed_flat GetReadsIndexedFlat

    void del_mapping_engine_iterator "delete" (c_mapping_engine_iterator * iterator)

    # MappingEngine class
    # declared as c_mapping_engine (only the bundle version :/)

    ctypedef void (*Initialize)()
    ctypedef c_mapping_engine_iterator * (*at_iter)(char *, int)
    ctypedef c_reads_at_index * (*GetReads)(char *, int)
    ctypedef c_reads_at_indexes * (*GetIntersectionFlat)(char *, int, int)
    ctypedef int (*GetReadLength)()

    ctypedef struct c_mapping_engine "MappingEngine":
        Initialize Initialize
        at_iter AtPtr
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

cdef class iterator:
    """Iterates through the genome, one index at a time, intersecting at each index, returning one read at a time."""
    cdef c_mapping_engine_iterator * thisiterator
    cdef public char * seqname
    cdef public int start_genome_position, stop_genome_position, current_genome_position, reverse, cache_normalized_position, read_number, first_time

    cdef c_reads_at_indexes * cache ## the cache of the set of mappings at the given index.

    def __cinit__(self, seqname, start, stop, reverse):
        self.seqname = seqname ## the name of the contig
        self.start_genome_position = start ## where in the genome we want to start.
        self.current_genome_position = start
        self.stop_genome_position = stop ## where should we stop.
        self.reverse = reverse ## whether we should be moving in reverse ## TODO, THIS DOES NOTHING
        self.first_time = 1

    def __iter__(self):
        return self

    def __repr__(self):
        return "iterator('%s', %d, %d)" % (self.seqname, self.start, self.current)

    def __next__(self):
        if (self.first_time == 1):
            self.first_time = 0
            self.init_cache()

        ## are we at the end of the line? (this is the END)
        if (self.current_genome_position < 0):
            raise StopIteration
        ## have we overrun the stop (assuming one was defined)?
        if (self.stop_genome_position > -1 and self.current_genome_position > self.stop_genome_position):
            raise StopIteration

        ## since we aren't at the end of the line, the cache MUST have a value in it.
        ## obtain the current value.
        assert self.read_number >= 0
        assert self.cache_normalized_position >= 0
        cdef c_mapping * item
        item = self.cache.at(self.cache_normalized_position).at(self.read_number) ## this should always have a value

        current_index = self.current_genome_position

        ## then iterate.
        self.move_next()

        ## return the value
        name = item.NameP()
        index = item.Index
        seq = item.mSequence.ToStringP()
        orientation = item.GetOrientation()

        if ( index + len(seq) <= current_index ):
            return self.next()

        return (index, name, seq, orientation) # we don't know anything about slicing in this context.

    def move_next(self):
        self.read_number += 1

        if (self.read_number >= self.cache.at(self.cache_normalized_position).size()): ## exhausted this inner list
            self.cache_normalized_position += 1 ## start at the next list.
            self.read_number = 0

            if (self.cache_normalized_position >= self.cache.size()): ## just exhausted the outer list too
                self.thisiterator.Next() ## increment the iterator
                self.current_genome_position = self.thisiterator.GetIndex() ## assign the index number
                self.cache = self.thisiterator.GetReadsIndexedFlat()
                if (self.cache.size() > 0): ## we got something!
                    self.cache_normalized_position = 0
                    self.read_number = 0
                else: ## um, there's nothing in here. (this triggers an assert during the next go-round)
                    self.cache_normalized_position = -1
                    self.read_number = -1

    def init_cache(self): ## so, we were given an index, which may or may not be in a no-man's land
        ## the iterator is already set to a location. Let's try querying it.
        self.cache = self.thisiterator.IntersectFlat()

        if (self.cache.size() == 0): ## no-man's land. Automatically advance to the next genome position with reads on it.
            self.thisiterator.Next() ## if there's something in this contig, we'll get set there.
            self.current_genome_position = self.thisiterator.GetIndex() ## if there not something left in this contig, this'll be set to -1
            self.cache = self.thisiterator.GetReadsIndexedFlat() ## and this will be empty...
            if (self.cache.size() > 0): ## we got something!
                self.cache_normalized_position = 0
                self.read_number = 0
            else: ## um, there's nothing in here.
                self.cache_normalized_position = -1
                self.read_number = -1
        else:
            self.cache_normalized_position = 0
            self.read_number = 0


cdef class mappingsinterval:
    """Two-dimensional dictionary of mappings (the collection of mappings at those indexes)"""
    #cdef c_reads_at_indexes * mappings ## the set of mappings that overlap the given index
    cdef c_mapping_engine_iterator * thisiterator

    cdef public char * seqname ## name of the contig
    cdef public int slice_start, slice_stop ## beginning and end of the interval (umm, does this make sense?)
    cdef public int intersect, reverse ## intersection fetching, or reverse.

    #cdef int current ## hey, I'm the current index. :P

    def __cinit__(self, seqname, slice_start, slice_stop, reverse): #, indexed_mappings): ## populating the info stuff
        self.seqname = seqname
        self.slice_start = slice_start
        self.slice_stop = slice_stop
        self.reverse = reverse

## TODO, add a deallocator for the iterator...
#    def __dealloc__(self):
#        cdef c_reads_at_indexes * mappings
#        mappings = self.mappings
#        del_reads_at_indexes(mappings) ## this should work, since it should call the right destructor... maybe.

    def __repr__(self):
        return "mappingsinterval('%s', %d, %d)" % (self.seqname, self.slice_start, self.slice_stop)

    def __iter__(self):
        cdef iterator x
        x = iterator(self.seqname, self.slice_start, self.slice_stop, self.reverse)
        x.thisiterator = self.thisiterator
        return x

#     def __len__(self): ## this probably doesn't work.
#         """Return number of overlapping mappings at this point."""
#         return self.mappings.size() ##this isn't correct. :(

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

    def get_reads(self, seqname, start, stop=-1, reverse=0):
        cdef mappingsinterval container
        container = mappingsinterval(seqname, start, stop, reverse)
        container.thisiterator = self.thismap.AtPtr(seqname, start)
        return container

####### END PYTHON CLASSES
