#include "MappingEngine.h"

MappingEngine::MappingEngine(string & aPath, Sequences & aReferenceGenome)
{
	ReferenceGenome = &aReferenceGenome;
	mPath = aPath;

  CacheA = NULL;
  CacheB = NULL;
}

void MappingEngine::Initialize()
{		
  PopulateReadInformation();
  PopulateSortedContigIdents();
  PopulateNumberOfWindows();
  PopulateContigBorders();
  PopulateMappingIndex();
}

void MappingEngine::PopulateSortedContigIdents()
{		
	for each ( pair<string,Sequence> lItem in *ReferenceGenome )
	{
		mSortedContigIdents.push_back( lItem.first );
	}

	//mSortedContigIdents.sort(); // don't know if this matters.
}

void MappingEngine::PopulateContigBorders()
{		
	ifstream lStream( mPath.c_str() );

  string lContig = "";
  long lContigStartingPos = 0;
  string lLine;
  long lCurrentPosition = 0;

  while ( lStream.peek() > -1 )
  {
    lCurrentPosition = lStream.tellg();
    getline( lStream, lLine );
    string lCurrentContig = GetContigIdent( lLine );
    if ( lCurrentContig != lContig )
    {
      if ( lContig.length() > 0 ) // there was a previous one, so close it up
      {
        mContigBorders.insert( pair<string, pair<long,long>>( 
          lContig, 
          pair<long,long>(lContigStartingPos, lCurrentPosition - 1)));
      }
      lContigStartingPos = lCurrentPosition;
      lContig = lCurrentContig;
    }
  }
  // close up the last contig
  mContigBorders.insert( pair<string, pair<long,long>>( 
    lContig, 
    pair<long,long>(lContigStartingPos, (long) lStream.tellg() - 1)));

  lStream.close();
}

void MappingEngine::PopulateNumberOfWindows()
{		
	for each ( pair<string,Sequence> lItem in *ReferenceGenome )
	{
		mNumberOfWindows.insert( pair<string, int>( lItem.first,
      (int) ceil( (double) lItem.second.Length / IndexIncrement ) ) );
	}	
}

void MappingEngine::PopulateReadInformation()
{	
  ifstream lStream( mPath.c_str() );

  string lLine;
  getline( lStream, lLine );

  mReadLength = GetSequence( lLine ).length();
  
  lStream.seekg( 0, ios::end );
  mEndOfFilePosition = lStream.tellg();

  lStream.close();
}

void MappingEngine::PopulateMappingIndex()
{		
  ifstream lStream( mPath.c_str() );

  string lContig = "";
  long lCurrentPosition = 0;
  int lTargetIndex = 0;
  int lReadCount = 0;
  
  string lLine;

  while ( lStream.peek() > -1 )
  {
    lCurrentPosition = lStream.tellg();
    getline( lStream, lLine );
    string lCurrentContig = GetContigIdent( lLine );
    int lCurrentIndex = GetIndex( lLine );

    if ( lCurrentContig != lContig )
    {
      if ( lContig.length() > 0 ) // there was a previous one, so close it up
      {
        NumberOfReads.insert( pair<string, int>( lContig, lReadCount ) );        
        lReadCount = 0;
      }

      lTargetIndex = 0;
      lContig = lCurrentContig;
      mMappingIndexes.insert( pair<string, vector<long>>( lContig, vector<long>() ) );
    }

    if ( lCurrentIndex >= lTargetIndex )
    {
      if ( lCurrentIndex >= lTargetIndex + IndexIncrement ) // overshot
      {
        mMappingIndexes[ lContig ].push_back( -1 ); // indicating that this section is empty.
        lStream.seekg( lCurrentPosition ); // unconsume the line, since we'll need it soon.
      }
      else
        mMappingIndexes[ lContig ].push_back( lCurrentPosition );

      lTargetIndex += IndexIncrement;
    }
    lReadCount++;
  }
  NumberOfReads.insert( pair<string, int>( lContig, lReadCount ) ); // close up the last contig read count

  lStream.close();
}



MappingCache* MappingEngine::BuildEmptyCache( string aContigIdent, int aLeft, int aRight )
{
  vector<vector<Mapping>> * lMappings = new vector<vector<Mapping>>();

  for ( int i = aLeft; i <= aRight; i++ )
  {
    lMappings->push_back( vector<Mapping>() );
  }

  return new MappingCache( lMappings, aContigIdent, aLeft, aRight );
}

vector<Mapping> * MappingEngine::GetReads(string lContigIdent, int aLeft, int aRight )
{
  vector<Mapping> * lResult = new vector<Mapping>();

  int lStartingCache = aLeft / IndexIncrement;
  int lEndingCache = aRight / IndexIncrement;      
  int lCachesRequired = lEndingCache - lStartingCache + 1; // this many caches.
  int lLeftPosition = aLeft; // starting position
         
  int lCurrentCachePosition = lStartingCache;
  for ( int i = 0; i < lCachesRequired; i++ ) // loop through the total number of caches required.
  {
    int lRightPosition = aRight < ( lCurrentCachePosition + 1 ) * IndexIncrement ?
      aRight : (( lCurrentCachePosition + 1 ) * IndexIncrement) - 1;

    MappingCache * lAppropriateCache = GetCorrectCache( lContigIdent, lLeftPosition, lRightPosition );
    vector<Mapping> * lTmp = lAppropriateCache->GetReads( lLeftPosition, lRightPosition );
    if ( !lTmp->empty() )
      for (int i = 0; i < lTmp->size(); i++)
        lResult->push_back( (*lTmp)[i] );

    // these only matter if we do end up going to the next cache.
    lCurrentCachePosition++;
    lLeftPosition = lCurrentCachePosition * IndexIncrement; // move the left pointer over to the beginning of the next one.
  }

  return lResult;
}

MappingCache* MappingEngine::GetCorrectCache(string aContigIdent, int aLeft, int aRight )
{
  if ( CacheA != NULL && CacheA->ContigIdent == aContigIdent && CacheA->LeftIndex <= aLeft && CacheA->RightIndex >= aLeft )
    return CacheA;
  else if ( CacheB != NULL && CacheB->ContigIdent == aContigIdent && CacheB->LeftIndex <= aLeft && CacheB->RightIndex >= aLeft )
    return CacheB;
  else
  {
    RebuildCaches( aContigIdent, aLeft ); // neither of them have it.
    return GetCorrectCache( aContigIdent, aLeft, aRight ); // check again.
  }
}

void MappingEngine::RebuildCaches(string aContigIdent, int aLeft ) // these define the left edge of the problem.
{
  int lStartingIndex = ( aLeft / IndexIncrement );

  delete CacheA;
  CacheA = RebuildCache( aContigIdent, lStartingIndex );

  delete CacheB;
  if ( lStartingIndex + 1 < mMappingIndexes[ aContigIdent ].size() )
    CacheB = RebuildCache( aContigIdent, lStartingIndex + 1 );
}

MappingCache * MappingEngine::RebuildCache( string aContigIdent, int lStartingIndex )
{
	//long lStartingPos = mMappingIndexes[aContigIdent][ lStartingIndex ];

 // if ( lStartingPos == -1 ) // there's no cache here
 // {
 //   return BuildEmptyCache( 
 //     aContigIdent, 
 //     lStartingIndex * IndexIncrement, 
 //     ( lStartingIndex * IndexIncrement ) + IndexIncrement - 1 );
 // }

	//long lCount = -1;

  //if ( lStartingIndex + 1 < [ aContigIdent ].size() )
  //  for ( int i = lStartingIndex + 1; i < mMappingIndexes[ aContigIdent ].size(); i++ )
  //  {
  //    if ( mMappingIndexes[ aContigIdent ][ i ] > -1 )
  //    {
  //      lCount = mMappingIndexes[ aContigIdent ][ i ] - lStartingPos;
  //      break;
  //    }
  //  }

  //if ( lCount == -1 ) // it's at the edge of the contig!
  //  lCount = mContigBorders[ aContigIdent ].second() - lStartingPos + 1;

  //ifstream lStream( Path );
  
  //char * lBlock = new char[ lCount ];
  ///*mStream.Position = lStartingPos;
  //mStream.BaseStream.Read( lBlock, 0, ( int ) lCount );

  
  
  MappingCache * lCache; // = BuildCache( lBlock, aContigIdent, lStartingIndex * IndexIncrement, ( ( lStartingIndex + 1 ) * IndexIncrement ) - 1 );

  return lCache;



	//if ( lStartingIndex + 1 < mMappingIndexes[ aContigIdent ].size() )
	//	lCount = mFilePositions[ lStartingIndex + 1 ] - lStartingPos;
	//else
	//	lCount = mEndOfFilePos - lStartingPos;

	//mStream.seekg( lStartingPos, mStream.beg );

	////string lBuffer;
	//char* lBuffer = new char[ lCount ];
	//mStream.read( lBuffer, lCount );

	//MappingCache* lCache = new MappingCache( lBuffer,
	//	lStartingIndex * IndexIncrement,
	//	( ( lStartingIndex + 1 ) * IndexIncrement ) - 1 );

	//delete[] lBuffer;

	//return lCache;
}

MappingCache * MappingEngine::BuildCache( char * aBlock, string aContigIdent, int aLeft, int aRight )
{
  MappingCache * lCache;

  return lCache;
}