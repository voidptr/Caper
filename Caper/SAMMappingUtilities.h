#pragma once
#include "MappingUtilities.h"

class SAMMappingUtilities :
  public MappingUtilities
{
public:
  string GetStrand( string & aLine )
  {
    string lFlag = GetLineItem( GetStrandColumn(), aLine );
    unsigned int lFlagInt = atoi( lFlag.c_str() );

    if ( (lFlagInt & 0x010) == 0 )
      return "+";
    else
      return "-";
  }

  int GetContigIdentColumn() { return 2; }
  int GetIndexColumn() { return 3; }
  int GetSequenceColumn() { return 9; }
  int GetNameColumn() { return 0; }
  int GetStrandColumn() { return 1; }
  int GetSequenceLengthColumn() { return -1; } // there isn't one.

  long long GetSequenceLength( string & aLine )
  {
    return GetSequence( aLine ).length();
  }

};
