Caper v0.3.1
Usage: caper [-s SaveIndexesToPath] [-i SavedMappingIndexFile] [-f SavedReferenceGenomeIndexFile] <-g|-G> <referencegenome.fa> <-m|-M|-b|-B> <mappingsfile>

-s : Save your indexes (both reference genome and mapping indexes) out to this directory

-i : load this mapping file index. 

-f : load this reference genome index. If you use this option, it assumes your reference genome file has already been massaged by Caper in past.
	 
-g : load this reference genome file. 

-G : load this pre-massaged reference genome file.

-m : load this mapview mapping file.

-M : load this pre-sorted mapview mapping file.

-b : load this bowtie mapping file. Don't use this at the same time as -m. Who knows what'll happen.

-B : load this pre-sorted bowtie mapping file. Don't use this at the same time as -m. Who knows what'll happen.



foo
