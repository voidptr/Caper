#include "MappingsPreparer.h"

#include "BowtieMappingUtilities.h"

MappingsPreparer::MappingsPreparer(string & aPath)
{
  mPath = aPath;
}

string MappingsPreparer::PrepareMappings()
{
  if (!IsSorted())
    return SortMappingsAndWriteToTmpFile();

  return mPath;
}

bool MappingsPreparer::IsSorted()
{
  ifstream lStream( mPath.c_str() );

  string lLastContig = "";
  int lLastIndex = -1;

  string lLine;
  while ( lStream.peek() > -1 )
  {
    getline( lStream, lLine );

    string lContig = GetContigIdent( lLine );
    int lIndex = GetIndex( lLine );

    if ( lContig.compare( lLastContig ) < 0 || 
      ( lContig == lLastContig && lIndex < lLastIndex ) )
    {
      lStream.close();
      return false;
    }
    lLastContig = lContig;
    lLastIndex = lIndex;
  }
  lStream.close();
  return true;
}

vector<string>* MappingsPreparer::ReadAllLines()
{
  ifstream lStream( mPath.c_str() );

  vector<string> * lList = new vector<string>();

  while (lStream.peek() > -1 )
  {
    string lLine;
    getline( lStream, lLine );

    lList->push_back( lLine );
  }

  lStream.close();
  return lList;
}
string MappingsPreparer::SortMappingsAndWriteToTmpFile()
{
  vector<string> * lMappings = ReadAllLines();

  // I WOULD MUCH RATHER BE DOING THIS WITH A PREDICATE, but I can't quite
  // figure this out right now. Stupid C++. Might do it by pre-creating mapping objects
  Introsort( lMappings, 0, lMappings->size() - 1, 0, (int) log( (double) lMappings->size() ) );

  string lFilename = mPath + ".tmp";
  WriteAllLines( lMappings, lFilename );

  return lFilename;
}

void MappingsPreparer::Introsort( vector<string> * aLines, int aFirstIndex, int aLastIndex, int aCurrentDepth, int aSwitchDepth )
{
  if ( aLastIndex > aFirstIndex )
  {
    if ( ++aCurrentDepth < aSwitchDepth )
    {
      cout << "Quick: F:" << aFirstIndex << ", L:" << aLastIndex << ", CD:" << aCurrentDepth << ", SD:" << aSwitchDepth << endl;
      int lNewPivotIndex = Partition( aLines, aFirstIndex, aLastIndex, aLastIndex );

      Introsort( aLines, aFirstIndex, lNewPivotIndex - 1, aCurrentDepth, aSwitchDepth );
      Introsort( aLines, lNewPivotIndex + 1, aLastIndex, aCurrentDepth, aSwitchDepth );
    }
    else
    {
      cout << "Heap: F:" << aFirstIndex << ", L:" << aLastIndex << endl;
      Heapsort( aLines, aFirstIndex, aLastIndex - aFirstIndex + 1 );
    }
  }
}

int MappingsPreparer::Partition( vector<string> * aLines, int aFirstIndex, int aLastIndex, int aPivotIndex )
{
  string lPivotLine = (*aLines)[aPivotIndex];
  string lContig = GetContigIdent( lPivotLine );

  int lStoreIndex = aFirstIndex;
  for ( int lIndex = aFirstIndex; lIndex < aLastIndex; lIndex++ )
  {
    string lTestLine = (*aLines)[lIndex];

    if ( CompareMappingLine( lTestLine, lPivotLine ) < 1 )
    {
      SwapLines( aLines, lIndex, lStoreIndex );

      lStoreIndex++;
    }
  }
  SwapLines( aLines, lStoreIndex, aPivotIndex );
  return lStoreIndex;
}

void MappingsPreparer::SwapLines( vector<string> * aLines, int aFirst, int aSecond )
{
  if ( aFirst != aSecond )
  {
    string lFirstLine = (*aLines)[ aFirst ];
    (*aLines)[ aFirst ] = (*aLines)[ aSecond ];
    (*aLines)[ aSecond ] = lFirstLine;
  }
}

int MappingsPreparer::CompareMappingLine( string & aLeft, string & aRight )
{
  if ( aLeft == aRight ) // if they are identical, woohoo!
    return 0;

  string lLeftContig = GetContigIdent( aLeft );
  string lRightContig = GetContigIdent( aRight );

  if ( lLeftContig.compare( lRightContig ) < 0 )
    return -1;
  else if ( lLeftContig == lRightContig )
  {
    int lLeftIndex = GetIndex( aLeft );
    int lRightIndex = GetIndex( aRight );

    if ( lLeftIndex < lRightIndex )
      return -1;
    else if ( lLeftIndex == lRightIndex )
      return 0;
    else
      return 1;
  }
  else
    return 1;
}
  
void MappingsPreparer::Heapsort( vector<string> * aLines, int aOffset, int aCount )
{
  Heapify( aLines, aOffset, aCount );

  int lEnd = aCount - 1;

  while ( lEnd > 0 )
  {
    SwapLines( aLines, lEnd + aOffset, aOffset );
    SiftDown( aLines, 0, lEnd - 1, aOffset );
    lEnd--;
  }
}
  
void MappingsPreparer::Heapify( vector<string> * aLines, int aOffset, int aCount )
{
  int lStart = ( ( aCount - 2 ) / 2 );
  while ( lStart >= 0 )
  {
    SiftDown( aLines, lStart, aCount - 1, aOffset );
    lStart--;
  }
}
  
void MappingsPreparer::SiftDown( vector<string> * aLines, int aStart, int aEnd, int aOffset )
{
  int lRoot = aStart;

  while ( ( lRoot * 2 ) + 1 <= aEnd )
  {
    int lChild = lRoot * 2 + 1;

    if ( lChild + 1 <= aEnd &&
      CompareMappingLine( 
        (*aLines)[ lChild + aOffset ], 
        (*aLines)[ lChild + 1 + aOffset ] ) < 0 )
    {
      lChild = lChild + 1;
    }

    if ( CompareMappingLine( (*aLines)[ lRoot + aOffset ], (*aLines)[ lChild + aOffset ] ) < 0 ) 
    {
      SwapLines( aLines, lRoot + aOffset, lChild + aOffset );
      lRoot = lChild;
    }
    else
      return;
  }
}

void MappingsPreparer::WriteAllLines( vector<string> *aMappingsFile, string & aFilename )
{
  ofstream lStream( aFilename.c_str() );

  for each ( string lLine in *aMappingsFile )
  {
    lStream << lLine << NewLine;
  }

  lStream.close();
}