#pragma once

#include "stdafx.h"
#include "Sequence.h"
#include "Mapping.h"

#include "StoredMappingIntervalBlock.h"

typedef map<string,Sequence*> Sequences;
typedef pair<string, Sequence*> SequencePair;


typedef vector<Mapping*> Mappings;
typedef vector<Mappings> IndexedMappings;

// for sorting
//typedef pair<string, long long> MappingKey; // contig, index
//typedef pair<MappingKey, long long> MappingIndex; // contig/index, file offset of line

// set of contigs with the vector of the blocks for each contig.
typedef map<string, vector<StoredMappingIntervalBlock> > MappingIndexes;

//typedef map<string, vector<LineIndex> > MappingsMap;



//// for storing the mapping indexes
//typedef triad<long long, long long, long long> StoredMappingIntervalBlock; // offset, block size, stored size

//typedef pair<long long, long long> ContigBorders; // start, finish.

enum MappingFileFormat
{
  DEFAULT,
  MAPVIEW,
  BOWTIE,
  SAM
}; // Mapview Mappings or BowtieMappings




