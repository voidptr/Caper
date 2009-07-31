#pragma once
#include "stdafx.h"

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

private:
  bool IsSorted();
  vector<string> * ReadAllLines();
  void WriteAllLines( vector<string>* aMappingsFile, string & aFilename );
  string SortMappingsAndWriteToTmpFile();  
  bool LessThanMappingLine( string & aLeft, string & aRight );

protected:
  virtual int GetIndex( string & aLine ) = 0;
  virtual string GetContigIdent( string & aLine ) = 0;

public:
  MappingsPreparer(string & aPath);
  string PrepareMappings();
};

