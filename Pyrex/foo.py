import caper
x = caper.mapping_container('s_1_1.mapview', 'foosaved.index',
                            'REL606.gmc.fa', 'foosaved.refgenomeindex')

m = x.get_reads_at("rel606", 0, 50)
print m.seqname, m.start, m.stop
print 'length:', len(m)

for i in m:
    print i

###

if 0:
    import time
    for i in range(100):
        print i
        for j in range(10000):
            x.get_reads_at("rel606", 0, 50)
