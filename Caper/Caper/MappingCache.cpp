#include "MappingCache.h"

MappingCache::MappingCache( IndexedMappings * aIndexedReads, string & aContigIdent, int & aLeftIndex, int & aRightIndex )
{
  IndexedReads = aIndexedReads;

  ContigIdent = aContigIdent;
  LeftIndex = aLeftIndex;
  RightIndex = aRightIndex;
}

Mappings * MappingCache::GetReads( int aLeft, int aRight )
{
  Mappings * lResult = new Mappings();

  for ( int i = PrivateIndex(aLeft); i <= PrivateIndex(aRight); i++ )
  {
    if ( !IndexedReads->at(i).empty() )
    {
      for ( int j = 0; j < IndexedReads->at(i).size() ; j++ )
      {        
        lResult->push_back( IndexedReads->at(i)[j] );
      }
    }
  }

  return lResult;
}

int MappingCache::PrivateIndex( int aPublicIndex )
{
  return aPublicIndex - LeftIndex;
}

void MappingCache::DestroyMappings()
{
  for (int i = 0; i < IndexedReads->size(); i++ )
  {
    for (int j = 0; j < IndexedReads->at(i).size(); j++ )
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


