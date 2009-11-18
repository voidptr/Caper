#pragma once
#include "stdafx.h"
#include "Typedefs.h"

#include <algorithm>
#include <time.h>

class MappingsPreparer
{
private:
  static const char Tab = '\t';
  static const char NewLine = '\n';

  string mPath;  

  class SortMappingIndexes
  {
    MappingsPreparer * mMappingsPrep;
  public:
    explicit SortMappingIndexes( MappingsPreparer * pf ) : mMappingsPrep(pf) {}
    bool operator() (MappingIndex aLeft, MappingIndex aRight ) 
    {
      return mMappingsPrep->LessThanMappingIndex( aLeft, aRight );
    }
  };

private:
  bool IsSorted();  
  vector<MappingIndex> * ReadAndInterpretMappingLines();  
  void WriteAllLines( vector<MappingIndex> * aMappingKeys, string & aFilename );
  string SortMappingsAndWriteToTmpFile();    
  bool LessThanMappingIndex( MappingIndex & aLeft, MappingIndex & aRight );

protected:
  virtual int GetIndex( string & aLine ) = 0;
  virtual string GetContigIdent( string & aLine ) = 0;

public:
  MappingsPreparer(string aPath);
  string PrepareMappings();
};

