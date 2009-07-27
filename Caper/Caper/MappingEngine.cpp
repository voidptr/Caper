#include "MappingEngine.h"

MappingEngine::MappingEngine(string & aPath, Sequences & aReferenceGenome)
{
	ReferenceGenome = &aReferenceGenome;
	mPath = aPath;
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

	mSortedContigIdents.sort(); // don't know if this matters.
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
      mMappingIndexes.insert( pair<string, list<long>>( lContig, list<long>() ) );
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

