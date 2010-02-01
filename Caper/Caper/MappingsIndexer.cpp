#include "MappingsIndexer.h"

MappingsIndexer::MappingsIndexer(string aMappingsFilePath, MappingFileFormat aFormat, string aSavePath, Sequences * aReferenceGenome)
{
	ReferenceGenome = aReferenceGenome;
	mMappingsPath = aMappingsFilePath;
  mSavePath = aSavePath;
  mFormat = aFormat;
  
}

void MappingsIndexer::IndexMappingsAndSave()
{		
  MappingUtilities = MappingUtilitiesFactory::BuildMappingUtilities( mFormat );

  MappingsPreparer * lPreparer = new MappingsPreparer( mMappingsPath, mSavePath, mFormat );
  mMappingsPath = lPreparer->PrepareMappings(); // the new path (or old path, if no prep was required).

  PopulateReadInformation();
  PopulateNumberOfWindows();
  PopulateContigBorders();  
  cout << " Indexing Mappings... ";
  cout.flush();
  PopulateMappingIndex();  
  cout << "Done!" << endl;

  SaveMappingIndex();
}

void MappingsIndexer::SaveMappingIndex()
{
  boost::filesystem::path lPath(mMappingsPath);
  boost::filesystem::path lSavePath(mSavePath);

  string lOutputFilename; 
  if (!(boost::filesystem::is_directory( lSavePath )) ) //not a directory
  {
    lOutputFilename = lSavePath.file_string() + lPath.filename() + ".mappingindex"; // TODO, make this be user definable.
  }
  else
  {
    boost::filesystem::path lIndexPath( lSavePath );
    lIndexPath /= lPath.filename();

    lOutputFilename = lIndexPath.file_string() + ".mappingindex";
  }

  ofstream lStream( lOutputFilename.c_str(), ios::binary );
  if ( !lStream.is_open() )
    throw string("Could not write to saved index file.");

  lStream << MappingUtilitiesFactory::ToString( mFormat ) << endl;

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

void MappingsIndexer::PopulateContigBorders()
{		
  ifstream lStream( mMappingsPath.c_str(), ios_base::binary );
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
    string lCurrentContig = MappingUtilities->GetContigIdent( lLine );
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

void MappingsIndexer::PopulateNumberOfWindows()
{		
  Sequences::iterator lIterator;
  for(lIterator = ReferenceGenome->begin() ; lIterator != ReferenceGenome->end(); lIterator++ )
	{
		mNumberOfWindows.insert( pair<string, int>( lIterator->first,
      (int) ceil( (double) lIterator->second->Length / IndexIncrement ) ) );
	}	
}

void MappingsIndexer::PopulateReadInformation()
{	
  ifstream lStream( mMappingsPath.c_str(), ios_base::binary );
  if ( !lStream.is_open() )
    throw string("Could not open mappings file.");

  string lLine;
  getline( lStream, lLine );

  ReadLength = MappingUtilities->GetSequence( lLine ).length();
  
  lStream.seekg( 0, ios::end );
  mEndOfFilePosition = lStream.tellg();

  lStream.close();
}

void MappingsIndexer::PopulateMappingIndex()
{		
  MappingUtilities = MappingUtilitiesFactory::BuildMappingUtilities( mFormat );

  ifstream lStream( mMappingsPath.c_str(), ios::binary );
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
    string lCurrentContig = MappingUtilities->GetContigIdent( lLine );
    int lCurrentIndex = MappingUtilities->GetIndex( lLine );

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
      mMappingIndexes[ lContig ].reserve( mNumberOfWindows[ lContig ] );
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

  // fill in -1s for the empty sections that may be at the end.
  for (int i = mMappingIndexes[ lContig ].size(); i < mNumberOfWindows[ lContig ]; ++i )
  {
    mMappingIndexes[ lContig ].push_back( -1 ); // empty sections at the tail end.
  }
}
