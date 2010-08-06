#pragma once

#include "stdafx.h"
#include "MappingUtilities.h"

class BowtieMappingUtilities : public MappingUtilities
{
public:
  int GetIndexColumn() { return 3; }
  int GetSequenceColumn() { return 4; }
  int GetNameColumn() { return 0; }
  int GetContigIdentColumn() { return 2; }
  int GetStrandColumn() { return 1; }
  int GetSequenceLengthColumn() { return -1; } // there isn't one.

  long long GetSequenceLength( string & aLine )
  {
    return GetSequence( aLine ).length();
  }
};
