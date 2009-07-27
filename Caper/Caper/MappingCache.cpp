#include "MappingCache.h"

MappingCache::MappingCache( vector<vector<Mapping>> * aMappings, string & aContigIdent, int & aLeftIndex, int & aRightIndex )
{
  Sequences = aMappings;

  ContigIdent = aContigIdent;
  LeftIndex = aLeftIndex;
  RightIndex = aRightIndex;
}

vector<Mapping> * MappingCache::GetReads( int aLeft, int aRight )
{
  vector<Mapping> * lResult = new vector<Mapping>();

  for ( int i = PrivateIndex(aLeft); i <= PrivateIndex(aRight); i++ )
  {
    if ( !Sequences[ i ].empty() )
    {
      for ( int j = 0; j < Sequences[i].size() ; j++ )
      {
        lResult->push_back( (*Sequences)[i][j] );
      }
    }
  }

  return lResult;
}

int MappingCache::PrivateIndex( int aPublicIndex )
{
  return aPublicIndex - LeftIndex;
}

