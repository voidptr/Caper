#pragma once

#include "stdafx.h"
#include "MappingUtilities.h"

class MapviewMappingUtilities : public MappingUtilities
{
public:
  long long StringToIndex( string & aIndex ) // mapview is 1-based.
  {
    return atoi( aIndex.c_str() ) - 1; // could be an overflow. :/
  }
  int GetContigIdentColumn() { return 1; }
  int GetIndexColumn() { return 2; }
  int GetSequenceColumn() { return 14; }
  int GetNameColumn() { return 0; }
  int GetStrandColumn() { return 3; }


};
