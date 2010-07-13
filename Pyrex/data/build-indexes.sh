#! /bin/bash
export PYTHONPATH=/u/t/dev/screed

CAPER=../CaperGCC/caper
CACHE=data/cache

rm -fr ${CACHE}
mkdir ${CACHE}

##${CAPER} indexgenome -g data/REL606.gmc.fa -o ${CACHE}

## converted to bundle (for the pyrex wrapper)
${CAPER} indexmappings -b -m data/REL606-maq-map.txt -t mapview -o ${CACHE}

# test!
echo interval rel606 0 20 | \
${CAPER} interactive -b ${CACHE}/REL606-maq-map.txt.bundle
#-m data/REL606-maq-map.txt  -i ${CACHE}/REL606-maq-map.txt.mappingindex

###

# also build the screed indexes
python -m screed.fadbm data/REL606.gmc.fa
python -m screed.fqdbm data/REL606-seqs.fastq

# one more conversion
${CAPER} indexmappings -b -m data/REL606-maq-map_7entries.txt -t mapview -o ${CACHE}
