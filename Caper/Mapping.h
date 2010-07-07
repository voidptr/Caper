#pragma once

#include "stdafx.h"
#include "Sequence.h"

class Mapping
{
public:
  string Name;
  long long Index;
  Sequence* mSequence;
  enum Orientation
  {
    PLUS,
    MINUS
  } Strand;

  int ReferenceCount;
public:

  const char * NameP() { return Name.c_str(); }
  const int GetOrientation() { if (Strand == PLUS) return +1; else return -1; }

  Mapping(string & aName, long long & aIndex, Sequence* aSequence, Orientation aStrand)
  {
    Name = aName;
    Index = aIndex;
    mSequence = aSequence;
    Strand = aStrand;

    ReferenceCount = 1; // someone just created me, I'm assuming they're holding a reference to me.
  }

  Mapping( Mapping & aMapping )
  {
    Name = aMapping.Name;
    Index = aMapping.Index;
    mSequence = new Sequence( *(aMapping.mSequence) );
    Strand = aMapping.Strand;
  }

  ~Mapping(void) // I'm assuming this will work.
  {
    cout << "~~Mapping - Destructor" << endl;
    --ReferenceCount;
    if ( ReferenceCount < 1 )
    {
      cout << "~~Mapping - Destructor - DOING IT" << endl;
      delete mSequence;
    }
  }
};
