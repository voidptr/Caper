#pragma once

#include "MappingCache.h"
#include "Mapping.h"
#include "MappingUtilities.h"
#include "MappingUtilitiesFactory.h"
#include "Typedefs.h"

#include "zlib.h"

class MappingEngine
{
  static const int IndexIncrement = 1000;
  static const char Tab = '\t';
  static const char NewLine = '\n';

private:
	Sequences * ReferenceGenome;

  MappingCache* CacheA;
  MappingCache* CacheB;
	
	long mEndOfFilePosition;
	string mPath;
  long mPathStartOffset;
  string mIndexPath;
  bool mBundle;
  int ReadLength;
	

	map<string, vector<StoredMappingBlock> > mMappingIndexes;
	map<string, pair<long,long> > mContigBorders; // do we even need this?

  MappingUtilities * mMappingUtilities;

private:
	void PopulateMappingIndex();	
	void PopulateContigBorders();
	void PopulateNumberOfWindows();
	void PopulateReadInformation();
  int DetermineCompressedMappingStartOffset();
  
	MappingCache * GetCorrectCache( string lContigIdent, int aLeft, int aRight );
	void RebuildCaches( string lContigIdent, int aLeft );
	MappingCache * RebuildCache( string aContigIdent, int aLeft );
	MappingCache * BuildEmptyCache( string aContigIdent, int aLeft, int aRight );
	MappingCache * BuildCache( char * aBlock, string aContigIdent, int aLeft, int aRight );

  void OffsetSeek( ifstream & aStream, long aPosition );

  void DecompressBlock( char *&lBlock, int aStoredSize, int aBlockSize );
  char * FetchBlock( string aContigIdent, int aStartingWindowIndex );

public:
	map<string, int> NumberOfReads;
  
public:
	//MappingEngine( string aPath, Sequences & aReferenceGenome );
	MappingEngine( string aPath, string aIndexPath );
  MappingEngine( string aBundlePath );
  Mappings * GetReads(string lContigIdent, int aLeft, int aRight );
  int GetReadLength();

  void Initialize();
  //void Initialize( string aIndexPath );
  //void SaveMappingIndex( string aSavePath );
};
