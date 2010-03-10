#include "MappingsIndexer.h"

MappingsIndexer::MappingsIndexer(string aMappingsFilePath, MappingFileFormat aFormat, string aSavePath, bool aBundle)
{

  MappingIndexFile = ".mappingindex";
  TemporaryCompressedMappingFile = ".tmp_compressedmappings";
  TemporaryMappingFile = ".mappings";
  BundleFile = ".bundle";

	mMappingsPath = aMappingsFilePath;
  mSavePath = aSavePath;
  mFormat = aFormat;
  mBundle = aBundle;
}

void MappingsIndexer::IndexMappingsAndSave()
{		
  DetermineFileNames();

  mMappingUtilities = MappingUtilitiesFactory::BuildMappingUtilities( mFormat );

  MappingsPreparer * lPreparer = new MappingsPreparer( mMappingsPath, mSavePath, mFormat );
  string lNewPath = lPreparer->PrepareMappings(); // the new path (or old path, if no prep was required).

  if ( mMappingsPath != lNewPath ) // we did a sort.
  {
    mMappingsPath = lNewPath;
    mSorted = true;
  }

  PopulateReadInformation();
  PopulateContigBorders();  
  cout << " Indexing Mappings... ";
  cout.flush();
  PopulateMappingIndex();  
  cout << "Done!" << endl;

  if ( mBundle )
  {
    CompressFileAndIndex();
    SaveMappingIndex(); // save to a temp file
    AssembleBundle(); // build the bundle and delete the tmp files
  }
  else
    SaveMappingIndex(); 
}

void MappingsIndexer::DetermineFileNames()
{
  Path lMappingPath(mMappingsPath);
  Path lSavePath(mSavePath);

  // set mapping and index file output path
  if (!lSavePath.IsDirectory() ) //not a directory
  {
    mOutputTmpMappingFilePath = lSavePath.mPathString + lMappingPath.Filename() + TemporaryCompressedMappingFile; // TODO, make this be user definable.
    mOutputIndexFilePath = lSavePath.mPathString + lMappingPath.Filename() + MappingIndexFile; // TODO, make this be user definable.
    mOutputBundleFilePath = lSavePath.mPathString + lMappingPath.Filename() + BundleFile; // TODO, make this be user definable.
  }
  else // is a directory, so append the mapping file name, plus the various terminations.
  {
    lSavePath = lSavePath / lMappingPath.Filename();

    mOutputTmpMappingFilePath = lSavePath.mPathString + TemporaryCompressedMappingFile; 
    mOutputIndexFilePath = lSavePath.mPathString + MappingIndexFile;
    mOutputBundleFilePath = lSavePath.mPathString + BundleFile; 
  }
}


void MappingsIndexer::CompressFileAndIndex() // do the compression separately, in the tmp file, and then write it out.
{
  ifstream lInStream( mMappingsPath.c_str(), ios_base::binary );
  if ( !lInStream.is_open() )
    throw string("Could not open mappings file.");

  ofstream lOutStream( mOutputTmpMappingFilePath.c_str(), ios::binary );
  if ( !lOutStream.is_open() )
    throw string("Could not write to temporary compressed mapping file.");

  unsigned long lPreviousEnd = -1;
  map<string, vector<StoredMappingBlock> >::iterator lContigsIterator; // TODO make these iterators be typedefed where the class is typed.
  for ( lContigsIterator = mMappingIndexes.begin(); 
    lContigsIterator != mMappingIndexes.end(); lContigsIterator++ )
  {
    vector<StoredMappingBlock>::iterator lSectionIterator; // TODO, see note above re: iterator typedef
    for ( lSectionIterator = lContigsIterator->second.begin();
      lSectionIterator != lContigsIterator->second.end(); lSectionIterator++ )
    {

      if ( lSectionIterator->first != -1 ) // nothing in this slot.
      {
        long lSize = lSectionIterator->second; // the incoming block size

        lInStream.seekg( lSectionIterator->first, std::ios_base::beg );

        // TODO abstract this out.
        Bytef *lSource;
        unsigned long lSourceLength = lSize;
        lSource = (Bytef *) malloc( lSourceLength ); // allocate the memory on the fly.
        assert( lSource != NULL ); // temporary. TODO handle this better, clean up after myself.

        Bytef *lCompressed;
        unsigned long lCompressedLength = compressBound( lSourceLength );
        lCompressed = (Bytef *) calloc( lCompressedLength, 1 );
        assert( lCompressed != NULL ); // temporary. TODO handle this better, clean up after myself.
        
        lInStream.read( (char*) lSource, lSourceLength );

        int lErrorCode = compress( lCompressed, &lCompressedLength, lSource, lSourceLength );
        assert( lErrorCode == 0 ); // temporary. TODO handle this better, clean up after myself.

        // now that we are done compressing, we can update the index of the compressed location start, based on what we did last time around.
        // i.e., assign the new start position of the current section based on our previous ending
        lSectionIterator->first = lPreviousEnd + 1;
        // second (block size) remains the same.
        lSectionIterator->third = lCompressedLength; // update to be the compressed length.

        // set the end value to account for what we did in this loop iteration.
        lPreviousEnd = lPreviousEnd + lCompressedLength;

        lOutStream.write( (char*) lCompressed, lCompressedLength );

        free( lSource ); // free the memory.
        free( lCompressed ); // free the memory.
        //free( lUncompressed ); // free the memory.
      }
    }
    // update the contig borders to represent to compressed contig.
    mContigBorders[ lContigsIterator->first ].first = lContigsIterator->second.front().first; // first location
    mContigBorders[ lContigsIterator->first ].second = lContigsIterator->second.back().first + lContigsIterator->second.back().second; // last location + size
  }

  lOutStream.close();
  lInStream.close();
}

// put together the compressed mappings with the index into a bundle.
void MappingsIndexer::AssembleBundle()
{
  ofstream lOutStream( mOutputIndexFilePath.c_str(), ios::binary | ios::app );

  if ( !lOutStream.is_open() )
    throw string( "Could not write to output bundle" );

  ifstream lInStream( mOutputTmpMappingFilePath.c_str(), ios_base::binary );
  if ( !lInStream.is_open() )
    throw string("Could not open compressed mappings temporary file.");

  char * lBuffer = new char [256];

  while( lInStream.peek() > -1 )
  {
    lInStream.read( lBuffer, 256 );
    lOutStream.write( lBuffer, lInStream.gcount() );
  }

  delete lBuffer;

  lInStream.close();
  lOutStream.close();
}


void MappingsIndexer::SaveMappingIndex() 
{
  ofstream lStream( mOutputIndexFilePath.c_str(), ios::binary );
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
  map<string, vector<StoredMappingBlock> >::iterator lMappingIndexIterator;
  for ( lMappingIndexIterator = mMappingIndexes.begin(); lMappingIndexIterator != mMappingIndexes.end(); lMappingIndexIterator++ ) // loop through the contigs
  {
    lStream << lMappingIndexIterator->first << Tab << NumberOfReads[ lMappingIndexIterator->first ] << Tab << lMappingIndexIterator->second.size() << endl;

    // loop through the index interval windows
    vector<StoredMappingBlock>::iterator lIndexIterator;
    for ( lIndexIterator = mMappingIndexes[ lMappingIndexIterator->first ].begin(); lIndexIterator != mMappingIndexes[ lMappingIndexIterator->first ].end(); lIndexIterator++ )
    {
      lStream << lIndexIterator->first << Tab << lIndexIterator->second << Tab << lIndexIterator->third << endl; // save the interval start, the block size, and the stored size
    }
  }

  lStream.close();
}

// is this something we even need?
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
  string lLastInsertedIntervalContig = "";
  int lLastInsertedIntervalVectorIndex = -1;


  while ( lStream.peek() > -1 )
  {
    string lLine = "";
    lCurrentPosition = lStream.tellg(); 
    getline( lStream, lLine );    
    string lCurrentContig = mMappingUtilities->GetContigIdent( lLine );
    int lCurrentIndex = mMappingUtilities->GetIndex( lLine );

    if ( lCurrentContig != lContig ) // different (or possibly first!) contig.
    {
      if ( lContig.length() > 0 ) // there was a previous one, so close it up
      {
        NumberOfReads.insert( pair<string, int>( lContig, lReadCount ) ); 
        lReadCount = 0;
      }

      // start the new contig.
      lTargetIndex = 0;
      lContig = lCurrentContig;
      mMappingIndexes.insert( pair<string, vector<StoredMappingBlock> >( lContig, vector<StoredMappingBlock>() ) );
    }

    // reading along, we passed the target index.
    if ( lCurrentIndex >= lTargetIndex )
    {
      if ( lCurrentIndex >= lTargetIndex + IndexIncrement ) // overshot
      {
        mMappingIndexes[ lContig ].push_back( StoredMappingBlock(-1,-1,-1) ); // indicating that this section is empty.
        lStream.seekg( lCurrentPosition ); // unconsume the line, since we'll need it soon.
      }
      else // found the start of the target index window.
      {
        // TODO, don't insert this until we know the proper content (jsut a few lines down)
        mMappingIndexes[ lContig ].push_back( StoredMappingBlock(lCurrentPosition,-1,-1) ); // temporary placeholder for the size until we know it.

        // we only do this if we found an actual, honest to goodness, interval.
        // assign a size to the previous interval, now that we know what it is. (this will happen even if we just crossed a contig boundary)
        if ( lLastInsertedIntervalContig.length() > 0 && lLastInsertedIntervalVectorIndex > -1 ) // check that there was one. 
        {
          // TODO, put this stuff in the StoredMappingBlock editor thing.
          mMappingIndexes[ lLastInsertedIntervalContig ][ lLastInsertedIntervalVectorIndex ].second = 
            lCurrentPosition - mMappingIndexes[ lLastInsertedIntervalContig ][ lLastInsertedIntervalVectorIndex ].first; // dunno if I need to do a -1 here or not. we'll see. TODO
          mMappingIndexes[ lLastInsertedIntervalContig ][ lLastInsertedIntervalVectorIndex ].third = // stored size is the same as block size
            mMappingIndexes[ lLastInsertedIntervalContig ][ lLastInsertedIntervalVectorIndex ].second;
        }

        lLastInsertedIntervalContig = lContig;
        lLastInsertedIntervalVectorIndex = mMappingIndexes[ lContig ].size() - 1; // latest one
      }

      lTargetIndex += IndexIncrement;
    }
    lReadCount++;
  }
  // since it's all over, assign the final size value.
  if ( lLastInsertedIntervalContig.length() > 0 && lLastInsertedIntervalVectorIndex > -1 ) // check that there was one. 
  {
    mMappingIndexes[ lLastInsertedIntervalContig ][ lLastInsertedIntervalVectorIndex ].second = // blocksize
      lCurrentPosition - mMappingIndexes[ lLastInsertedIntervalContig ][ lLastInsertedIntervalVectorIndex ].first; // TODO, do I need to do a -1 here? probably. 
    mMappingIndexes[ lLastInsertedIntervalContig ][ lLastInsertedIntervalVectorIndex ].third = // stored size is the same as block size
      mMappingIndexes[ lLastInsertedIntervalContig ][ lLastInsertedIntervalVectorIndex ].second;
  }

  NumberOfReads.insert( pair<string, int>( lContig, lReadCount ) ); // close up the last contig read count

  lStream.close();

  // we have no knowledge of what comes after the last one. Doesn't matter.
}
