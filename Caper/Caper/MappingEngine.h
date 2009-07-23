#pragma once

#include "stdafx.h"
#include "Sequence.h"
#include "Sequences.h"
#include "MappingCache.h"
#include "Pair.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <map>
#include <list>

using namespace std;

class Mapping;
class Sequence;
class MappingCache;
class MapviewMappingEngine;

class MappingEngine
{
private:
	Sequences ReferenceGenome;

	int mReadLength;
	long mEndOfFilePosition;

	map<string, list<long>> mFilePositions;
	map<string, int> mNumberOfWindows;
	map<string, Pair<long>> mContigBorders;
	map<string, int> mSortedContigIdents;	

	//void Initialize();
	//void PopulateFilePositions();	
	//void PopulateContigBorders();
	//void PopulateNumberOfWindows();
	//void PopulateReadInformation();
	//void GetReads( string lContigIdent, int aLeft, int aRight, list<Mapping> & aMappings );
	//MappingCache* GetCorrectCache( string lContigIdent, int aLeft, int aRight );
	//void RebuildCaches( string lContigIdent, int aLeft );
	//MappingCache* RebuildCache( string aContigIdent, int aLeft );
	//MappingCache* BuildEmptyCache( string aContigIdent, int aLeft );
	//MappingCache* BuildCache( char[] aBlock, string aContigIdent, int aLeft, int aRight );
	//string[] GetLinePieces( string aLine );
	//virtual string GetSequence( string aLine ) = 0;
	//virtual string GetIndex( string aLine ) = 0;
	//virtual string GetContigIdent( string aLine ) = 0;

public:
	map<string, int> NumberOfReads;
	
	//MappingEngine( string aPath, Sequences aReferenceGenome );
	//MappingEngine( string aPath );
	~MappingEngine(void);
};
