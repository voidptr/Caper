#pragma once

#include "Path.h"

#include "MappingsPreparer.h"
#include "MappingUtilities.h"

#include "Typedefs.h"

#include <stdio.h>

#include "zlib.h"

class MappingsIndexer
{
  static const int IndexIncrement = 1000;
  static const char Tab = '\t';
  static const char NewLine = '\n';

private:  
  string MappingIndexFile;
  string TemporaryCompressedMappingFile;
  string TemporaryMappingFile;
  string BundleFile;

	long mEndOfFilePosition;
	string mMappingsPath;
  string mSavePath;
  MappingFileFormat mFormat;
  bool mBundle;
  string mOutputTmpMappingFilePath;
  string mOutputIndexFilePath;
  string mOutputBundleFilePath;
  bool mSorted;

  MappingUtilities * mMappingUtilities;

  map<string, vector<StoredMappingBlock> > mMappingIndexes; // a map of contigs, containing vectors of positions and sizes of index interval windows.
	map<string, pair<long,long> > mContigBorders;

private:
  void DetermineFileNames();
	void PopulateMappingIndex();	
	void PopulateContigBorders();
	void CompressFileAndIndex();
	void PopulateReadInformation();
  void SaveMappingIndex();
  void AssembleBundle();

protected:
  MappingsPreparer* ConstructPreparer( string aMappingsPath, string aSavePath );

public:
	map<string, int> NumberOfReads;
  int ReadLength;
	
public:
	//MappingEngine( string aPath, Sequences & aReferenceGenome );
	MappingsIndexer( string aMappingFilePath, MappingFileFormat aFormat, string aSavePath, bool aBundle );
  
  void IndexMappingsAndSave();
};
