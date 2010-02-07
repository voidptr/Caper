#include "MappingsIndexer.h"

MappingsIndexer::MappingsIndexer(string aMappingsFilePath, MappingFileFormat aFormat, string aSavePath)
{
	mMappingsPath = aMappingsFilePath;
  mSavePath = aSavePath;
  mFormat = aFormat;
  
}

void MappingsIndexer::IndexMappingsAndSave()
{		
  mMappingUtilities = MappingUtilitiesFactory::BuildMappingUtilities( mFormat );

  MappingsPreparer * lPreparer = new MappingsPreparer( mMappingsPath, mSavePath, mFormat );
  mMappingsPath = lPreparer->PrepareMappings(); // the new path (or old path, if no prep was required).

  PopulateReadInformation();
  PopulateContigBorders();  
  cout << " Indexing Mappings... ";
  cout.flush();
  PopulateMappingIndex();  
  cout << "Done!" << endl;

  CompressFileAndIndex();
  SaveMappingIndex();
}
void MappingsIndexer::CompressFileAndIndex() // do the compression separately, in the tmp file, and then write it out.
{
}


void MappingsIndexer::SaveMappingIndex() // do bundling here.
{
  Path lPath(mMappingsPath);
  Path lSavePath(mSavePath);

  string lOutputFilename; 
  if (!lSavePath.IsDirectory() ) //not a directory
  {
    lOutputFilename = lSavePath.mPathString + lPath.Filename() + ".mappingindex"; // TODO, make this be user definable.
  }
  else
  {
    Path lIndexPath( lSavePath );
        lIndexPath = lIndexPath / lPath.Filename();

    lOutputFilename = lIndexPath.mPathString + ".mappingindex";
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
    string lCurrentContig = mMappingUtilities->GetContigIdent( lLine );
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

void MappingsIndexer::PopulateReadInformation()
{	
  ifstream lStream( mMappingsPath.c_str(), ios_base::binary );
  if ( !lStream.is_open() )
    throw string("Could not open mappings file.");

  string lLine;
  getline( lStream, lLine );

  ReadLength = mMappingUtilities->GetSequence( lLine ).length();
  
  lStream.seekg( 0, ios::end );
  mEndOfFilePosition = lStream.tellg();

  lStream.close();
}

void MappingsIndexer::PopulateMappingIndex()
{		
  mMappingUtilities = MappingUtilitiesFactory::BuildMappingUtilities( mFormat );

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
    string lCurrentContig = mMappingUtilities->GetContigIdent( lLine );
    int lCurrentIndex = mMappingUtilities->GetIndex( lLine );

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

  // we have no knowledge of what comes after the last one. Doesn't matter.
}
