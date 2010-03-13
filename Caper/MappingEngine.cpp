#include "MappingEngine.h"

MappingEngine::MappingEngine( string aPath, string aIndexPath )
{
	mPath = aPath;
  mIndexPath = aIndexPath;
  mPathStartOffset = 0;
  mBundle = false;
  ReadLength = -1;

  CacheA = NULL;
  CacheB = NULL;
}

MappingEngine::MappingEngine( string aBundlePath )
{
	mPath = aBundlePath;
  mIndexPath = aBundlePath;
  mPathStartOffset = -1;
  mBundle = true;
  ReadLength = -1;

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

  //PopulateContigBorders();
  int lCount = 0;
  lStream >> lCount; // get the count of contigs as the first line.
  for (int i = 0; i < lCount; i++ )
  {
    string lContigIdent = "";
    long long lStart = 0;
    long long lEnd = 0;

    lStream >> lContigIdent;
    lStream >> lStart;
    lStream >> lEnd;

    mContigBorders.insert( pair<string, pair<long long,long long> >( 
      lContigIdent, 
      pair<long long,long long>(lStart, lEnd)));
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
    mMappingIndexes.insert( pair<string, vector<StoredMappingBlock> >( lContigIdent, vector<StoredMappingBlock>() ) );

    mMappingIndexes[ lContigIdent ].reserve( lWindowsCount ); // preallocate the vector.

    for (int j = 0; j < lWindowsCount; j++ ) // could this break something? see where we are getting this window count.
    {
      long long lIndex = 0;
      int lBlockSize = 0;
      int lStoredSize = 0;
      lStream >> lIndex; // TODO have the reading of these reside in the StoredMappingBlock class (which does not yet exist), same with the out. Need to decouple this crap.
      lStream >> lBlockSize; // ditto
      lStream >> lStoredSize; // ditto
      mMappingIndexes[ lContigIdent ].push_back( StoredMappingBlock( lIndex, lBlockSize, lStoredSize ) );
    }
  }

  if ( mBundle ) // remember the offset.
    mPathStartOffset = lStream.tellg() + 1l; // the start of the rest of the bundle.

  lStream.close();


  PopulateReadInformation(); // this depends on having the index fully populated.

  cout << "Done!" << endl;
}

int MappingEngine::GetReadLength()
{
  if (ReadLength > -1 ) // we've already got one!
    return ReadLength;

  string lTargetContig = mMappingIndexes.begin()->first;

  int lTargetWindow = 0;
  while ( mMappingIndexes[ lTargetContig ][ lTargetWindow ].first == -1 ) // loop through the listed windows until we find a populated one.
    lTargetWindow++; // if we overrun here, that's fine. We have an empty contig, which should never ever happen, so it's fine to die messily, since we did something really wrong upstream.

  char * lBlock = FetchBlock( lTargetContig, lTargetWindow ); // fetch the first block in the database. Hopefully it's not empty.
  
  //string lStringBlock(lBlock); // long string. ;)
  istringstream lStream( string(lBlock), ios_base::binary);

  string lLine;
  getline( lStream, lLine );
  ReadLength = mMappingUtilities->GetSequence( lLine ).length();

  return ReadLength;
}


void MappingEngine::PopulateReadInformation()
{	
  ifstream lStream( mPath.c_str(), ios_base::binary );
  if ( !lStream.is_open() )
    throw string("Could not open mappings file.");
  
  lStream.seekg( 0, ios::end );
  mEndOfFilePosition = lStream.tellg();

  lStream.close();
}


Mappings * MappingEngine::GetReads(string lContigIdent, long long aLeft, long long aRight )
{
  Mappings * lResult = new Mappings();

  long long lStartingCache = aLeft / IndexIncrement;
  long long lEndingCache = aRight / IndexIncrement;      
  long long lCachesRequired = lEndingCache - lStartingCache + 1; // this many caches.
  long long lLeftPosition = aLeft; // starting position
         
  long long lCurrentCachePosition = lStartingCache;
  for ( long long i = 0; i < lCachesRequired; ++i ) // loop through the total number of caches required.
  {
    long long lRightPosition = aRight < ( lCurrentCachePosition + 1 ) * IndexIncrement ?
      aRight : (( lCurrentCachePosition + 1 ) * IndexIncrement) - 1;

    MappingCache * lAppropriateCache = 
      GetCorrectCache( lContigIdent, lLeftPosition, lRightPosition );

    Mappings * lTmp = lAppropriateCache->GetReads( lLeftPosition, lRightPosition );

    if ( !lTmp->empty() )
      for (long long i = 0; i < lTmp->size(); ++i)
        lResult->push_back( new Mapping(*(lTmp->at(i))) ); // make a copy of the mapping.

    delete lTmp;

    // these only matter if we do end up going to the next cache.
    lCurrentCachePosition++;
    lLeftPosition = lCurrentCachePosition * IndexIncrement; // move the left pointer over to the beginning of the next one.
  }

  return lResult;
}


MappingCache* MappingEngine::GetCorrectCache(string aContigIdent, long long aLeft, long long aRight )
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

void MappingEngine::RebuildCaches(string aContigIdent, long long aLeft ) // these define the left edge of the problem.
{
  long long lStartingIndex = ( aLeft / IndexIncrement );

  delete CacheA;
  CacheA = RebuildCache( aContigIdent, lStartingIndex );

  delete CacheB;
  if ( lStartingIndex + 1 < mMappingIndexes[ aContigIdent ].size() )
    CacheB = RebuildCache( aContigIdent, lStartingIndex + 1 );
  else
    CacheB = NULL;
}

char * MappingEngine::FetchBlock( string aContigIdent, long long aStartingWindowIndex )
{
  long long lStartingPos = mMappingIndexes[aContigIdent][ aStartingWindowIndex ].first;
  long long lBlockSize = mMappingIndexes[aContigIdent][ aStartingWindowIndex ].second;
  long long lStoredSize = mMappingIndexes[aContigIdent][ aStartingWindowIndex ].third;

  if ( lStartingPos == -1 )
    return NULL; // a valid return value, where there is no block to fetch.

  // now, fetch the block
  ifstream lStream( mPath.c_str(), ios_base::binary );
  if ( !lStream.is_open() )
    throw string("Could not open mapping file.");

  char * lBlock = new char[ lStoredSize + 1 ]; 
  OffsetSeek( lStream, lStartingPos );
  lStream.read( lBlock, lStoredSize );

  lStream.close();

  // is it compressed?
  if ( lStoredSize != lBlockSize )
    DecompressBlock( lBlock, lStoredSize, lBlockSize );
  else
    lBlock[ lStoredSize ] = 0; // impose our own null termination BLEH TODO

  return lBlock;
}

void MappingEngine::DecompressBlock( char *&lBlock, long long aStoredSize, long long aBlockSize )
{
  Bytef *lUncompressed;
  unsigned long lUncompressedLength = aBlockSize; // do these go negative?
  lUncompressed = new Bytef[ aBlockSize + 1 ]; // allocate the memory on the fly.
  assert( lUncompressed != NULL ); // temporary. TODO handle this better, clean up after myself.

  int lErrorCode = uncompress(lUncompressed, &lUncompressedLength, (Bytef *) lBlock, aStoredSize);
  assert( lErrorCode == 0 ); // temporary. TODO handle this better, clean up after myself.

  delete [] lBlock;

  lBlock = ( char * ) lUncompressed;

  lBlock[ aBlockSize ] = 0; // impose our own null termination BLEH TODO
}


MappingCache * MappingEngine::RebuildCache( string aContigIdent, long long lStartingWindowIndex )
{  
  long long lStartingPos = mMappingIndexes[aContigIdent][ lStartingWindowIndex ].first;

  char * lBlock = FetchBlock( aContigIdent, lStartingWindowIndex );

  if ( lBlock == NULL ) // no catch exists.
  {
    return BuildEmptyCache( 
      aContigIdent, 
      lStartingWindowIndex * IndexIncrement, 
      ( lStartingWindowIndex * IndexIncrement ) + IndexIncrement - 1 );
  }

  MappingCache * lCache = BuildCache( lBlock, aContigIdent, lStartingWindowIndex * IndexIncrement, ( ( lStartingWindowIndex + 1 ) * IndexIncrement ) - 1 );
  
  delete [] lBlock;

  return lCache;
}

void MappingEngine::OffsetSeek( ifstream & aStream, long long aPosition )
{
  if ( mBundle )
    aStream.seekg( mPathStartOffset + aPosition );
  else
    aStream.seekg( aPosition );
}

MappingCache* MappingEngine::BuildEmptyCache( string aContigIdent, long long aLeft, long long aRight )
{
  IndexedMappings * lMappings = new IndexedMappings();

  lMappings->reserve( aRight - aLeft ); // avoid dynamic reallocation.
  for ( long long i = aLeft; i <= aRight; ++i ) // do I need to do this, or just say resize()? What is the default value?
  {
    lMappings->push_back( Mappings() );
  }

  return new MappingCache( lMappings, aContigIdent, aLeft, aRight );
}

// This (and related) method should really be in a separate MappingCache Builder.
MappingCache * MappingEngine::BuildCache( char * aBlock, string aContigIdent, long long aLeft, long long aRight )
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

    long long lIndex = mMappingUtilities->GetIndex( lLine );
    if ( lIndex < aLeft ) {	// @CTB may only need to check for first pass?
      throw string("WTF"); // TEMPORARY
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

