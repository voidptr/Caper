#pragma once

#include "stdafx.h"
#include "Sequence.h"
#include "Mapping.h"

typedef map<string,Sequence*> Sequences;
typedef pair<string, Sequence*> SequencePair;


typedef vector<Mapping*> Mappings;
typedef vector<Mappings> IndexedMappings;

// for sorting
typedef pair<string, int> MappingKey;
typedef pair<MappingKey, long> MappingIndex;




