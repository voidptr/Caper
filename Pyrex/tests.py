# @CTB to test: do our own mapping parsing & verify against caper, too.
# @CTB refactor test_bridge_nlmsa_equiv to use the new pygr ival construction
# lib code.
import os.path

#try:
#    import screed
#except ImportError:
#    raise Exception, "you need to install screed!"

import caper
#from caper_pygr_bridge import CaperBridge
#from screed.pygr_api import ScreedSequenceDB
from pygr import cnestedlist, seqdb

#sequence_path = 'data/REL606.gmc.fa'

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

class MappingContainer_Test(object):
    def setup(self):
        self.cont = caper.mapping_container(map_bundle)
        self.db = seqdb.SequenceFileDB('data/REL606.gmc.fa')
        self.seq = self.db['rel606']

    def test_retrieve(self):
        matches = self.cont.get_slice('rel606', 0, 5)

        x = list()
        for item in matches:
            for read in item[4]:
                thing = (item[0], item[1], item[2], item[3], read[1], read[3]);
                x.append(thing)

        assert len(matches) == 3

        assert x == \
               [(1, 5, 0, 4, 'HWI-EAS_4_PE-FC20GCB:1:146:930:514/1', -1),
                (3, 5, 0, 2, 'HWI-EAS_4_PE-FC20GCB:1:210:846:698/1', 1),
                (4, 5, 0, 1, 'HWI-EAS_4_PE-FC20GCB:1:285:772:762/1', 1)], x
        # check lengths
        for (tstart, tstop, bstart, bstop, _, o) in x:
            assert tstop - tstart == bstop - bstart

    def test_empty_retrieve(self):
        matches = self.cont.get_slice('rel606', 14000, 14999)
        assert len(matches) == 0

    def test_was_broken_boundary(self):
        matches = self.cont.get_slice('rel606', 0, 20000)

        count = 0;
        for index in matches:
            count = count + len(index[4])

        assert count == 10000

    def test_retrieve_all(self):
        matches = self.cont.get_slice('rel606', 0, len(self.seq))

        count = 0;
        for index in matches:
            count = count + len(index[4])

        assert count == 10000


# class PygrBridge_Test(object):
#     def setup(self):
#         self.cont = caper.mapping_container(map_bundle)
#         self.db = seqdb.SequenceFileDB('data/REL606.gmc.fa') #ScreedSequenceDB('data/REL606.gmc.fa')
#         #self.db = ScreedSequenceDB('data/REL606.gmc.fa')
#         self.seq = self.db['rel606']
#         self.reads_db = ScreedSequenceDB('data/REL606-seqs.fastq')
#
#         self.al = CaperBridge(self.cont, self.db, self.reads_db)
#
#     def test_basic_slice(self):
#         ival = self.seq[0:50]
#         slice = self.al[ival]
#         assert len(slice) == 22
#
#         for src, dest, _ in slice.edges():
#             print src
#             print dest
#             print ''
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
#         for ival_start, ival_stop, read_name, read_start, read_stop, o in m:
#             N += 1
#             if ival_start == ival_stop:
#                 continue
#
#             ival = self.seq[ival_start:ival_stop]
#             if o == -1:
#                 match_seq = -self.reads_db[read_name]
#             else:
#                 match_seq = self.reads_db[read_name]
#
#             match_ival = match_seq[read_start:read_stop]
#             nlmsa[ival] += match_ival
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
