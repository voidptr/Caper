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

    def test_get_reads_edge_case(self):
        """Test that at the edges, that get_reads doesn't return anything weird"""
        get_reads_overlap_0 = self.cont.get_reads(contig, 0, 0) ## this should just zero out
        count = 0
        for read in get_reads_overlap_0:
            count += 1
        print "get_reads_overlap_0 Expect: 0, Actual: ", count
        assert count == 0

        get_reads_last_read_indexed_front = self.cont.get_reads(contig, 13779, 13779) ## the last read starts here
        count = 0
        for read in get_reads_last_read_indexed_front:
            count += 1
        print "get_reads_last_read_indexed_front Expect: 26, Actual: ", count
        assert count == 26

        get_reads_last_read_indexed_back = self.cont.get_reads(contig, 13814, 13814) ## the last read last nt
        count = 0
        for read in get_reads_last_read_indexed_back:
            count += 1
        print "get_reads_last_read_indexed_back Expect: 0, Actual: ", count
        assert count == 1

        get_reads_just_past_last_read_indexed = self.cont.get_reads(contig, 13815, 13815) ## nothing here
        count = 0
        for read in get_reads_just_past_last_read_indexed:
            count += 1
        print "get_reads_just_past_last_read_indexed Expect: 0, Actual: ", count
        assert count == 0

        get_reads_past_the_last_window = self.cont.get_reads(contig, 14815, 14816) ## nothing here
        count = 0
        for read in get_reads_past_the_last_window:
            count += 1
        print "get_reads_just_past_last_read_indexed Expect: 0, Actual: ", count
        assert count == 0

    def test_get_reads_retrieve(self):
        """Test retrieval of reads indexed at a particular index"""
        reads = self.cont.get_reads(contig, 13, 13)

        reads_result = list()
        count = 0
        for index, name, sequence, orientation in reads:
           thing = (index, name, sequence, orientation)
           reads_result.append(thing)
           count += 1

        print "test_get_reads_retrieve Expect: 8, Actual: ", count
        assert count == 8

        assert reads_result == \
            [(1, 'HWI-EAS_4_PE-FC20GCB:1:146:930:514/1', 'gcttttcattctgactgcaacgggcaatatgtctct', -1),
             (3, 'HWI-EAS_4_PE-FC20GCB:1:210:846:698/1', 'ttttcattctgactgcaacgggcaatatgtctctgt', 1),
             (4, 'HWI-EAS_4_PE-FC20GCB:1:285:772:762/1', 'tttcattctgactgcaacgggcaatatgtctctgtg', 1),
             (10, 'HWI-EAS_4_PE-FC20GCB:1:172:430:569/1', 'tctgactgcaacgggcaatatgtctctgtgtggatt', -1),
             (11, 'HWI-EAS_4_PE-FC20GCB:1:1:563:116/1', 'ctgactgcaacgggcaatatgtctctgtgcagaata', 1),
             (12, 'HWI-EAS_4_PE-FC20GCB:1:113:162:700/1', 'tgactgcaacgggcaatatgtctctgtgtggattaa', -1),
             (13, 'HWI-EAS_4_PE-FC20GCB:1:212:153:213/1', 'gactgcaacgggcaatatgtctctgtgtggattaaa', 1),
             (13, 'HWI-EAS_4_PE-FC20GCB:1:246:99:950/1', 'gactgcaacgggcaatatgtctctgcgtggattcaa', 1)], reads_result

    def test_slice_was_broken_boundary(self):
        """Test that slice returns the proper set of reads, even if we overrun the edge of the known reads for a given contig"""
        matches = self.cont.get_reads(contig, 0, 20000)

        count = 0
        for read in matches:
            count += 1

        assert count == 10000

    def test_slice_retrieve_all(self):
        """Test that slice retrieves all the reads in the whole genome"""
        matches = self.cont.get_reads(contig, 0, len(self.seq))

        count = 0;
        for index in matches:
            count += 1

        assert count == 10000


# class PygrBridge_Test(object):
#     def setup(self):
#         try:
#             screed
#         except NameError:
#             raise SkipTest
#
#         self.cont = caper.mapping_container(map_bundle)
#         self.db = seqdb.SequenceFileDB(sequence_path) #ScreedSequenceDB('data/REL606.gmc.fa')
#         self.db = ScreedSequenceDB(sequence_path)
#         self.seq = self.db[contig]
#         self.reads_db = ScreedSequenceDB(sequence_fastq)
#
#         self.al = CaperBridge(self.cont, self.db, self.reads_db)
#
#     def test_basic_slice(self):
#         ival = self.seq[0:50]
#         slice = self.al[ival]
#         assert len(slice) == 22
#
#     def test_bridge_nlmsa_equiv(self):
#         nlmsa = cnestedlist.NLMSA('foo', 'memory', pairwiseMode=True)
#         nlmsa += self.seq
#
#         # select across a wider window then we'll actually slice, to get
#         # an inclusive set of reads
#         m = self.cont.get_slice('rel606', 0, 200)
#
#         # construct nlmsa
#         N=0
#         for ival_start, ival_stop, read_start, read_stop, match in m:
#             for index, read_name, sequence, orientation in match:
#                 N += 1
#                 if ival_start == ival_stop:
#                     continue
#
#                 ival = self.seq[ival_start:ival_stop]
#                 if orientation == -1:
#                     match_seq = -self.reads_db[read_name]
#                 else:
#                     match_seq = self.reads_db[read_name]
#
#                 match_ival = match_seq[read_start:read_stop]
#
#                 nlmsa[ival] += match_ival
#
#         nlmsa.build()
#
#         ### now, slice!
#         ival = self.seq[75:80]
#         nlmsa_slice = nlmsa[ival]
#         caper_slice = self.al[ival]
#
#         s = set(nlmsa_slice.keys())
#         t = set(caper_slice.keys())
#
#         assert s == t
#
#         for k, v, e in nlmsa[ival].edges():
#             print repr(k), repr(v), e.pIdentity(), v.orientation
#             assert e.pIdentity() >= 0.8, (repr(k), repr(v), e.pIdentity())
