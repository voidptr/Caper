#include "MapviewMappingEngine.h"

MapviewMappingEngine::MapviewMappingEngine(string & aPath, Sequences & aGenome) : MappingEngine(aPath, aGenome), MapviewMappingUtilities()
{
}

string MapviewMappingEngine::GetContigIdent( string & aLine )
{
  return MapviewMappingUtilities::GetContigIdent(aLine);
}

int MapviewMappingEngine::GetIndex( string & aLine )
{
  return MapviewMappingUtilities::GetIndex(aLine);
}

string MapviewMappingEngine::GetSequence( string & aLine )
{
  return MapviewMappingUtilities::GetSequence(aLine);
}


//
//MapviewMappingEngine::MapviewMappingEngine(char * aPath, Sequence* aSequence)
//{
//  CacheA = NULL;
//  CacheB = NULL;
//
//	mPath = aPath;
//	mReferenceGenome = aSequence;
//
//	mNumberOfWindows = (int) ceil( ( double ) ( mReferenceGenome->Bases.length() / IndexIncrement ) );
//
//	mFilePositions = new long[mNumberOfWindows];
//  
//	mLineLengthIsh = 0;
//	mEndOfFilePos = 0;
//}
//
//bool MapviewMappingEngine::Initialize()
//{	
//	mStream.open( mPath.c_str() );
//	InitFileInformation();
//
//	mFilePositions[0] = 0; // the zeroeth one is always at the start of the file.
//
//	for ( int i = 1; i < mNumberOfWindows; i++ )
//	{
//		// start by setting a thousand line jump forward
//		long lPos = FindNextIndexStart( i * IndexIncrement, StartingJumpIncrement, JumpForward );
//
//		if ( lPos != -1 )
//			mFilePositions[i] = lPos;
//		else
//			break; // no more targets to be found.
//	}
//
//	return true; // test whether the thing finished or not.
//
//}
//long MapviewMappingEngine::FindNextIndexStart( 
//	int aTarget, 
//	int aIncrement, 
//	Direction aDirection )
//{
//	long lPreJumpPos = mStream.tellg();
//
//	// execute the jump (as directed)      
//	if ( aDirection == JumpForward )
//		SafeSeek( (int) (mStream.tellg()) + aIncrement - 1 );
//	else
//		SafeSeek( (int) (mStream.tellg()) - aIncrement - 1 );
//
//  char lBuffer[1024]; // this is a complete guess. :/
//  char lDummy[256];
//	// Where are we?
//
//  mStream.getline( lBuffer, 1024 );// jump to next line to clear things up.    
//  //mStream.seekg( -1, mStream.cur );
//  mStream.getline( lBuffer, 1024 );
//  //mStream.seekg( -1, mStream.cur );
//
//  string lLine = lBuffer;
//	int lIndex = GetIndex( lLine );
//
//	if ( lPreJumpPos == mStream.tellg() ) // we are bouncing off a wall. This is it.
//		if ( aDirection == JumpForward )
//			return -1; // short circuit. we are done.
//		else
//			return 0; // this is the closest left line to the index we are looking for.
//
//
//	if ( lIndex < aTarget && aDirection == JumpForward ) // not far enough, keep going forward
//	{
//		return FindNextIndexStart( aTarget, aIncrement, JumpForward );
//	}
//	else if ( lIndex >= aTarget && aDirection == JumpForward ) // too far forward, skip back, slowing down.
//	{
//		return FindNextIndexStart( aTarget, aIncrement / 10, JumpBackward );
//	}
//	else if ( lIndex >= aTarget && aDirection == JumpBackward ) // not far enough backward, keep skipping back.
//	{
//		return FindNextIndexStart( aTarget, aIncrement, JumpBackward );
//	}
//	else if ( lIndex < aTarget && aDirection == JumpBackward ) // too far, skip forward, slowing down.
//	{
//		if ( ( aIncrement / 10 ) <= mLineLengthIsh ) // our expected jumps are now small enough that we just need to plough forward, line by line
//		{
//			while ( mStream.tellg() < mEndOfFilePos )
//			{
//				long lPosLine = mStream.tellg();
//        
//        mStream >> lDummy; // 0
//        mStream >> lDummy; // 1
//        string lIndex = "";
//        mStream >> lIndex; // index
//        
//				int lNormalizedIndex = GetNormalizedIndex( lIndex );
//
//        mStream.getline( lBuffer, 1024 ); // clear the rest of the line.
//        //mStream.seekg( -1, mStream.cur );
//
//				if ( lNormalizedIndex >= aTarget )// FOUND IT
//				{
//					return lPosLine;
//				}
//			}
//			return ( -1 ); // the target we seek doesn't exist in the file.          
//		}
//		else // rather than below, where we organize an even smaller skip forward.
//		{
//			return FindNextIndexStart( aTarget, aIncrement / 10, JumpForward );
//		}
//	}
//	return -1; // HUH?
//	//throw new Exception( "BLEHHHH! HOW CAN THIS BE??!?!" );
//}
//
//void MapviewMappingEngine::SafeSeek( long aTargetSeek )
//{
//	if ( aTargetSeek >= mEndOfFilePos ) // at the end, back it up by a line and a half.
//		mStream.seekg( ( mLineLengthIsh * 1.5 ), mStream.end );
//	else if ( aTargetSeek < 0 )
//		mStream.seekg(0, mStream.beg );
//	else
//		mStream.seekg( aTargetSeek, mStream.beg) ; // do the seek as requested.
//}
//
//int MapviewMappingEngine::GetNormalizedIndex( string aValue )
//{
//	return atoi( aValue.c_str() ) - 1;
//}
//
//int MapviewMappingEngine::GetNormalizedIndex( int aValue )
//{
//	return aValue - 1;
//}
//
//int MapviewMappingEngine::GetIndex( string aLine )
//{
//  char lDummy[256];
//  string lIndex = "";
//
//  istringstream lStream( aLine );
//
//  lStream >> lDummy; // 0;
//  lStream >> lDummy; // 1;
//  lStream >> lIndex;
//
//  return GetNormalizedIndex( lIndex );
//}
//
//void MapviewMappingEngine::InitFileInformation()
//{
//	char lBuffer[1024];
//  mStream.getline( lBuffer, 1024 );
//  string lLine = lBuffer;
//
//  mLineLengthIsh = lLine.length();
//
//	mStream.seekg( 0, mStream.end );
//	mEndOfFilePos = mStream.tellg();
//
//	mStream.seekg( 0, mStream.beg );
//}
//
//vector<Mapping*> MapviewMappingEngine::GetReads( int aLeft, int aRight )
//{
//  vector<Mapping*> lResult;
//
//  int lStartingCache = aLeft / IndexIncrement;
//  int lEndingCache = aRight / IndexIncrement;      
//  int lCachesRequired = lEndingCache - lStartingCache + 1; // this many caches.
//  int lLeftPosition = aLeft; // starting position
//         
//  int lCurrentCachePosition = lStartingCache;
//  for ( int i = 0; i < lCachesRequired; i++ ) // loop through the total number of caches required.
//  {
//    int lRightPosition = aRight < ( lCurrentCachePosition + 1 ) * IndexIncrement ?
//      aRight : (( lCurrentCachePosition + 1 ) * IndexIncrement) - 1;
//
//    MappingCache* lAppropriateCache = GetCorrectCache( lLeftPosition, lRightPosition );
//    vector<Mapping*> lTmp = lAppropriateCache->GetReads( lLeftPosition, lRightPosition );
//    if ( !lTmp.empty() )
//      for (int i = 0; i < lTmp.size(); i++)
//        lResult.push_back( lTmp[i] );
//
//    // these only matter if we do end up going to the next cache.
//    lCurrentCachePosition++;
//    lLeftPosition = lCurrentCachePosition * IndexIncrement; // move the left pointer over to the beginning of the next one.
//  }
//
//  return lResult;
//}
//
//void MapviewMappingEngine::RebuildCaches( int aLeft ) // these define the left edge of the problem.
//{
//  int lStartingIndex = ( aLeft / IndexIncrement );
//
//  delete CacheA;
//  CacheA = RebuildCache( lStartingIndex );
//
//  delete CacheB;
//  if ( lStartingIndex + 1 < mNumberOfWindows )
//    CacheB = RebuildCache( lStartingIndex + 1 );
//}
//
//
//MappingCache* MapviewMappingEngine::GetCorrectCache( int aLeft, int aRight )
//{
//  if ( CacheA != NULL && CacheA->LeftIndex <= aLeft && CacheA->RightIndex >= aLeft )
//    return CacheA;
//  else if ( CacheB != NULL && CacheB->LeftIndex <= aLeft && CacheB->RightIndex >= aLeft )
//    return CacheB;
//  else
//  {
//    RebuildCaches( aLeft ); // neither of them have it.
//    return GetCorrectCache( aLeft, aRight ); // check again.
//  }
//}
//
//MappingCache* MapviewMappingEngine::RebuildCache( int lStartingIndex )
//{
//	long lStartingPos = mFilePositions[ lStartingIndex ];
//
//	long lCount;
//
//	if ( lStartingIndex + 1 < mNumberOfWindows )
//		lCount = mFilePositions[ lStartingIndex + 1 ] - lStartingPos;
//	else
//		lCount = mEndOfFilePos - lStartingPos;
//
//	mStream.seekg( lStartingPos, mStream.beg );
//
//	//string lBuffer;
//	char* lBuffer = new char[ lCount ];
//	mStream.read( lBuffer, lCount );
//
//	MappingCache* lCache = new MappingCache( lBuffer,
//		lStartingIndex * IndexIncrement,
//		( ( lStartingIndex + 1 ) * IndexIncrement ) - 1 );
//
//	delete[] lBuffer;
//
//	return lCache;
//}
//
//void MapviewMappingEngine::DestroyMappings(vector<Mapping*> aMappings)
//{
//  for (int i = 0; i < aMappings.size() ; i++)
//  {
//    delete aMappings[i];
//  }
//  aMappings.clear();
//}
//
//
//MapviewMappingEngine::~MapviewMappingEngine(void)
//{
//	delete[] mFilePositions;
//	mStream.close();
//}
