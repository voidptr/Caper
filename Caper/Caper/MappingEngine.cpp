#include "MappingEngine.h"

MappingEngine::MappingEngine(string aPath, string aIndexPath)
{
//	ReferenceGenome = aReferenceGenome;
	mPath = aPath;
  mIndexPath = aIndexPath;

  CacheA = NULL;
  CacheB = NULL;
}

void MappingEngine::Initialize()
{	

  ifstream lStream( mIndexPath.c_str(), ios::binary );
  if ( !lStream.is_open() )
    throw string("Could not open saved index file.");

  string lDatatype;
  lStream >>lDatatype;

  mMappingUtilities = MappingUtilitiesFactory::BuildMappingUtilities( lDatatype ); // I don't like having this here. This needs to be done before EVERYTHING else... 

  PopulateReadInformation();

  //PopulateContigBorders();
  int lCount = 0;
  lStream >> lCount; // get the count of contigs as the first line.
  for (int i = 0; i < lCount; i++ )
  {
    string lContigIdent = "";
    long lStart = 0;
    long lEnd = 0;

    lStream >> lContigIdent;
    lStream >> lStart;
    lStream >> lEnd;

    mContigBorders.insert( pair<string, pair<long,long> >( 
      lContigIdent, 
      pair<long,long>(lStart, lEnd)));
  }
  
  //PopulateMappingIndex()
  for ( int i = 0; i < lCount; i++ )
  {
    string lContigIdent = "";
    int lReadCount = 0;
    int lWindowsCount = 0;

    lStream >> lContigIdent;
    lStream >> lReadCount;
    lStream >> lWindowsCount;

    NumberOfReads.insert( pair<string, int>( lContigIdent, lReadCount ) );   
    mMappingIndexes.insert( pair<string, vector<long> >( lContigIdent, vector<long>() ) );

    mMappingIndexes[ lContigIdent ].reserve( lWindowsCount ); // preallocate the vector.

    for (int j = 0; j < lWindowsCount; j++ )
    {
      long lIndex = 0;
      lStream >> lIndex;
      mMappingIndexes[ lContigIdent ].push_back( lIndex );
    }
  }
  lStream.close();

  cout << "Done!" << endl;
}

void MappingEngine::PopulateReadInformation()
{	
  ifstream lStream( mPath.c_str(), ios_base::binary );
  if ( !lStream.is_open() )
    throw string("Could not open mappings file.");

  string lLine;
  getline( lStream, lLine );

  ReadLength = mMappingUtilities->GetSequence( lLine ).length();
  
  lStream.seekg( 0, ios::end );
  mEndOfFilePosition = lStream.tellg();

  lStream.close();
}


Mappings * MappingEngine::GetReads(string lContigIdent, int aLeft, int aRight )
{
  Mappings * lResult = new Mappings();

  int lStartingCache = aLeft / IndexIncrement;
  int lEndingCache = aRight / IndexIncrement;      
  int lCachesRequired = lEndingCache - lStartingCache + 1; // this many caches.
  int lLeftPosition = aLeft; // starting position
         
  int lCurrentCachePosition = lStartingCache;
  for ( int i = 0; i < lCachesRequired; i++ ) // loop through the total number of caches required.
  {
    int lRightPosition = aRight < ( lCurrentCachePosition + 1 ) * IndexIncrement ?
      aRight : (( lCurrentCachePosition + 1 ) * IndexIncrement) - 1;

    MappingCache * lAppropriateCache = 
      GetCorrectCache( lContigIdent, lLeftPosition, lRightPosition );

    Mappings * lTmp = lAppropriateCache->GetReads( lLeftPosition, lRightPosition );

    if ( !lTmp->empty() )
      for (int i = 0; i < lTmp->size(); i++)
        lResult->push_back( new Mapping(*(lTmp->at(i))) ); // make a copy of the mapping.

    delete lTmp;

    // these only matter if we do end up going to the next cache.
    lCurrentCachePosition++;
    lLeftPosition = lCurrentCachePosition * IndexIncrement; // move the left pointer over to the beginning of the next one.
  }

  return lResult;
}


MappingCache* MappingEngine::GetCorrectCache(string aContigIdent, int aLeft, int aRight )
{
  if ( CacheA != NULL && 
    CacheA->ContigIdent == aContigIdent && 
    CacheA->LeftIndex <= aLeft && 
    CacheA->RightIndex >= aLeft )
    return CacheA;
  else if ( CacheB != NULL && 
    CacheB->ContigIdent == aContigIdent && 
    CacheB->LeftIndex <= aLeft && 
    CacheB->RightIndex >= aLeft )
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
  else
    CacheB = NULL;
}

MappingCache * MappingEngine::RebuildCache( string aContigIdent, int lStartingWindowIndex )
{  
  long lStartingPos = mMappingIndexes[aContigIdent][ lStartingWindowIndex ];  

  if ( lStartingPos == -1 ) // there's no cache here
  {
    return BuildEmptyCache( 
      aContigIdent, 
      lStartingWindowIndex * IndexIncrement, 
      ( lStartingWindowIndex * IndexIncrement ) + IndexIncrement - 1 );
  }

	long lCount = -1;
  if ( lStartingWindowIndex + 1 < mMappingIndexes[ aContigIdent ].size() )
  {
    for ( int i = lStartingWindowIndex + 1; i < mMappingIndexes[ aContigIdent ].size(); i++ )
    {
      if ( mMappingIndexes[ aContigIdent ][ i ] > -1 )
      {
        lCount = mMappingIndexes[ aContigIdent ][ i ] - lStartingPos;
        break;
      }
    }
  }

  if ( lCount == -1 ) // it's at the edge of the contig!
    lCount = mContigBorders[ aContigIdent ].second - lStartingPos + 1;

  ifstream lStream( mPath.c_str(), ios_base::binary );

  char * lBlock = new char[ lCount + 1 ];  
  lStream.seekg( lStartingPos );
  lStream.read( lBlock, lCount );
  lBlock[ lCount ] = 0; // impose our own null termination

  MappingCache * lCache = BuildCache( lBlock, aContigIdent, lStartingWindowIndex * IndexIncrement, ( ( lStartingWindowIndex + 1 ) * IndexIncrement ) - 1 );

  delete [] lBlock;
  lStream.close();

  return lCache;
}

MappingCache* MappingEngine::BuildEmptyCache( string aContigIdent, int aLeft, int aRight )
{
  IndexedMappings * lMappings = new IndexedMappings();

  lMappings->reserve( aRight - aLeft ); // avoid dynamic reallocation.
  for ( int i = aLeft; i <= aRight; i++ ) // do I need to do this, or just say resize()? What is the default value?
  {
    lMappings->push_back( Mappings() );
  }

  return new MappingCache( lMappings, aContigIdent, aLeft, aRight );
}

// This (and related) method should really be in a separate MappingCache Builder.
MappingCache * MappingEngine::BuildCache( char * aBlock, string aContigIdent, int aLeft, int aRight )
{
  MappingCache * lCache = BuildEmptyCache( aContigIdent, aLeft, aRight );

  string lBlock(aBlock);

  istringstream lStream(lBlock, ios_base::binary);

  string lLine;

  while ( lStream.peek() >= 0 )
  {
    getline( lStream, lLine );
    if ( lLine.length() < 1 )
      break;

    int lIndex = mMappingUtilities->GetIndex( lLine );
    if ( lIndex < aLeft ) {	// @CTB may only need to check for first pass?
      break; // @RCK this should never happen. Investigate further.
    }

    int lPrivateIndex = lIndex - aLeft;
   
    string lSeq = mMappingUtilities->GetSequence( lLine );
    string lName = mMappingUtilities->GetName( lLine );

    // This is probably not the right place for this, since, concievably, not all mapping
    // types use +/- to denote strand. But where else to put, w/o making
    // a global Orientation enum? This probably needs a neat namespace somewhere.
    string lStrand = mMappingUtilities->GetStrand( lLine );
    Mapping::Orientation lOrientation;
    if ( lStrand[0] == '+' )
      lOrientation = Mapping::PLUS;
    else if ( lStrand[0] == '-' )
      lOrientation = Mapping::MINUS;
    else 
      throw string("Unsupported Orientation Type (not + or -)");

    lCache->IndexedReads->at(lPrivateIndex).push_back( 
      new Mapping( lName, lIndex, new Sequence( lSeq ), lOrientation ) );
  }

  return lCache;
}

