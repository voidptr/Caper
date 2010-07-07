#pragma once

#include "stdafx.h"
#include "Sequence.h"
#include "Mapping.h"
#include "IndexedMappings.h"

typedef map<string,Sequence*> Sequences;
typedef pair<string, Sequence*> SequencePair;

//typedef map<long long, ReadsAtIndex *> IndexedMappings;

// for storing the mapping indexes
typedef triad<long long, long long, long long> StoredMappingBlock; // offset, block size, stored size

typedef pair<long long, long long> ContigBorders; // start, finish.

enum MappingFileFormat
{
  DEFAULT,
  MAPVIEW,
  BOWTIE,
  SAM
}; // Mapview Mappings or BowtieMappings




