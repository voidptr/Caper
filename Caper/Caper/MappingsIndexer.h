#pragma once

#include "../BoostLibraries/boost/filesystem.hpp"

#include "MappingsPreparer.h"
#include "MappingUtilities.h"

#include "Typedefs.h"

class MappingsIndexer
{
  static const int IndexIncrement = 1000;
  static const char Tab = '\t';
  static const char NewLine = '\n';

private:  
	long mEndOfFilePosition;
	string mMappingsPath;
  string mSavePath;
  MappingFileFormat mFormat;

  MappingUtilities * MappingUtilities;

	map<string, vector<long> > mMappingIndexes;
	//map<string, int> mNumberOfWindows;
	map<string, pair<long,long> > mContigBorders;

private:
	void PopulateMappingIndex();	
	void PopulateContigBorders();
	//void PopulateNumberOfWindows();
	void PopulateReadInformation();
  void SaveMappingIndex();

protected:
  MappingsPreparer* ConstructPreparer( string aMappingsPath, string aSavePath );

public:
	map<string, int> NumberOfReads;
  int ReadLength;
	
public:
	//MappingEngine( string aPath, Sequences & aReferenceGenome );
	MappingsIndexer( string aMappingFilePath, MappingFileFormat aFormat, string aSavePath );
  
  void IndexMappingsAndSave();
};
