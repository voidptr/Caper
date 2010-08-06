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
print "ITERATE - GET SLICE - 47"

reads = engine.get_reads(contig,47,47)
count = 0
for item in reads:
    print item
    count += 1

print count

print
print "ITERATE - GET SLICE - 48"

reads = engine.get_reads(contig,48,48)
count = 0
for item in reads:
    print item
    count += 1

print count

print
print "ITERATE - GET SLICE - 0 to the end"

reads = engine.get_reads(contig,0)
count = 0
for item in reads:
    print item
    count += 1

print count
