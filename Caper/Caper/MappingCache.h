#pragma once

#include "stdafx.h"
#include "Mapping.h"

class MappingCache
{
private:
	static const char Tab = '\t';

public:
  string ContigIdent;
  int LeftIndex;
  int RightIndex;
  vector<vector<Mapping>> * Sequences;

public:
	MappingCache( 
    vector<vector<Mapping>> * lMappings, 
    string & aContigIdent, 
    int & aLeftIndex, 
    int & aRightIndex );

  vector<Mapping>* GetReads( int aLeft, int aRight );
 
private:
  int PrivateIndex( int aPublicIndex );
};
