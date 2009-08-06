Caper v0.3
Usage: caper -r [-s savepath] [-i savedindexfile] [-f savedreferencegenomeindex] -g <referencegenome.fa> <-m|-b> <mappingsfile>

-r : The mappings file is already sorted, so don't bother.

-s : Save your indexes (both reference genome and mapping indexes) out to this directory

-i : load this mapping file index. 

-f : load this reference genome index. If you use this option, it assumes your reference genome file has already been massaged by Caper in past.
	 
-g : load this reference genome file. (does double duty in concert with -f to load a massaged .fa file)

-m : load this mapview mapping file.

-b : load this bowtie mapping file. Don't use this at the same time as -m. Who knows what'll happen.




