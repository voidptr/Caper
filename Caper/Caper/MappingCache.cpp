#include "MappingCache.h"

MappingCache::MappingCache( char * aBuffer, int aLeftIndex, int aRightIndex )
{
  Sequences = new vector<Mapping*>[ aRightIndex - aLeftIndex + 1 ];

  LeftIndex = aLeftIndex;
  RightIndex = aRightIndex;

  Initialize( aBuffer );
}
bool MappingCache::Initialize( char * aBuffer )
{
  istringstream lStringStream( aBuffer );
  string lLine;

  char lBuffer[1024];

  while ( lStringStream.getline( lBuffer, 1024 ) && lStringStream.gcount() > 100 ) // improbably small
  {
    int lCount = lStringStream.gcount();

    istringstream lLineStream( lBuffer );
    char lDummy[256];
    lLineStream >> lDummy; // 0;
    lLineStream >> lDummy; // 1;
    string lIndex;
    lLineStream >> lIndex;
    int lNormalizedIndex = MapviewMappingEngine::GetNormalizedIndex( lIndex );

    for (int i = 3; i < 14; i++ )
    {
      lLineStream >> lDummy; // 3 - 13
    }
    string lSequence = "";
    lLineStream >> lSequence;

    int lPrivateIndex = PrivateIndex( lNormalizedIndex );

    Sequences[ lPrivateIndex ].push_back( new Mapping( lNormalizedIndex, new Sequence( lSequence ) ) );
  }

  return true;
}

vector<Mapping*> MappingCache::GetReads( int aLeft, int aRight )
{
  vector<Mapping*> lResult;

  for ( int i = PrivateIndex(aLeft); i <= PrivateIndex(aRight); i++ )
  {
    if ( !Sequences[ i ].empty() )
    {
      for ( int j = 0; j < Sequences[i].size() ; j++ )
      {
        lResult.push_back( new Mapping( Sequences[i][j]->Index, new Sequence( Sequences[i][j]->mSequence->Bases ) ) );
      }
    }
  }

  return lResult;
}

int MappingCache::PrivateIndex( int aPublicIndex )
{
  return aPublicIndex - LeftIndex;
}

MappingCache::~MappingCache(void)
{
  for (int i = 0; i < PrivateIndex(RightIndex) ; i++)
  {
    if ( !Sequences[i].empty() )
    {
      for (int j = 0; j < Sequences[i].size(); j++ )
      {
        delete Sequences[i][j];
      }
    }
  }
  delete[] Sequences;
}
