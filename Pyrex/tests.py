# @CTB to test: do our own mapping parsing & verify against caper, too.
# @CTB refactor test_bridge_nlmsa_equiv to use the new pygr ival construction
# lib code.
import os.path
from nose import SkipTest

try:
    import screed
    from screed.pygr_api import ScreedSequenceDB
except ImportError:
    pass

import caper
from caper_pygr_bridge import CaperBridge
from pygr import cnestedlist, seqdb

sequence_path = 'data/REL606.gmc.fa'
sequence_fastq = 'data/REL606-seqs.fastq'

contig = 'rel606'

#genome_index = 'data/cache/REL606.gmc.fa.genomeindex'
#sequence_index = 'data/cache/REL606.gmc.fa.indexed'

#map_path = 'data/REL606-maq-map.txt'
#map_index = 'data/cache/REL606-maq-map.txt.mappingindex'

map_bundle = 'data/cache/REL606-maq-map.txt.bundle'

def setup():
#    assert os.path.exists(map_index), "run data/build-indexes.sh, please!"
#    assert os.path.exists(sequence_index), "run data/build-indexes.sh, please!"
    assert os.path.exists(map_bundle), "run data/build-indexes.sh, please!"

###

"""
The REL606 genome, from index 0-76, along with reads mapped to indexes from 2(1) to 42(41), looks as follows:

                                                       1111111111222222222233333333334444444444555555555566666666667777777
                                             01234567890123456789012345678901234567890123456789012345678901234567890123456
                                             ^         ^         ^         ^         ^         ^         ^         ^
                                             agcttttcattctgactgcaacgggcaatatgtctctgtgtggattaaaaaaagagtgtctgatagcagcttctgaa
HWI-EAS_4_PE-FC20GCB:1:146:930:514/1    2 (1) gcttttcattctgactgcaacgggcaatatgtctct
HWI-EAS_4_PE-FC20GCB:1:210:846:698/1    4 (3)   ttttcattctgactgcaacgggcaatatgtctctgt
HWI-EAS_4_PE-FC20GCB:1:285:772:762/1    5 (4)    tttcattctgactgcaacgggcaatatgtctctgtg
HWI-EAS_4_PE-FC20GCB:1:172:430:569/1    11 (10)        tctgactgcaacgggcaatatgtctctgtgtggatt
HWI-EAS_4_PE-FC20GCB:1:1:563:116/1      12 (11)         ctgactgcaacgggcaatatgtctctgtgcagaata
HWI-EAS_4_PE-FC20GCB:1:113:162:700/1    13 (12)          tgactgcaacgggcaatatgtctctgtgtggattaa
HWI-EAS_4_PE-FC20GCB:1:212:153:213/1    14 (13)           gactgcaacgggcaatatgtctctgtgtggattaaa
HWI-EAS_4_PE-FC20GCB:1:246:99:950/1	    14 (13)           gactgcaacgggcaatatgtctctgcgtggattcaa
HWI-EAS_4_PE-FC20GCB:1:278:188:572/1    19 (18)                caacgggcaatatgtctctgtgtggattaaaaaaag
HWI-EAS_4_PE-FC20GCB:1:267:373:737/1    20 (19)                 aacgggcaatatgtctctgtgtggattaaaaaaaga
HWI-EAS_4_PE-FC20GCB:1:306:568:350/1    26 (25)                       aaatatgtctctgtgtggattaaaaaaagagtgtct
HWI-EAS_4_PE-FC20GCB:1:14:877:706/1	    27 (26)                        aatatgtctctgtgtggattaaaaaaagagtgtctg
HWI-EAS_4_PE-FC20GCB:1:65:981:916/1	    28 (27)                         atatgtctctgtgtggattaaaaaaagagtgtctga
HWI-EAS_4_PE-FC20GCB:1:163:985:583/1    30 (29)                           atgtctctgtgtggattaaaaaaagagtgtctgata
HWI-EAS_4_PE-FC20GCB:1:180:581:274/1    34 (33)                               ctctgtatggattaaaaaaagagtgtctgatagcag
HWI-EAS_4_PE-FC20GCB:1:160:122:568/1    38 (37)                                   gtgtggattaaaaaaagagtgtctgatagcagcttc
HWI-EAS_4_PE-FC20GCB:1:23:564:736/1	    39 (38)                                    tgtggattaaaaaaagagtgtctgatagcagcttct
HWI-EAS_4_PE-FC20GCB:1:166:896:40/1	    39 (38)                                    tgtggattaaaaaaagagtgtctgatagcagcttct
HWI-EAS_4_PE-FC20GCB:1:133:865:549/1    42 (41)                                       ggattaaaaaaagagtgtctgatagcagtttttgaa

A single read 36bp in length:

          11111111112222222222333333
012345678901234567890123456789012345
ggattaaaaaaagagtgtctgatagcagtttttgaa


111111111111111111111111111111111111
333333333333333333333333333333333333
777777777777777777777888888888888888
788888888889999999999          11111
901234567890123456789012345678901234
ccatctgctgcacagcacccgtaagcaggttgaaga
"""

class MappingContainer_Test(object):
    def setup(self):
        self.cont = caper.mapping_container(map_bundle)
        self.db = seqdb.SequenceFileDB(sequence_path)
        self.seq = self.db[contig]

    def test_read_length(self):
        """Test the read length"""
        length = self.cont.get_read_length()
        assert length == 36

    def test_intersect_borders(self):
        """Test that the intersect returns different results when a read drops off the front, and hops on the back"""
        inter36 = self.cont.get_intersect(contig, 36)
        inter37 = self.cont.get_intersect(contig, 37)

        ## the first results shouldn't be the same, since we drop the read at index 1
        assert inter36[0][4][0][0] != inter37[0][4][0][0]

        ## the last result shouldn't be the same, since we pick up a new read at index 37
        assert inter36[13][4][0][0] != inter37[13][4][0][0]

    def test_slice_borders(self):
        """Test that the slice returns different results when a read drops off the front, and hops on the back"""
        inter36 = self.cont.get_slice(contig, 36, 36)
        inter37 = self.cont.get_slice(contig, 37, 37)

        ## the first results shouldn't be the same, since we drop the read at index 1
        assert inter36[0][4][0][0] != inter37[0][4][0][0]

        ## the last result shouldn't be the same, since we pick up a new read at index 37
        assert inter36[13][4][0][0] != inter37[13][4][0][0]

    def test_get_reads_edge_case(self):
        """Test that at the edges, that get_reads doesn't return anything weird"""
        get_reads_minus_1 = self.cont.get_reads(contig, -1) ## this should just zero out
        assert len(get_reads_minus_1) == 0

        get_reads0 = self.cont.get_reads(contig, 0) ## nothing at this index, but ok.
        assert len(get_reads0) == 0

        get_reads_last_read_indexed_front = self.cont.get_reads(contig, 13779) ## the last read starts here
        assert len(get_reads_last_read_indexed_front) == 1

        get_reads_last_read_indexed_back = self.cont.get_reads(contig, 13814) ## the last read ends here (nothing)
        print len(get_reads_last_read_indexed_back)
        assert len(get_reads_last_read_indexed_back) == 0

        get_reads_just_past_last_read_indexed = self.cont.get_reads(contig, 13815) ## nothing here
        print len(get_reads_just_past_last_read_indexed)
        assert len(get_reads_just_past_last_read_indexed) == 0

        get_reads_past_the_last_window = self.cont.get_reads(contig, 14815) ## nothing here
        print len(get_reads_past_the_last_window)
        assert len(get_reads_past_the_last_window) == 0

    def test_intersect_edge_case(self):
        """Test that at the edges, that intersect doesn't return anything weird"""
        intersect_minus_1 = self.cont.get_intersect(contig, -1) ## this should just zero out
        assert len(intersect_minus_1) == 0

        intersect0 = self.cont.get_intersect(contig, 0) ## nothing at this index, but ok.
        assert len(intersect0) == 0

        intersect_last_read_indexed_front = self.cont.get_intersect(contig, 13779) ## the last read starts here
        assert len(intersect_last_read_indexed_front) == 18

        intersect_last_read_indexed_back = self.cont.get_intersect(contig, 13814) ## the last read ends here
        print len(intersect_last_read_indexed_back)
        assert len(intersect_last_read_indexed_back) == 1

        intersect_just_past_last_read_indexed = self.cont.get_intersect(contig, 13815) ## nothing here
        print len(intersect_just_past_last_read_indexed)
        assert len(intersect_just_past_last_read_indexed) == 0

        intersect_past_the_last_window = self.cont.get_intersect(contig, 14815) ## nothing here
        print len(intersect_past_the_last_window)
        assert len(intersect_past_the_last_window) == 0

    def test_slice_edge_cases(self):
        """Test that at the edges, that slice doesn't return anything weird"""
        slice_minus_1 = self.cont.get_slice(contig, -1, -1) ## this should just zero out
        assert len(slice_minus_1) == 0

        slice0 = self.cont.get_slice(contig, 0, 0) ## nothing at this index, but ok.
        assert len(slice0) == 0

        slice_last_read_indexed_front = self.cont.get_slice(contig, 13779, 13779) ## the last read starts here
        assert len(slice_last_read_indexed_front) == 18

        slice_last_read_indexed_back = self.cont.get_slice(contig, 13814, 13814) ## the last read ends here
        print len(slice_last_read_indexed_back)
        assert len(slice_last_read_indexed_back) == 1

        slice_just_past_last_read_indexed = self.cont.get_slice(contig, 13815, 13815) ## nothing here
        print len(slice_just_past_last_read_indexed)
        assert len(slice_just_past_last_read_indexed) == 0

        slice_last_read_indexed_back_to_the_next_window = self.cont.get_slice(contig, 13814, 14815) ## nothing here
        print len(slice_last_read_indexed_back_to_the_next_window)
        assert len(slice_last_read_indexed_back_to_the_next_window) == 1

        slice_just_past_last_read_indexed_to_the_next_window = self.cont.get_slice(contig, 13815, 14815) ## nothing here
        print len(slice_just_past_last_read_indexed_to_the_next_window)
        assert len(slice_just_past_last_read_indexed_to_the_next_window) == 0

        slice_start_over_one_window_past_last_read_indexed = self.cont.get_slice(contig, 14815, 14815) ## nothing here
        print len(slice_start_over_one_window_past_last_read_indexed)
        assert len(slice_start_over_one_window_past_last_read_indexed) == 0

    def test_get_reads_retrieve(self):
        """Test retrieval of reads indexed at a particular index"""
        reads = self.cont.get_reads(contig, 13)

        assert len(reads) == 2

        reads_result = list()
        for index, name, sequence, orientation in reads:
           thing = (index, name, sequence, orientation)
           reads_result.append(thing)

        assert reads_result == \
            [(13, 'HWI-EAS_4_PE-FC20GCB:1:212:153:213/1', 'gactgcaacgggcaatatgtctctgtgtggattaaa', 1),
             (13, 'HWI-EAS_4_PE-FC20GCB:1:246:99:950/1', 'gactgcaacgggcaatatgtctctgcgtggattcaa', 1)], reads_result

    def test_slice_retrieve(self):
        """Test retrieval of slices"""
        slice = self.cont.get_slice(contig, 0, 5)

        slice_result = list()
        for ival_start, ival_stop, start, stop, matches in slice:
            for index, name, sequence, orientation in matches:
                thing = (ival_start, ival_stop, start, stop, name, orientation);
                slice_result.append(thing)

        assert len(slice) == 3

        assert slice_result == \
               [(1, 5, 0, 4, 'HWI-EAS_4_PE-FC20GCB:1:146:930:514/1', -1),
                (3, 5, 0, 2, 'HWI-EAS_4_PE-FC20GCB:1:210:846:698/1', 1),
                (4, 5, 0, 1, 'HWI-EAS_4_PE-FC20GCB:1:285:772:762/1', 1)], slice_result
        # check lengths
        for (tstart, tstop, bstart, bstop, _, o) in slice_result:
            assert tstop - tstart == bstop - bstart

    def test_intesect_retrieve(self):
        """Test retrieval of intersections"""
        intersect = self.cont.get_intersect(contig, 5)

        intersect_result = list()
        for ival_start, ival_stop, start, stop, matches in intersect:
            for index, name, sequence, orientation in matches:
                thing = (ival_start, ival_stop, start, stop, name, orientation);
                intersect_result.append(thing)

        assert len(intersect) == 3

        assert intersect_result == \
               [(5, 5, 4, 4, 'HWI-EAS_4_PE-FC20GCB:1:146:930:514/1', -1),
                (5, 5, 2, 2, 'HWI-EAS_4_PE-FC20GCB:1:210:846:698/1', 1),
                (5, 5, 1, 1, 'HWI-EAS_4_PE-FC20GCB:1:285:772:762/1', 1)], intersect_result

        # check lengths
        for (tstart, tstop, bstart, bstop, _, o) in intersect_result:
            assert tstop - tstart == bstop - bstart

    def test_restricted_slice_and_intersect_equivalency(self):
        """Test that the values returned by intersect at a particular index, and slice restricted to that index are the same"""
        slice = self.cont.get_slice(contig, 5, 5)
        slice_result = list()
        for ival_start, ival_stop, start, stop, matches in slice:
            for index, name, sequence, orientation in matches:
                thing = (ival_start, ival_stop, start, stop, name, orientation);
                slice_result.append(thing)

        assert len(slice) == 3

        intersect = self.cont.get_intersect(contig, 5)
        intersect_result = list()
        for ival_start, ival_stop, start, stop, matches in intersect:
            for index, name, sequence, orientation in matches:
                thing = (ival_start, ival_stop, start, stop, name, orientation);
                intersect_result.append(thing)

        assert len(intersect) == 3

        assert slice_result == intersect_result

    def test_empty_slice_retrieve(self):
        """Test that slice returns an emtpy set at the right time"""
        matches = self.cont.get_slice(contig, 14000, 14999)
        assert len(matches) == 0

    def test_slice_was_broken_boundary(self):
        """Test that slice returns the proper set of reads, even if we overrun the edge of the known reads for a given contig"""
        matches = self.cont.get_slice(contig, 0, 20000)

        count = 0;
        for index in matches:
            count = count + len(index[4])

        assert count == 10000

    def test_slice_retrieve_all(self):
        """Test that slice retrieves all the reads in the whole genome"""
        matches = self.cont.get_slice(contig, 0, len(self.seq))

        count = 0;
        for index in matches:
            count = count + len(index[4])

        assert count == 10000

    def test_iterator_edge_cases(self):
        """Test that at the edges, that the iterator doesn't return anything weird"""
        iterator = self.cont.get_iterator(contig, -1)

        get_reads_minus_1 = iterator.get_reads()
        assert len(get_reads_minus_1) == 0
        intersect_minus_1 = iterator.get_intersection()
        assert len(intersect_minus_1) == 0

        iterator.next() ## the next index is the one at 2 (1)

        get_reads_next = iterator.get_reads()
        assert len(get_reads_next) == 1 ## a single read is present
        assert get_reads_next[0][0] == 1 ## and has an index of 1

        intersect_next = iterator.get_intersection()
        assert len(intersect_next) == 1 ## only one index intersects here
        assert len(intersect_next[0][4]) == 1 ## and that one index has one read
        assert intersect_next[0][4][0][0] == 1 ## and the single read's index is 1

        iterator.next() ## the next index is the one at 4 (3)

        get_reads_next = iterator.get_reads()
        assert len(get_reads_next) == 1 ## a single read is present
        assert get_reads_next[0][0] == 3 ## and has an index of 3

        intersect_next = iterator.get_intersection()
        assert len(intersect_next) == 2 ## two indices intersects here
        assert len(intersect_next[0][4]) == 1 ## first index has one read
        assert intersect_next[0][4][0][0] == 1 ## and the single read's index is 1
        assert len(intersect_next[1][4]) == 1 ## second index has one read
        assert intersect_next[1][4][0][0] == 3 ## and the single read's index is 3

        ### last position to get a read in the genome
        iterator = self.cont.get_iterator(contig, 13779)
        get_reads_last_indexed_front = iterator.get_reads()
        assert len(get_reads_last_indexed_front) == 1

        intersect_last_indexed_front = iterator.get_intersection()
        assert len(intersect_last_indexed_front) == 18

        try:
            iterator.next()
            assert 0
        except StopIteration:
            pass

        # @CTB
        #get_reads_past_last_indexed_front = iterator.get_reads()
        #assert len(get_reads_past_last_indexed_front) == 0

        ### last position to get an intersection in the genome
        iterator = self.cont.get_iterator(contig, 13814)

        get_reads_last_indexed_back = iterator.get_reads()
        assert len(get_reads_last_indexed_back) == 0

        intersect_last_indexed_back = iterator.get_intersection()
        assert len(intersect_last_indexed_back) == 1

        try:
            iterator.next()
            assert 0
        except StopIteration:
            pass

        intersect_past_last_indexed_back = iterator.get_reads()
        assert len(intersect_past_last_indexed_back) == 0

        ### past the last window
        iterator = self.cont.get_iterator(contig, 14815)

        get_reads_past_the_last_window = iterator.get_reads()
        assert len(get_reads_past_the_last_window) == 0

        intersect_past_the_last_window = iterator.get_intersection()
        assert len(intersect_past_the_last_window) == 0

        try:
            iterator.next()
            assert 0
        except StopIteration:
            pass

        get_reads_past_the_last_window_next = iterator.get_reads()
        assert len(get_reads_past_the_last_window_next) == 0

        intersect_past_the_last_window_next = iterator.get_intersection()
        assert len(intersect_past_the_last_window_next) == 0

    def test_iterator_get_reads_retrieve(self):
        """Test retrieval of reads indexed at a particular index"""
        iterator = self.cont.get_iterator(contig, 13)
        reads = iterator.get_reads()

        assert len(reads) == 2

        reads_result = list()
        for index, name, sequence, orientation in reads:
           thing = (index, name, sequence, orientation)
           reads_result.append(thing)

        assert reads_result == \
            [(13, 'HWI-EAS_4_PE-FC20GCB:1:212:153:213/1', 'gactgcaacgggcaatatgtctctgtgtggattaaa', 1),
             (13, 'HWI-EAS_4_PE-FC20GCB:1:246:99:950/1', 'gactgcaacgggcaatatgtctctgcgtggattcaa', 1)], reads_result

    def test_iterator_intesect_retrieve(self):
        """Test retrieval of iterated intersections"""
        iterator = self.cont.get_iterator(contig, 5)
        intersect = iterator.get_intersection()

        intersect_result = list()
        for ival_start, ival_stop, start, stop, matches in intersect:
            for index, name, sequence, orientation in matches:
                thing = (ival_start, ival_stop, start, stop, name, orientation);
                intersect_result.append(thing)

        assert len(intersect) == 3

        assert intersect_result == \
               [(5, 5, 4, 4, 'HWI-EAS_4_PE-FC20GCB:1:146:930:514/1', -1),
                (5, 5, 2, 2, 'HWI-EAS_4_PE-FC20GCB:1:210:846:698/1', 1),
                (5, 5, 1, 1, 'HWI-EAS_4_PE-FC20GCB:1:285:772:762/1', 1)], intersect_result

        # check lengths
        for (tstart, tstop, bstart, bstop, _, o) in intersect_result:
            assert tstop - tstart == bstop - bstart

    def test_iterator_intersect_and_engine_intersect_equivalency(self):
        iterator = self.cont.get_iterator(contig, 5)
        it_intersect = iterator.get_intersection()

        it_intersect_result = list()
        for ival_start, ival_stop, start, stop, matches in it_intersect:
            for index, name, sequence, orientation in matches:
                thing = (ival_start, ival_stop, start, stop, name, orientation);
                it_intersect_result.append(thing)
                print thing

        intersect = self.cont.get_intersect(contig, 5)
        intersect_result = list()
        for ival_start, ival_stop, start, stop, matches in intersect:
            for index, name, sequence, orientation in matches:
                thing = (ival_start, ival_stop, start, stop, name, orientation);
                intersect_result.append(thing)
                print thing

        assert len(intersect_result) == len(it_intersect_result) == 3

        assert it_intersect_result == intersect_result

    def test_iterated_slice_retrieve(self):
        """Test retrieval of slices"""
        iterator = self.cont.get_slice_iterator(contig, 0)

        slice_result = list()
        for i in range(4): #(0->1->3->4->)
            reads = iterator.get_reads()
            for ival_start, ival_stop, start, stop, matches in reads:  ## not thrilled that the iterator is returning weird stuff for the ival start/stop
                for index, name, sequence, orientation in matches:
                    thing = (index, name, orientation);
                    slice_result.append(thing)
                    print thing

            iterator.next()

        print len(slice_result)
        assert len(slice_result) == 3

        assert slice_result == \
               [(1, 'HWI-EAS_4_PE-FC20GCB:1:146:930:514/1', -1),
                (3, 'HWI-EAS_4_PE-FC20GCB:1:210:846:698/1', 1),
                (4, 'HWI-EAS_4_PE-FC20GCB:1:285:772:762/1', 1)], slice_result

    def test_iterated_slice_retrieval_and_engine_slice_retrieval_equivalency(self): ## minus the ival start and stop, since there's really no way to make that work
        """Test that an engine slice retrieval and an iterated slice retrieval are equivalent"""
        iterator = self.cont.get_slice_iterator(contig, 0)

        iterated_slice_result = list()
        for i in range(4): #(0->1->3->4->)
            reads = iterator.get_reads()
            for ival_start, ival_stop, start, stop, matches in reads:  ## not thrilled that the iterator is returning weird stuff for the ival start/stop
                for index, name, sequence, orientation in matches:
                    thing = (index, name, orientation);
                    iterated_slice_result.append(thing)
                    print thing

            iterator.next()

        print len(iterated_slice_result)
        assert len(iterated_slice_result) == 3

        engine_slice = self.cont.get_slice(contig, 0, 5)

        engine_slice_result = list()
        for ival_start, ival_stop, start, stop, matches in engine_slice:
            for index, name, sequence, orientation in matches:
                thing = (index, name, orientation);
                print thing
                engine_slice_result.append(thing)

        assert len(engine_slice_result) == 3

        assert iterated_slice_result == engine_slice_result == \
               [(1, 'HWI-EAS_4_PE-FC20GCB:1:146:930:514/1', -1),
                (3, 'HWI-EAS_4_PE-FC20GCB:1:210:846:698/1', 1),
                (4, 'HWI-EAS_4_PE-FC20GCB:1:285:772:762/1', 1)], iterated_slice_result

    def test_iterated_simulated_slice_retrieval_and_slice_iterator_equivalency(self):
        """Test retrieval of slices"""
        iterator = self.cont.get_slice_iterator(contig, 0)

        slice_result = list()
        for i in range(4): #(0->1->3->4->)
            reads = iterator.get_reads()
            for ival_start, ival_stop, start, stop, matches in reads:  ## not thrilled that the iterator is returning weird stuff for the ival start/stop
                for index, name, sequence, orientation in matches:
                    thing = (index, name, orientation);
                    slice_result.append(thing)
                    print thing

            iterator.next()

        print len(slice_result)
        assert len(slice_result) == 3

        assert slice_result == \
               [(1, 'HWI-EAS_4_PE-FC20GCB:1:146:930:514/1', -1),
                (3, 'HWI-EAS_4_PE-FC20GCB:1:210:846:698/1', 1),
                (4, 'HWI-EAS_4_PE-FC20GCB:1:285:772:762/1', 1)], slice_result

        iterator = self.cont.get_iterator(contig, 0)

        sim_slice_result = list()
        for i in range(4): #(0->1->3->4->)
            if ( i == 0 ):
                reads = iterator.get_intersection()
                for ival_start, ival_stop, start, stop, matches in reads:  ## not thrilled that the iterator is returning weird stuff for the ival start/stop
                    for index, name, sequence, orientation in matches:
                        thing = (index, name, orientation);
                        sim_slice_result.append(thing)
                        print thing
            else:
                reads = iterator.get_reads()
                for index, name, sequence, orientation in reads:
                    thing = (index, name, orientation);
                    sim_slice_result.append(thing)
                    print thing

            iterator.next()

        print len(sim_slice_result)
        assert len(sim_slice_result) == 3

        assert sim_slice_result == slice_result

    def test_iterated_restricted_slice_and_iterated_intersect_equivalency(self):
        """Test that the values returned by intersect at a particular index, and slice restricted to that index are the same"""
        slice_iterator = self.cont.get_slice_iterator(contig, 5)
        slice = slice_iterator.get_reads()
        slice_result = list()
        for ival_start, ival_stop, start, stop, matches in slice: ## not thrilled that the iterator is returning weird stuff for the ival start/stop
            for index, name, sequence, orientation in matches:
                thing = (ival_start, ival_stop, start, stop, name, orientation);
                slice_result.append(thing)

        assert len(slice) == 3

        iterator = self.cont.get_iterator(contig, 5)
        intersect = iterator.get_intersection()
        intersect_result = list()
        for ival_start, ival_stop, start, stop, matches in intersect:
            for index, name, sequence, orientation in matches:
                thing = (ival_start, ival_stop, start, stop, name, orientation);
                intersect_result.append(thing)

        assert len(intersect) == len(slice) == 3

        assert slice_result == intersect_result

class PygrBridge_Test(object):
    def setup(self):
        try:
            screed
        except NameError:
            raise SkipTest
        
        self.cont = caper.mapping_container(map_bundle)
        self.db = seqdb.SequenceFileDB(sequence_path) #ScreedSequenceDB('data/REL606.gmc.fa')
        self.db = ScreedSequenceDB(sequence_path)
        self.seq = self.db[contig]
        self.reads_db = ScreedSequenceDB(sequence_fastq)

        self.al = CaperBridge(self.cont, self.db, self.reads_db)

    def test_basic_slice(self):
        ival = self.seq[0:50]
        slice = self.al[ival]
        assert len(slice) == 22

    def test_bridge_nlmsa_equiv(self):
        nlmsa = cnestedlist.NLMSA('foo', 'memory', pairwiseMode=True)
        nlmsa += self.seq

        # select across a wider window then we'll actually slice, to get
        # an inclusive set of reads
        m = self.cont.get_slice('rel606', 0, 200)

        # construct nlmsa
        N=0
        for ival_start, ival_stop, read_start, read_stop, match in m:
            for index, read_name, sequence, orientation in match:
                N += 1
                if ival_start == ival_stop:
                    continue

                ival = self.seq[ival_start:ival_stop]
                if orientation == -1:
                    match_seq = -self.reads_db[read_name]
                else:
                    match_seq = self.reads_db[read_name]

                match_ival = match_seq[read_start:read_stop]

                nlmsa[ival] += match_ival

        nlmsa.build()

        ### now, slice!
        ival = self.seq[75:80]
        nlmsa_slice = nlmsa[ival]
        caper_slice = self.al[ival]

        s = set(nlmsa_slice.keys())
        t = set(caper_slice.keys())

        assert s == t

        for k, v, e in nlmsa[ival].edges():
            print repr(k), repr(v), e.pIdentity(), v.orientation
            assert e.pIdentity() >= 0.8, (repr(k), repr(v), e.pIdentity())
