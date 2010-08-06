# @CTB why is it called 7entries when there's only 6? ;)

import os

import caper

bundle_path = 'data/cache/REL606-maq-map_7entries.txt.bundle'
contig = 'rel606'

###

thisdir = os.path.dirname(__file__)
thisdir = os.path.realpath(thisdir)
bundle_path = os.path.join(thisdir, bundle_path)

class Test_Basic(object):
    def setup(self):
        self.engine = caper.mapping_container(bundle_path)

    def test_length(self):
        length = self.engine.get_read_length()
        assert length == 36, length

    def test_get_reads_overlap_4(self):
        reads = self.engine.get_reads(contig, 4, 4)

        bleh = 0
        count = 0
        for read in reads:
            bleh = read
            count += 1

        assert count == 3
        (contig_pos, read_name, read_seq, o) = bleh ## the last read

        assert contig_pos == 4
        assert read_name == 'HWI-EAS_4_PE-FC20GCB:1:285:772:762/1'
        assert read_seq == 'tttcattctgactgcaacgggcaatatgtctctgtg'
        assert o == 1

    def test_get_reads_overlap_0(self):
        reads = self.engine.get_reads(contig, 0, 0)

        count = 0
        for read in reads:
            count += 1

        assert count == 0

#     def test_get_reads_int_2(self):
#         reads = self.engine.get_intersect(contig, 2)
#         assert len(reads) == 1, list(reads)
# #        assert 0, list(reads)
# #    test_get_reads_int_2.failing = True
#
#     def test_get_reads_slice(self):
#         reads = self.engine.get_slice(contig, 0, 4)
#         assert len(reads) == 3, list(reads)
#         #assert 0, list(reads)
# #    test_get_reads_slice.failing = True
#
#     def test_get_reads_slice_iterator(self):
#         iterator = self.engine.get_slice_iterator(contig, 0)
#         reads = iterator.get_reads()
#         assert len(reads) == 0
# #        assert 0, list(reads)
# #    test_get_reads_slice_iterator.failing = True
#
#     def test_get_iterator(self):
#         x = [ index for (index, readname, readseq, o) in self.engine.get_reads(contig, 0) ]
#         assert x == [ 1, 3, 4, 1000, 1002, 1003 ]

if __name__ == '__main__':
    import nose
    nose.main()

