#pragma once

#include "Path.h"
#include "Typedefs.h"
#include "MappingUtilities.h"
#include "MappingUtilitiesFactory.h"

#include "MappingMap.h"
#include "Line.h"

#include <stdio.h>

#include "zlib.h"

class MappingFile : public MappingMap
{

private:
  string mSourceFilename;
  MappingFileFormat mSourceFormat;

  bool mIsSourceFileSorted;

  MappingUtilities * mMappingUtils;

public:

  MappingFile( string aSourceFilename, MappingFileFormat aSourceFormat )
  {
    mIsSourceFileSorted = false;

    mSourceFilename = aSourceFilename;
    mSourceFormat = aSourceFormat;

    mMappingUtils = MappingUtilitiesFactory::BuildMappingUtilities( aSourceFormat );
  }

  void Init()
  {
    ReadMappingFile();
  }

  ~MappingFile(void)
  {
    delete mMappingUtils;
  }

private:

  void ReadMappingFile()
  {
    ifstream lStream( mSourceFilename.c_str(), ios::binary );

    if ( !lStream.is_open() || lStream.peek() < 0 )
      throw string("Could not open mapping file.");

    string lLine;
    long long lPos = -1;
    while (lStream.peek() > -1 )
    {      
      lPos = lStream.tellg(); // could underflow :/
      getline( lStream, lLine );

      string lContig = mMappingUtils->GetContigIdent( lLine );
      long long lIndex = mMappingUtils->GetIndex( lLine );

      AddLine( lContig, lIndex, lPos );
    }

    lStream.close();

    mIsSourceFileSorted = IsSorted(); // was the file sorted when it was read in?
  }
};
