#pragma once

#include "MappingCache.h"
#include "Mapping.h"
#include "MappingUtilities.h"
#include "MappingUtilitiesFactory.h"
#include "StoredMappingIntervalBlock.h"
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
	
	long long mEndOfFilePosition;
	string mPath;
  long long mPathStartOffset;
  string mIndexPath;
  bool mBundle;
  int ReadLength;
	

	map<string, vector<StoredMappingIntervalBlock> > mMappingIndexes;
//	map<string, pair<long long,long long> > mContigBorders; // do we even need this?

  MappingUtilities * mMappingUtilities;

private:
	void PopulateMappingIndex();	
//	void PopulateContigBorders();
	void PopulateNumberOfWindows();
	void PopulateReadInformation();
  int DetermineCompressedMappingStartOffset();
  
	MappingCache * GetCorrectCache( string lContigIdent, long long aLeft, long long aRight );
	void RebuildCaches( string lContigIdent, long long aLeft );
	MappingCache * RebuildCache( string aContigIdent, long long aLeft );
	MappingCache * BuildEmptyCache( string aContigIdent, long long aLeft, long long aRight );
	MappingCache * BuildCache( char * aBlock, string aContigIdent, long long aLeft, long long aRight );

  void OffsetSeek( ifstream & aStream, long long aPosition );

  void DecompressBlock( char *&lBlock, long long aStoredSize, long long aBlockSize );
  char * FetchBlock( string aContigIdent, long long aStartingWindowIndex );

public:
	map<string, int> NumberOfReads;
  
public:
	//MappingEngine( string aPath, Sequences & aReferenceGenome );
	MappingEngine( string aPath, string aIndexPath );
  MappingEngine( char * aPath, char * aIndexPath );
  MappingEngine( string aBundlePath );
  MappingEngine( char * aBundlePath );
  Mappings * GetReads(string lContigIdent, long long aLeft, long long aRight );
  int GetReadLength();

  void Initialize();
  //void Initialize( string aIndexPath );
  //void SaveMappingIndex( string aSavePath );
};
