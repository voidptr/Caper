#pragma once

#include "stdafx.h"
#include "XGetopt.h"

class Arguments
{
private:
  int mArgC;
  char * mArgV;
public:
  enum MappingStyle
  {
    MAPVIEW,
    BOWTIE
  } MappingStyle;

  string IndexPath;
  string GenomePath;
  string MappingPath;
  string SavePath;
  string ReferenceGenomeIndexPath;

  bool SaveIndexes;
  bool LoadIndex;
  bool AlreadySorted;
  bool LoadReferenceGenomeIndex;

  bool ValidArguments;
public: 
  bool ProcessArguments( int argc, char * const argv[] );
  Arguments(void);
};
