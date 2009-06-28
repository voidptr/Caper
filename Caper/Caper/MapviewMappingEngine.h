#pragma once

#include "stdafx.h"
#include "Sequence.h"
#include "MappingCache.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>

using namespace std;

class Mappings;
class Sequence;
class MappingCache;
class MapviewMappingEngine;

class MapviewMappingEngine
{
private:
  string mPath;
  Sequence* mReferenceGenome;

  static const int IndexIncrement = 1000;
  static const int StartingJumpIncrement = 10000;
  static const char Tab = '\t';

  int mLineLengthIsh;
  long mEndOfFilePos;
  int mNumberOfWindows;

  MappingCache* CacheA;
  MappingCache* CacheB;

  ifstream mStream;
  long* mFilePositions;

  enum Direction
  {
    JumpForward,
    JumpBackward
  };

public:
  MapviewMappingEngine(char* aPath, Sequence* aSequence);
  vector<Mapping*> GetReads( int aLeft, int aRight );
  
  ~MapviewMappingEngine(void);
  bool Initialize();
private:
  void InitFileInformation();
  long FindNextIndexStart( int aTarget, int aIncrement, Direction aDirection );
  void SafeSeek( long aTargetSeek );
  
  void RebuildCaches( int aLeft );
  MappingCache* RebuildCache( int lStartingIndex );
  MappingCache* GetCorrectCache( int aLeft, int aRight );
public:
  static int GetNormalizedIndex( string aValue );
  static int GetNormalizedIndex( int aValue );
  static int GetIndex( string aLine );
  static void DestroyMappings( vector<Mapping*> aMappings );
};
