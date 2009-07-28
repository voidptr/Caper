#include "Mapping.h"

Mapping::Mapping(int & aIndex, Sequence* aSequence)
{
	Index = aIndex;
	mSequence = aSequence;
}

Mapping::~Mapping(void)
{
  delete mSequence;
}
