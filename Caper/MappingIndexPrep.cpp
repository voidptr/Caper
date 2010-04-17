
#include "MappingIndexPrep.h"

MappingIndexPrep::MappingIndexPrep( string aSourceMappingFilePath, MappingFileFormat aFormat, string aSavePath, bool aBundle )
{
  mSourceMappingFilePath = aSourceMappingFilePath;
  mSavePath = aSavePath;
  mSourceFileFormat = aFormat;
  mDoBundle = aBundle;

  mMappingFile = new MappingFile( aSourceMappingFilePath, aFormat );
}

void MappingIndexPrep::CreateIndex()
{
  mMappingFile->Init();

  if ( !mMappingFile->IsSorted() )
    mMappingFile->SortMap(); // the flags about whether things are sorted or not are weird.

  GenerateIndexMap();

  if ( mDoBundle )
    WriteBundle();
  else
    WriteSeparateMappingIndexAndFile();

  cout << "DONE!!!" << endl;
}

string MappingIndexPrep::GenerateSavePathFilename(string aFilename, string aPostfix)
{
  Path lFilename(aFilename);
  Path lSavePath(mSavePath);

  string lOutPath;
  
  
  if (!lSavePath.IsDirectory() ) //not a directory
  {
    lOutPath = lSavePath.mPathString + lFilename.Filename() + aPostfix; 
  }
  else // is a directory, so append the file name, plus a postfix.
  {
    lSavePath = lSavePath / lFilename.Filename();

    lOutPath = lSavePath.mPathString + aPostfix; 
  }
  return lOutPath;
}

void MappingIndexPrep::WriteBundle()
{
  // first, compress the mappings, updating the mapping as you go. Then, write out the mapping, and append the content of the compressed tmp file to it. :/ Slow.

  ifstream lSourceMappingFile( mSourceMappingFilePath.c_str(), ios_base::binary );
  if ( !lSourceMappingFile.is_open() )
    throw string("Could not open source mappings file.");

  string lCompressedMappingTmpFilePath = GenerateSavePathFilename( "CompressedMappingFile.tmp" );

  ofstream lOutStream( lCompressedMappingTmpFilePath.c_str(), ios::binary );
  if ( !lOutStream.is_open() )
    throw string("Could not write to temporary compressed mapping file.");


  long long lPos = 0;
  // loop throught the contigs in the file
  for( MappingIndexMap::iterator lIt = mIndex.begin(); lIt != mIndex.end(); ++lIt )
  {
    string lContig = lIt->first;

    // loop through the window info entries for the given contig
    for ( map<long long, MappingWindowBlockInfo>::iterator lWindowIt = lIt->second.begin();
      lWindowIt != lIt->second.end(); ++lWindowIt )
    {
      long long lBlockStartVectorIndex = lWindowIt->second.StartingLineVectorIndex;

      int lHowManyLinesInTheBlock = HowManyLinesInThisBlock( lIt, lWindowIt ); 

      string lBlock;
      ReadBlock( lSourceMappingFile, lContig, lBlockStartVectorIndex, lHowManyLinesInTheBlock, lBlock );

      string lCompressedBlock;
      CompressBlock( lBlock, lCompressedBlock );

      lOutStream.write( lCompressedBlock.c_str(), lCompressedBlock.size() );

      lWindowIt->second.FinalBlockPosition = lPos;
      lWindowIt->second.BlockSizeInBytes = lBlock.size();
      lWindowIt->second.CompressedBlockSizeInBytes = lCompressedBlock.size();

      lPos += lCompressedBlock.size();
    }      
  }

  lSourceMappingFile.close();
  lOutStream.close();

  string lBundleFilePath = GenerateSavePathFilename( Path( mSourceMappingFilePath ).Filename(), ".bundle" );
  WriteIndexFile( lBundleFilePath );
  AppendFile( lBundleFilePath, lCompressedMappingTmpFilePath );
  // TODO, delete the tmp file.

}

int MappingIndexPrep::HowManyLinesInThisBlock( MappingIndexMap::iterator lContigIterator, map<long long, MappingWindowBlockInfo>::iterator lWindowIterator )
{
  map<long long, MappingWindowBlockInfo>::iterator lNextWindow = lWindowIterator;
  ++lNextWindow;

  if ( lNextWindow != lContigIterator->second.end() )
  {
    return lNextWindow->second.StartingLineVectorIndex - lWindowIterator->second.StartingLineVectorIndex; // next window start, minus current window start.
  }
  else // we fell off the end, so we gotta find out how many lines are left.
  {
    int lTotalLinesInThisContig = mMappingFile->find( lContigIterator->first )->second.size();

    return lTotalLinesInThisContig - lWindowIterator->second.StartingLineVectorIndex; // total count, minus where we start.
  }
}


void MappingIndexPrep::ReadBlock( ifstream & aStream, string & aContig, long long aStart, int aCount, string & aBlock )
{
  MappingMap::iterator lVectorPair = mMappingFile->find( aContig );
  
  for ( int lIndex = aStart; lIndex < aStart + aCount; ++lIndex )
  { 
    string lLine;
    aStream.seekg( lVectorPair->second[ lIndex ].Offset );
    getline( aStream, lLine );

    aBlock.append( lLine + NEWLINE );
  }
}


void MappingIndexPrep::CompressBlock( string & aBlock, string & aCompressedBlock )
{
  Bytef *lSource;
  unsigned long long lSourceLength = aBlock.size() + 1;
  lSource = (Bytef *) calloc( lSourceLength, 1 ); // allocate the memory on the fly.
  assert( lSource != NULL ); // temporary. TODO handle this better, clean up after myself.
  strcpy( (char*) lSource, aBlock.c_str() );
  lSource[aBlock.size()] = NULL; // set null termination manually.

  Bytef *lCompressed;
  unsigned long lCompressedLength = compressBound( lSourceLength );
  lCompressed = (Bytef *) calloc( lCompressedLength, 1 );
  assert( lCompressed != NULL ); // temporary. TODO handle this better, clean up after myself.

  int lErrorCode = compress( lCompressed, &lCompressedLength, lSource, lSourceLength );
  assert( lErrorCode == 0 ); // temporary. TODO handle this better, clean up after myself.

  //// TEST CODE
  //Bytef *lUncompressed;
  //unsigned long lUncompressedLength = aBlock.size() + 1; // do these go negative?
  //lUncompressed = new Bytef[ lUncompressedLength ]; // allocate the memory on the fly.
  //assert( lUncompressed != NULL ); // temporary. TODO handle this better, clean up after myself.

  //int lErrorCode2 = uncompress(lUncompressed, &lUncompressedLength, lCompressed, lCompressedLength);
  //assert( lErrorCode2 == 0 ); // temporary. TODO handle this better, clean up after myself.

  //delete [] lUncompressed;
  //// END TEST CODE

  aCompressedBlock.append( (char*) lCompressed, lCompressedLength );

  free( lSource );
  free( lCompressed );  
}


void MappingIndexPrep::WriteIndexFile( string aFilePath )
{
  ofstream lStream( aFilePath.c_str(), ios::binary );
  if ( !lStream.is_open() )
    throw string("Could not write to saved index file.");

  lStream << MappingUtilitiesFactory::ToString( mSourceFileFormat ) << endl;

  // saving the number of contigs
  lStream << mIndex.size() << endl;

  //saving the mapping index
  MappingIndexMap::iterator lMappingIndexIterator;
  for ( lMappingIndexIterator = mIndex.begin(); lMappingIndexIterator != mIndex.end(); ++lMappingIndexIterator ) // loop through the contigs
  {
    //            Contig Name                             Number of Stored Windows
    lStream << lMappingIndexIterator->first << Tab << lMappingIndexIterator->second.size() << endl;

    // loop through the windows
    map<long long, MappingWindowBlockInfo>::iterator lWindowIterator;
    for ( lWindowIterator = lMappingIndexIterator->second.begin(); lWindowIterator != lMappingIndexIterator->second.end(); ++lWindowIterator )
    {
      lStream << lWindowIterator->first << Tab << // window number
        lWindowIterator->second.FinalBlockPosition << Tab << // window offset
        lWindowIterator->second.BlockSizeInBytes << Tab << // block size
        lWindowIterator->second.CompressedBlockSizeInBytes << endl; // stored block size
    }
  }

  lStream.close();
}

void MappingIndexPrep::AppendFile( string aTargetPath, string aSourcePath )
{
  ofstream lOutStream( aTargetPath.c_str(), ios::binary | ios::app );

  if ( !lOutStream.is_open() )
    throw string( "Could not write to output bundle" );

  ifstream lInStream( aSourcePath.c_str(), ios_base::binary );
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

void MappingIndexPrep::WriteSeparateMappingIndexAndFile()
{
}

void MappingIndexPrep::GenerateIndexMap()
{
  for( MappingFile::iterator lContig = mMappingFile->begin(); lContig != mMappingFile->end(); ++lContig )
  {
    long long lTargetWindow = 0;
    for ( int lVectorIndex = 0; lVectorIndex < lContig->second.size(); ++lVectorIndex )
    {
      long long lLineIndex = lContig->second[lVectorIndex].Index;

      if ( lLineIndex >= (lTargetWindow * IndexIncrement) ) // after the start of the window
      {
        if ( lLineIndex >= ((lTargetWindow + 1) * IndexIncrement) ) // overshot. Which one did we end up in? Populate that one.
        {
          lTargetWindow = (lLineIndex / IndexIncrement); // DOES THIS TRUNCATE LIKE I WANT IT TO?
        }
        mIndex.AddEntry( lContig->first, lTargetWindow, lVectorIndex );
        ++lTargetWindow; // move to the next target window.
      }
    }
  }
}

MappingIndexPrep::~MappingIndexPrep(void)
{
  delete mMappingFile;
}
