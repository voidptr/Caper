import caper
x = caper.mapping_container('/scratch2/rose/s_1_sequence.map.trim3-20.sam.sorted', '/scratch2/rose/s_1_sequence.map.trim3-20.sam.mappingindex', 35)

m = x.get_reads_at("Contig0", 0, 1050)
print m.seqname, m.start, m.stop
print 'length:', len(m)

for i in m:
    print i

###

#if 0:
#    import time
#    for i in range(100):
#        print i
#        for j in range(10000):
#            x.get_reads_at("rel606", 0, 50)
