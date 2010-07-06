import caper
engine = caper.mapping_container('/scratch2/rose/capertestdata/out/s_1_sequence.map.trim3-20.bundle')

print "GET READ LENGTH"
length = engine.get_read_length()
print length

print "GET READS"

reads = engine.get_reads("Contig0",373)
for item in reads:
    print item

print
print "GET INTERSECT"

inte = engine.get_intersect("Contig0",385)
for item in inte:
    for read in item:
        print read

print
print "GET SLICE"

slice = engine.get_slice("Contig0",0, 1000)
for item in slice:
    for read in item:
        print read

#print
#print "GET ITERATOR"
#iter = engine.get_iterator("Contig0",0)

#for i in range(10):
#    reads = iter.get_reads()
#    for item in reads:
#        print item.NameP()

#m = x.get_reads_at("Contig0", 0, 1050)
#print m.seqname, m.start, m.stop
#print 'length:', len(m)

#for i in m:
#    print i

###

#if 0:
#    import time
#    for i in range(100):
#        print i
#        for j in range(10000):
#            x.get_reads_at("rel606", 0, 50)
