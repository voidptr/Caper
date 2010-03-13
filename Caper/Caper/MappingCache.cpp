#include "MappingCache.h"

MappingCache::MappingCache( IndexedMappings * aIndexedReads, string & aContigIdent, long long & aLeftIndex, long long & aRightIndex )
{
  IndexedReads = aIndexedReads;

  ContigIdent = aContigIdent;
  LeftIndex = aLeftIndex;
  RightIndex = aRightIndex;
}

Mappings * MappingCache::GetReads( long long aLeft, long long aRight )
{
  Mappings * lResult = new Mappings();

  for ( long long i = PrivateIndex(aLeft); i <= PrivateIndex(aRight); ++i )
  {
    if ( !IndexedReads->at(i).empty() )
    {
      for ( long long j = 0; j < IndexedReads->at(i).size() ; ++j )
      {        
        lResult->push_back( IndexedReads->at(i)[j] );
      }
    }
  }

  return lResult;
}

int MappingCache::PrivateIndex( long long aPublicIndex )
{
  return (int) aPublicIndex - LeftIndex; // this is ok, since it'll always be < 1000. I could even do this with a short.
}

void MappingCache::DestroyMappings()
{
  for (long long i = 0; i < IndexedReads->size(); ++i )
  {
    for (long long j = 0; j < IndexedReads->at(i).size(); ++j )
    {
      delete IndexedReads->at(i)[j];
    }    
  }
}

MappingCache::~MappingCache()
{
  DestroyMappings();
  delete IndexedReads; // there's a memory leak here, because the pointers are getting deallocated...?
}


