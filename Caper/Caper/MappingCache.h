#pragma once

#include "stdafx.h"
#include "Mapping.h"
#include "Typedefs.h"

class MappingCache
{
private:
	static const char Tab = '\t';
  

public:
  string ContigIdent;
  long long LeftIndex;
  long long RightIndex;
  IndexedMappings * IndexedReads;

public:
	MappingCache( 
    IndexedMappings * lMappings, 
    string & aContigIdent, 
    long long & aLeftIndex, 
    long long & aRightIndex );

  Mappings * GetReads( long long aLeft, long long aRight );

  ~MappingCache(void);
 
private:
  int PrivateIndex( long long aPublicIndex );
  void DestroyMappings();
};
