#! /bin/bash
export PYTHONPATH=/u/t/dev/screed

CAPER=../CaperGCC/Caper
CACHE=data/cache

rm -fr ${CACHE}
mkdir ${CACHE}

${CAPER} indexgenome -g data/REL606.gmc.fa -o ${CACHE}

${CAPER} indexmappings -m data/REL606-maq-map.txt -t mapview -o ${CACHE}

# test!
echo rel606:0:20:p | \
${CAPER} interactive -f ${CACHE}/REL606.gmc.fa.genomeindex -g ${CACHE}/REL606.gmc.fa.indexed -m data/REL606-maq-map.txt  -i ${CACHE}/REL606-maq-map.txt.mappingindex

###

# also build the screed indexes
type python
python -m screed.fqdbm data/REL606-seqs.fastq
