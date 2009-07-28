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

  string GenomePath;
  string MappingPath;

  bool SaveSortedMapping;
  bool SaveIndex;

  bool ValidArguments;
public: 
  bool ProcessArguments( int argc, char * const argv[] );
};
