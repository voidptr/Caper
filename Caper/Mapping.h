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
public:
	Mapping(string & aName, long long & aPosition, Sequence* aSequence, Orientation aStrand);
  Mapping( Mapping & aMapping );
	~Mapping(void);

	const char * NameP() { return Name.c_str(); }
	const int GetOrientation() { if (Strand == PLUS) return +1; else return -1; }
};
