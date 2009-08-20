#include "Mapping.h"

Mapping::Mapping(string & aName, int & aIndex, Sequence* aSequence)
{
  Name = aName;
	Index = aIndex;
	mSequence = aSequence;
}

Mapping::Mapping( Mapping & aMapping )
{
  Name = aMapping.Name;
  Index = aMapping.Index;
  mSequence = new Sequence( *(aMapping.mSequence) );
}

Mapping::~Mapping(void)
{
  delete mSequence;
}
