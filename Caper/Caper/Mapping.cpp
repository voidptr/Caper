#include "Mapping.h"

Mapping::Mapping(int & aIndex, Sequence* aSequence)
{
	Index = aIndex;
  //delete mSequence;
	mSequence = aSequence;
}

Mapping::~Mapping(void)
{
  delete mSequence;
}
