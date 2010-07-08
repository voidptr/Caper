import caper
#bundle_path = '/scratch2/rose/capertestdata/out/s_1_sequence.map.trim3-20.bundle'
#contig = 'Contig0'

bundle_path = 'data/cache/REL606-maq-map.txt.bundle'
contig = 'rel606'
engine = caper.mapping_container(bundle_path)


# print "GET READ LENGTH"
# length = engine.get_read_length()
# print length
#
# print "GET READS"
#
# reads = engine.get_reads(contig,373)
# for item in reads:
#     print item
#
# print
# print "GET INTERSECT"
#
# inte = engine.get_intersect(contig,385)
# for item in inte:
#     print "array", item
#     for read in item[4]:
#         print read

print
print "GET SLICE TEST"

print "0-5"
slice = engine.get_slice(contig,0, 5)
# print "1001-1005"
# slice3 = engine.get_slice(contig, 1001, 1005)
# print "0-5 again"
# slice2 = engine.get_slice(contig,0, 5)
#
# print "FETCHING FROM the original 0-5"
# print
# print "## ROOT"
# print "======================================================"
# val = slice[0]
# print ("VAL: ", val)
# print "------------------------------------------------------"
# print ("SLICE[0]: ", slice[0])
# print "------------------------------------------------------"
#
# print
# print
# print "##0th"
# print "======================================================"
#
# print ("VAL[0]: ",val[0])
# print "------------------------------------------------------"
# print ("SLICE[0][0]: ",slice[0][0])
# print "------------------------------------------------------"
#
# print
# print
# print "##1st"
# print "======================================================"
#
# print ("VAL[1]: ",val[1])
# print "------------------------------------------------------"
# print ("SLICE[0][1]: ",slice[0][1])
# print "------------------------------------------------------"
#
# print
# print
# print "##2nd"
# print "======================================================"
#
# print ("VAL[2]: ",val[2])
# print "------------------------------------------------------"
# print ("SLICE[0][2]: ",slice[0][2])
# print "------------------------------------------------------"


for ival_start, ival_stop, start, stop, matches in slice:
    for index, name, sequence, orientation in matches:
        print index, name, sequence, orientation



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
