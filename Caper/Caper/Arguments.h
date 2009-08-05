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

  bool SaveSortedMapping;
  bool LoadIndex;

  bool ValidArguments;
public: 
  bool ProcessArguments( int argc, char * const argv[] );
  Arguments(void);
};
