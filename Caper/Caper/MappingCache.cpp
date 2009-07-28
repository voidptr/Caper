#include "MappingCache.h"

MappingCache::MappingCache( vector<vector<Mapping>> * aMappings, string & aContigIdent, int & aLeftIndex, int & aRightIndex )
{
  Mappings = aMappings;

  ContigIdent = aContigIdent;
  LeftIndex = aLeftIndex;
  RightIndex = aRightIndex;
}

vector<Mapping> * MappingCache::GetReads( int aLeft, int aRight )
{
  vector<Mapping> * lResult = new vector<Mapping>();

  for ( int i = PrivateIndex(aLeft); i <= PrivateIndex(aRight); i++ )
  {
    if ( !Mappings->at(i).empty() )
    {
      for ( int j = 0; j < Mappings->at(i).size() ; j++ )
      {        
        lResult->push_back( Mappings->at(i)[j] );
      }
    }
  }

  return lResult;
}

int MappingCache::PrivateIndex( int aPublicIndex )
{
  return aPublicIndex - LeftIndex;
}

MappingCache::~MappingCache()
{
  delete Mappings;
}
