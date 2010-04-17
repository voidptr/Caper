#pragma once

#include "Path.h"
#include "Typedefs.h"
#include "MappingUtilities.h"

#include "MappingFile.h"
#include "MappingIndexMap.h"

#include <stdio.h>

#include "zlib.h"


class MappingIndexPrep
{
  // Private Method Values
private:
  // static
  static const int IndexIncrement = 1000;
  static const char Tab = '\t';
  static const char NewLine = '\n';

  string mSourceMappingFilePath;
  MappingFileFormat mSourceFileFormat;
  string mSavePath;
  bool mDoBundle;

  MappingFile * mMappingFile;
  MappingIndexMap mIndex;

public:
  MappingIndexPrep( string aSourceMappingFilePath, MappingFileFormat aFormat, string aSavePath, bool aBundle );
  void CreateIndex();
  ~MappingIndexPrep(void);

private:
  void GenerateIndexMap();
  void WriteBundle();
  void WriteSeparateMappingIndexAndFile();
  string GenerateSavePathFilename(string aFilename, string aPostfix="");
  int HowManyLinesInThisBlock( MappingIndexMap::iterator lContigIterator, map<long long, MappingWindowBlockInfo>::iterator lWindowIterator );
  void ReadBlock( ifstream & lStream, string & lContig, long long lStart, int lCount, string & lBlock );
  void CompressBlock( string & aBlock, string & aCompressedBlock );
  void WriteIndexFile( string aFilePath );
  void AppendFile( string aTargetPath, string aSourcePath );

};
