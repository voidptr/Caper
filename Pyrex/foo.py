import caper
#bundle_path = '/scratch2/rose/capertestdata/out/s_1_sequence.map.trim3-20.bundle'
#contig = 'Contig0'

bundle_path = 'data/cache/REL606-maq-map.txt.bundle'
contig = 'rel606'
engine = caper.mapping_container(bundle_path)


print "ENGINE"
print "------"

print "GET READ LENGTH"

length = engine.get_read_length()
print length

print
print "GET READS"

reads = engine.get_reads(contig,13)
for item in reads:
    print item

print
print "GET INTERSECT"

inte = engine.get_intersect(contig,4)
for item in inte:
    for read in item[4]:
        print read

print
print "GET SLICE TEST"
slice = engine.get_slice(contig,0, 5)
for ival_start, ival_stop, start, stop, matches in slice:
    for index, name, sequence, orientation in matches:
        print index, name, sequence, orientation

print
print "ITERATOR"
print "--------"

print
print "ITERATOR: GET READS"
iter = engine.get_iterator(contig,0)
for i in range(10):
    x = iter.get_reads()
    print "Iterator Index: ", iter.current
    for index, name, sequence, orientation in x:
        print index, name, sequence, orientation
    iter.next()

print
print "ITERATOR: GET INTERSECTION"
iter = engine.get_iterator(contig,0)
for i in range(10):
    x = iter.get_intersection()
    print
    print "Iterator Index: ", iter.current
    for ival_start, ival_stop, slice_start, slice_stop, reads in x:
        for index, name, sequence, orientation in reads:
            print index, name, sequence, orientation

    iter.next()

print
print "ITERATOR: SET, GET INTERSECTION"
iter = engine.get_iterator(contig,5)
x = iter.get_intersection()
print
print "Iterator Index: ", iter.current
for ival_start, ival_stop, slice_start, slice_stop, reads in x:
    for index, name, sequence, orientation in reads:
        print index, name, sequence, orientation

print
print "SLICE ITERATOR"
print "--------------"

print
print "SLICE ITERATOR: GET READS"
slice_iter = engine.get_slice_iterator(contig,5)
for i in range(10):
    x = slice_iter.get_reads()
    print "Slice Iterator Index: ", slice_iter.current
    for ival_start, ival_stop, slice_start, slice_stop, reads in x:
        for index, name, sequence, orientation in reads:
            print index, name, sequence, orientation
    slice_iter.next()

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
