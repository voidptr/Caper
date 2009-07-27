#pragma once
#include "stdafx.h"

class MappingsPreparer
{
private:
  static const char Tab = '\t';
  static const char NewLine = '\n';

  string mPath;

private:
  bool IsSorted();
  vector<string> * ReadAllLines();
  void WriteAllLines( vector<string>* aMappingsFile, string & aFilename );
  string SortMappingsAndWriteToTmpFile();

  void Introsort( vector<string> * aLines, int aFirstIndex, int aLastIndex, int aCurrentDepth, int aSwitchDepth );
  int Partition( vector<string> * aLines, int aFirstIndex, int aLastIndex, int aPivotIndex );
  void Heapsort( vector<string> * aLines, int aOffset, int aCount );
  void Heapify( vector<string> * aLines, int aOffset, int aCount );
  void SiftDown( vector<string> * aLines, int aStart, int aEnd, int aOffset );
  void SwapLines( vector<string> * aLines, int aFirst, int aSecond );
  int CompareMappingLine( string & aLeft, string & aRight );

protected:
  virtual int GetIndex( string & aLine ) = 0;
  virtual string GetContigIdent( string & aLine ) = 0;

public:
  MappingsPreparer(string & aPath);
  string PrepareMappings();
};
