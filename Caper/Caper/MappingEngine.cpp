#include "MappingEngine.h"
#include "Typedefs.h" // DEV Intellisense
#include <sstream>
#include <iostream>

MappingEngine::MappingEngine(string aPath, Sequences aReferenceGenome)
{
	ReferenceGenome = &aReferenceGenome;
	mPath = aPath;

  CacheA = NULL;
  CacheB = NULL;
}

void MappingEngine::Initialize()
{		
  PopulateReadInformation();
  PopulateNumberOfWindows();
  PopulateContigBorders();  
  cout << " Indexing Mappings... ";
  cout.flush();
  PopulateMappingIndex();  
  cout << "Done!" << endl;
}

void MappingEngine::Initialize( string & aIndexPath )
{	
  PopulateReadInformation();
  PopulateNumberOfWindows();

  //PopulateContigBorders();
  ifstream lStream( aIndexPath.c_str(), ios::binary );
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

void MappingEngine::SaveMappingIndex( string & aSavePath )
{
  string lOutputFilename = aSavePath + "saved.index";

  ofstream lStream( lOutputFilename.c_str(), ios::binary );

  // saving the contig borders
  lStream << mContigBorders.size() << endl;
  map<string, pair<long,long> >::iterator lContigBordersIterator;
  for ( lContigBordersIterator = mContigBorders.begin(); 
    lContigBordersIterator != mContigBorders.end(); lContigBordersIterator++ )
  {
    lStream << lContigBordersIterator->first << Tab << lContigBordersIterator->second.first << Tab << lContigBordersIterator->second.second << endl;
  }

  //saving the mapping index
  map<string, vector<long> >::iterator lMappingIndexIterator;
  for ( lMappingIndexIterator = mMappingIndexes.begin(); lMappingIndexIterator != mMappingIndexes.end(); lMappingIndexIterator++ )
  {
    lStream << lMappingIndexIterator->first << Tab << NumberOfReads[ lMappingIndexIterator->first ] << Tab << lMappingIndexIterator->second.size() << endl;
    vector<long>::iterator lIndexIterator;
    for ( lIndexIterator = mMappingIndexes[ lMappingIndexIterator->first ].begin(); lIndexIterator != mMappingIndexes[ lMappingIndexIterator->first ].end(); lIndexIterator++ )
    {
      lStream << *lIndexIterator << endl;
    }
  }

  lStream.close();
}

void MappingEngine::PopulateContigBorders()
{		
  ifstream lStream( mPath.c_str(), ios_base::binary );
  if ( !lStream.is_open() )
    throw string("Could not open mappings file.");

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
        mContigBorders.insert( pair<string, pair<long,long> >( 
          lContig, 
          pair<long,long>(lContigStartingPos, lCurrentPosition - 1)));
      }
      lContigStartingPos = lCurrentPosition;
      lContig = lCurrentContig;
    }
  }
  // close up the last contig
  mContigBorders.insert( pair<string, pair<long,long> >( 
    lContig, 
    pair<long,long>(lContigStartingPos, (long) lStream.tellg() - 1)));

  lStream.close();
}

void MappingEngine::PopulateNumberOfWindows()
{		
  Sequences::iterator lIterator;
  for(lIterator = ReferenceGenome->begin() ; lIterator != ReferenceGenome->end(); lIterator++ )
	{
		mNumberOfWindows.insert( pair<string, int>( lIterator->first,
      (int) ceil( (double) lIterator->second->Length / IndexIncrement ) ) );
	}	
}

void MappingEngine::PopulateReadInformation()
{	
  ifstream lStream( mPath.c_str(), ios_base::binary );
  if ( !lStream.is_open() )
    throw string("Could not open mappings file.");

  string lLine;
  getline( lStream, lLine );

  ReadLength = GetSequence( lLine ).length();
  
  lStream.seekg( 0, ios::end );
  mEndOfFilePosition = lStream.tellg();

  lStream.close();
}

void MappingEngine::PopulateMappingIndex()
{		
  ifstream lStream( mPath.c_str(), ios::binary );
  if ( !lStream.is_open() )
    throw string("Could not open mappings file.");

  string lContig = "";
  long lCurrentPosition = 0;
  int lTargetIndex = 0;
  int lReadCount = 0;
  
  while ( lStream.peek() > -1 )
  {
    string lLine = "";
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
      mMappingIndexes.insert( pair<string, vector<long> >( lContig, vector<long>() ) );
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

MappingCache * MappingEngine::RebuildCache( string aContigIdent, int lStartingIndex )
{
	long lStartingPos = mMappingIndexes[aContigIdent][ lStartingIndex ];

  if ( lStartingPos == -1 ) // there's no cache here
  {
    return BuildEmptyCache( 
      aContigIdent, 
      lStartingIndex * IndexIncrement, 
      ( lStartingIndex * IndexIncrement ) + IndexIncrement - 1 );
  }

	long lCount = -1;
  if ( lStartingIndex + 1 < mMappingIndexes[ aContigIdent ].size() )
  {
    for ( int i = lStartingIndex + 1; i < mMappingIndexes[ aContigIdent ].size(); i++ )
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
  
  MappingCache * lCache = BuildCache( lBlock, aContigIdent, lStartingIndex * IndexIncrement, ( ( lStartingIndex + 1 ) * IndexIncrement ) - 1 );

  delete [] lBlock;
  lStream.close();

  return lCache;
}

MappingCache* MappingEngine::BuildEmptyCache( string aContigIdent, int aLeft, int aRight )
{
  IndexedMappings * lMappings = new IndexedMappings();

  for ( int i = aLeft; i <= aRight; i++ )
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
    int lChar = lStream.peek();
    getline( lStream, lLine );
    if ( lLine.length() < 1 )
      break;

    int lIndex = GetIndex( lLine );
    int lPrivateIndex = lIndex - aLeft;
   
    string lSeq = GetSequence( lLine );
    string lName = GetName( lLine );

    lCache->IndexedReads->at(lPrivateIndex).push_back( new Mapping(lName, lIndex, new Sequence( lSeq ) ) );
  }

  return lCache;
}
