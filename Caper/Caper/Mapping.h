#pragma once

#include "stdafx.h"
#include "Sequence.h"

class Mapping
{
public:
	int Index;
	Sequence* mSequence;
public:
	Mapping(int & aPosition, Sequence* aSequence);
	~Mapping(void);
};
