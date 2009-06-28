#include "Mapping.h"

Mapping::Mapping(int & aPosition, Sequence* aSequence)
{
	mPosition = aPosition;
  //delete mSequence;
	mSequence = aSequence;
}

Mapping::~Mapping(void)
{
  delete mSequence;
}
