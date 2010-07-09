# @CTB to test: do our own mapping parsing & verify against caper, too.
# @CTB refactor test_bridge_nlmsa_equiv to use the new pygr ival construction
# lib code.
import os.path

try:
    import screed
except ImportError:
    raise Exception, "you need to install screed!"

import caper
from caper_pygr_bridge import CaperBridge
from screed.pygr_api import ScreedSequenceDB
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

    def test_intersect_edge_case(self):
        """Test that at the 0th interval, that intersect doesn't return anything weird"""
        inter0 = self.cont.get_intersect(contig, 0)
        assert len(inter0) == 0

    def test_slice_edge_cases(self):
        """Test that at the edges, that intersect doesn't return anything weird"""
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

        slice_just_past_last_read_indexed_to_the_next_window = self.cont.get_slice(contig, 13815, 14815) ## nothing here
        print len(slice_just_past_last_read_indexed_to_the_next_window)
        assert len(slice_just_past_last_read_indexed_to_the_next_window) == 0

        ## this causes a segfault. Fix!
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

    def test_retrieve_all(self):
        """Test that slice retrieves all the reads in the whole genome"""
        matches = self.cont.get_slice(contig, 0, len(self.seq))

        count = 0;
        for index in matches:
            count = count + len(index[4])

        assert count == 10000

class PygrBridge_Test(object):
    def setup(self):
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
