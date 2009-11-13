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
  int LeftIndex;
  int RightIndex;
  IndexedMappings * IndexedReads;

public:
	MappingCache( 
    IndexedMappings * lMappings, 
    string & aContigIdent, 
    int & aLeftIndex, 
    int & aRightIndex );

  Mappings * GetReads( int aLeft, int aRight );

  ~MappingCache(void);
 
private:
  int PrivateIndex( int aPublicIndex );
  void DestroyMappings();
};
