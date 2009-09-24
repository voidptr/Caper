#! /bin/bash
rm -fr data/cache/
mkdir data/cache
./Caper -s data/cache/ -g data/REL606.gmc.fa -m data/REL606-maq-map.txt < /dev/null

# test!
echo rel606:0:20:p |
./Caper -i data/cache/saved.index -f data/cache/saved.refgenomeindex -M data/REL606-maq-map.txt -G data/REL606.gmc.fa

###

# also build the screed indexes
~t/dev/screed/bin/fqdbm data/REL606-seqs.fastq
