#pragma once

#include "stdafx.h"
#include "Sequence.h"

class Mapping
{
public:
  string Name;
	int Index;
	Sequence* mSequence;
public:
	Mapping(string & aName, int & aPosition, Sequence* aSequence);
  Mapping( Mapping & aMapping );
	~Mapping(void);

	const char * NameP() { return Name.c_str(); }
};
