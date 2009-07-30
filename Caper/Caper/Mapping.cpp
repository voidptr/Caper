#include "Mapping.h"

Mapping::Mapping(int & aIndex, Sequence* aSequence)
{
	Index = aIndex;
	mSequence = aSequence;
}

Mapping::Mapping( Mapping & aMapping )
{
  Index = aMapping.Index;
  mSequence = new Sequence( *(aMapping.mSequence) );
}

Mapping::~Mapping(void)
{
  delete mSequence;
}
