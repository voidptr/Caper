#pragma once

#include "stdafx.h"
#include "MappingCache.h"
#include "MappingFileEngine.h"

class MappingCacheFactory
{
private:
  static const int IndexIncrement = 1000;

  MappingFileEngine * mFileEngine;

public:

  MappingCacheFactory( MappingFileEngine * aFileEngine )
  {
	  mFileEngine = aFileEngine;
  }

  MappingCache * BuildMappingCache( string & aContig, long long aWindowNumber )
  {
    cout << "~~MappingCacheFactory - BuildMappingCache " << aContig << " window " << aWindowNumber << endl;
	vector<Mapping*> * lMappings = mFileEngine->FetchMappings( aContig, aWindowNumber );

    if ( lMappings == NULL ) // no window!
      return NULL;

    long long aLeft = aWindowNumber * IndexIncrement;
    long long aRight = ( aWindowNumber * IndexIncrement ) + IndexIncrement - 1;
    MappingCache * lCache = new MappingCache( aContig, aLeft, aRight );
	  lCache->Populate( lMappings );

    delete lMappings;

    return lCache;
  }
};
