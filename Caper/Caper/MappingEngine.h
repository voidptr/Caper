#pragma once

#include "stdafx.h"
#include "Sequence.h"
#include "Sequences.h"
#include "MappingCache.h"

class MappingEngine
{
private:
  static const int IndexIncrement = 1000;

	Sequences * ReferenceGenome;

  MappingCache* CacheA;
  MappingCache* CacheB;

	int mReadLength;
	long mEndOfFilePosition;
	string mPath;

	map<string, vector<long>> mMappingIndexes;
	map<string, int> mNumberOfWindows;
	map<string, pair<long,long>> mContigBorders;
	vector<string> mSortedContigIdents;	

	void PopulateMappingIndex();	
	void PopulateContigBorders();
	void PopulateNumberOfWindows();
	void PopulateSortedContigIdents();
	void PopulateReadInformation();
  vector<Mapping> * GetReads(string lContigIdent, int aLeft, int aRight );
	MappingCache * GetCorrectCache( string lContigIdent, int aLeft, int aRight );
	void RebuildCaches( string lContigIdent, int aLeft );
	MappingCache * RebuildCache( string aContigIdent, int aLeft );
	MappingCache * BuildEmptyCache( string aContigIdent, int aLeft, int aRight );
	MappingCache * BuildCache( char * aBlock, string aContigIdent, int aLeft, int aRight );

	virtual string GetSequence( string & aLine ) = 0;
	virtual int GetIndex( string & aLine ) = 0;
	virtual string GetContigIdent( string & aLine ) = 0;

public:
	map<string, int> NumberOfReads;
	
	MappingEngine( string & aPath, Sequences & aReferenceGenome );

  void Initialize();
};
