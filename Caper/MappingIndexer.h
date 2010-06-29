#pragma once

#include "MappingFile.h"
#include "Typedefs.h"
#include "stdafx.h"

class MappingIndexer
{
private:
  string mSourceMappingFilePath;
  MappingFileFormat mSourceFileFormat;
  string mSavePath;
  bool mDoBundle;

  MappingFile * mMappingFile;

public:

  MappingIndexer( string aSourceMappingFilePath, 
    MappingFileFormat aFormat, 
    string aSavePath, 
    bool aBundle )
  {
    mSourceMappingFilePath = aSourceMappingFilePath;
    mSavePath = aSavePath;
    mSourceFileFormat = aFormat;
    mDoBundle = aBundle;

    mMappingFile = new MappingFile( aSourceMappingFilePath, aFormat, aSavePath );
  }

  void CreateIndex()
  {
    mMappingFile->ProduceIndex( mDoBundle );
  }

  ~MappingIndexer(void)
  {
    delete mMappingFile;
  }
};
