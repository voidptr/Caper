#pragma once

#include "Path.h"
#include "Typedefs.h"
#include "MappingUtilities.h"
#include "MappingUtilitiesFactory.h"
#include "IndexAndOffset.h"

#include "MappingMap.h"
#include "MappingIndex.h"
#include "Line.h"

#include <stdio.h>

#include "zlib.h"

#include <time.h>

class MappingFile
{

private:
  static const int IndexIncrement = 1000;
  static const char Tab = '\t';

  MappingMap mMappingFileRepresentation;
  MappingIndex mMappingIndex;

  string mSavePath;
  string mSourceFilename;
  MappingFileFormat mSourceFormat;

  MappingUtilities * mMappingUtils;

public:
  MappingFile( string aSourceFilename, MappingFileFormat aSourceFormat, string aSavePath )
  {
    mSavePath = aSavePath;
    mSourceFilename = aSourceFilename;
    mSourceFormat = aSourceFormat;

    mMappingUtils = MappingUtilitiesFactory::BuildMappingUtilities( aSourceFormat );
  }

  void ProduceIndex(bool aCompress=false)
  {
    ReadMappingFile();
    GenerateIndex();

    if ( aCompress ) // compress (requires write of mapping file, regardless of sort need or not.)
    {
      string lSavedCompressedFilename = CompressAndWriteMappingFile();
      string lBundleFilename = GenerateSavePathFilename( mSourceFilename, ".bundle" );
      WriteIndexFile( lBundleFilename );
      Bundle( lBundleFilename, lSavedCompressedFilename );
    }
    else if ( mMappingFileRepresentation.MapWasRearrangedBySort() ) // sorted! write the mapping file
    {
      string lSavedSortedFilename = WriteSortedMappingFile();
      string lIndexFilename = GenerateSavePathFilename( lSavedSortedFilename, ".mappingindex" );
      WriteIndexFile( lIndexFilename );
    }
    else // not sorted, so no need to write out the mapping file.
    {
      string lIndexFilename = GenerateSavePathFilename( mSourceFilename, ".mappingindex" );
      WriteIndexFile( lIndexFilename );
    }
  }

  ~MappingFile(void)
  {
    delete mMappingUtils;
  }

private:

  void ReadMappingFile()
  {
 //   time_t lStart = time(NULL);
    ifstream lStream( mSourceFilename.c_str(), ios::binary );

    if ( !lStream.is_open() || lStream.peek() < 0 )
      throw string("Could not open mapping file.");

    string lLine;
    long long lPos = -1;
    while (lStream.peek() > -1 )
    {
      lPos = lStream.tellg(); // could underflow :/
      getline( lStream, lLine );

      int lLength = (long long )lStream.tellg() - lPos;

      string lContig = mMappingUtils->GetContigIdent( lLine );
      long long lIndex = mMappingUtils->GetIndex( lLine );

      //lLine.append( NEWLINE );

      mMappingFileRepresentation.AddLine( lContig, lIndex, lPos, lLength );
    }

    lStream.close();
    mMappingFileRepresentation.Finalize(); // signal that we are done reading. do whatever, incl. sorting.
  }

  void GenerateIndex()
  {
    long long lLastWindowLineCount = 0; // the number of lines in the previous window (counting up)
    long long lLastWindowSize = 0;

    long long lPos = 0;
    MappingIndex::mapped_type::iterator lLastWindowInfo; // previous window pointer
    bool lNotFirst = false;

    for (MappingMap::iterator lContig = mMappingFileRepresentation.begin();
      lContig != mMappingFileRepresentation.end();
      ++lContig)
    {
      long long lTargetWindow = 0;

      int lLinesInThisContig = lContig->second.size();
      for (int lLineVectorIndex = 0; lLineVectorIndex < lLinesInThisContig; ++lLineVectorIndex)
      {
        MappingMap::mapped_type::reference lLine = lContig->second.at(lLineVectorIndex);

        long long lIndex = lLine.Index;
        if ( lIndex >= (lTargetWindow * IndexIncrement) ) // after the start of the window.
        {
          if ( lIndex >= ((lTargetWindow + 1) * IndexIncrement) ) // overshot
            lTargetWindow = (lIndex / IndexIncrement); // reset the window to be the one we found.

          if ( lNotFirst ) // there was a previous window.
          {
            lLastWindowInfo->second.NumberOfLines = lLastWindowLineCount; // apply the count
            lLastWindowInfo->second.BlockSizeInBytes = lLastWindowSize; // apply the length of the block
          }
          else
          {
            lNotFirst = true;
          }
          lLastWindowLineCount = 0; // it's a new window!
          lLastWindowSize = 0;

          mMappingIndex.AddEntry( lContig->first, lTargetWindow, lLineVectorIndex, lPos );

          lLastWindowInfo = mMappingIndex.find( lContig->first )->second.find( lTargetWindow );

          ++lTargetWindow;
        }

        ++lLastWindowLineCount;
        lLastWindowSize += lLine.Length;


        lPos += lLine.Length;
      }
    }
    // the number of lines for the last one.
    if ( lNotFirst ) // there was a previous window.
    {
      lLastWindowInfo->second.NumberOfLines = lLastWindowLineCount; // apply the count
      lLastWindowInfo->second.BlockSizeInBytes = lLastWindowSize; // apply the length of the block
    }
  }


  string WriteSortedMappingFile()
  {
   ifstream lSourceMappingFile( mSourceFilename.c_str(), ios_base::binary );
   if ( !lSourceMappingFile.is_open() )
      throw string("Could not open source mappings file.");

    string lSortedFilename = GenerateSavePathFilename( Path( mSourceFilename ).Filename(), ".sorted" );
    ofstream lOutStream( lSortedFilename.c_str(), ios::binary );
    if ( !lOutStream.is_open() )
      throw string("Could not write to sorted mapping file.");

 //   long long lPos = 0;
    for (MappingMap::iterator lContig = mMappingFileRepresentation.begin();
      lContig != mMappingFileRepresentation.end();
      ++lContig)
    {
      for (MappingMap::mapped_type::iterator lLine = lContig->second.begin();
        lLine != lContig->second.end();
        ++lLine )
      {
        char * lLineContent = new char[ lLine->Length ];
        lSourceMappingFile.seekg( lLine->Offset );
        lSourceMappingFile.read( lLineContent, lLine->Length );
        //getline( lSourceMappingFile, lLineContent );

        // preserve the f'ing line endings
        //int lExtraCharsToRead = (int)lSourceMappingFile.tellg() - lLineContent.length();
        //char * lBuf = new char[ lExtraCharsToRead ];
        //lSourceMappingFile.seekg( (int)lSourceMappingFile.tellg() - lExtraCharsToRead );
        //lSourceMappingFile.read( lBuf, lExtraCharsToRead );
        //lLineContent.append( lBuf, lExtraCharsToRead );

        lOutStream.write( lLineContent, lLine->Length );
        delete lLineContent;
      }
    }
    return lSortedFilename;
  }

  string CompressAndWriteMappingFile()
  {
   ifstream lSourceMappingFile( mSourceFilename.c_str(), ios_base::binary );
   if ( !lSourceMappingFile.is_open() )
      throw string("Could not open source mappings file.");

    string lCompressedFilename = GenerateSavePathFilename( Path( mSourceFilename ).Filename(), ".compressed" );
    ofstream lOutStream( lCompressedFilename.c_str(), ios::binary );
    if ( !lOutStream.is_open() )
      throw string("Could not write to compressed mapping file.");

    long long lPos = 0;
    // loop throught the contigs in the file
    for( MappingIndex::iterator lContig = mMappingIndex.begin();
      lContig != mMappingIndex.end(); ++lContig )
    {
      string lContigName = lContig->first;

      // loop through the window info entries for the given contig
      for ( MappingIndex::mapped_type::iterator lWindow = lContig->second.begin();
        lWindow != lContig->second.end(); ++lWindow )
      {
        string lBlock;
        ReadBlock( lSourceMappingFile,
          lContigName,
          lWindow->second.SortedContigStartingLineNumber,
          lWindow->second.NumberOfLines, lBlock );

        string lCompressedBlock;
        CompressBlock( lBlock, lCompressedBlock );

        lOutStream.write( lCompressedBlock.c_str(), lCompressedBlock.size() );

        lWindow->second.BlockOffset = lPos; // update w/ compressed value
        lWindow->second.CompressedBlockSizeInBytes = lCompressedBlock.size();

        lPos += lCompressedBlock.size();
      }
    }

    lSourceMappingFile.close();
    lOutStream.close();

    return lCompressedFilename;
  }

  void WriteIndexFile( string aFilePath )
  {
    ofstream lStream( aFilePath.c_str(), ios::binary );
    if ( !lStream.is_open() )
      throw string("Could not write to saved index file.");

    lStream << MappingUtilitiesFactory::ToString( mSourceFormat ) << endl;

    // saving the number of contigs
    lStream << mMappingIndex.size() << endl;

    //saving the mapping index
    MappingIndex::iterator lMappingIndexIterator;
    for ( lMappingIndexIterator = mMappingIndex.begin();
      lMappingIndexIterator != mMappingIndex.end(); ++lMappingIndexIterator ) // loop through the contigs
    {
      //            Contig Name                             Number of Stored Windows
      lStream << lMappingIndexIterator->first << Tab << lMappingIndexIterator->second.size() << endl;

      // loop through the windows
      map<long long, MappingWindowBlockInfo>::iterator lWindowIterator;
      for ( lWindowIterator = lMappingIndexIterator->second.begin(); lWindowIterator != lMappingIndexIterator->second.end(); ++lWindowIterator )
      {
        lStream << lWindowIterator->first << Tab << // window number
          lWindowIterator->second.BlockOffset << Tab << // window offset
          lWindowIterator->second.BlockSizeInBytes << Tab << // block size
          lWindowIterator->second.CompressedBlockSizeInBytes << endl; // stored block size
      }
    }

    lStream.close();
  }

  void Bundle( string aTargetPath, string aSourcePath )
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

    // TODO, delete the tmp compressed file.
  }

  string GenerateSavePathFilename(string aFilename, string aPostfix)
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

  void ReadBlock( ifstream & aStream, string & aContig, long long aStart, int aCount, string & aBlock )
  {
    MappingMap::iterator lVectorPair = mMappingFileRepresentation.find( aContig );

    for ( int lIndex = aStart; lIndex < aStart + aCount; ++lIndex )
    {
      MappingMap::mapped_type::reference lLine(lVectorPair->second.at(lIndex));

      char * lLineContent = new char[ lLine.Length ];
      aStream.seekg( lLine.Offset );
      aStream.read( lLineContent, lLine.Length );

      aBlock.append( lLineContent );

      delete lLineContent;
    }
  }

  void CompressBlock( string & aBlock, string & aCompressedBlock )
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
};
