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

  class SortMapping
  {
    MappingsPreparer * mMappingsPrep;
  public:
    explicit SortMapping( MappingsPreparer * pf) : mMappingsPrep(pf) {}
    bool operator() (string aLeft, string aRight ) 
    {
      return mMappingsPrep->LessThanMappingLine( aLeft, aRight );
    }
  };

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
  vector<MappingIndex> * InterpretMappingLines( vector<string> * aMappings );
  vector<string> * ReadAllLines();
  void WriteAllLines( vector<string>* aMappingsFile, string & aFilename );
  string SortMappingsAndWriteToTmpFile();  
  bool SeparateByContigs( vector<string> * aMappings );
  bool LessThanMappingLine( string & aLeft, string & aRight );
  bool LessThanMappingIndex( MappingIndex & aLeft, MappingIndex & aRight );

protected:
  virtual int GetIndex( string & aLine ) = 0;
  virtual string GetContigIdent( string & aLine ) = 0;

public:
  MappingsPreparer(string aPath);
  string PrepareMappings();
};

