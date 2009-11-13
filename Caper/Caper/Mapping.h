#pragma once

#include "stdafx.h"
#include "Sequence.h"

class Mapping
{
public:
  string Name;
	int Index;
	Sequence* mSequence;
  enum Orientation
  {
    PLUS,
    MINUS
  } Strand;
public:
	Mapping(string & aName, int & aPosition, Sequence* aSequence, Orientation aStrand);
  Mapping( Mapping & aMapping );
	~Mapping(void);

	const char * NameP() { return Name.c_str(); }
};
