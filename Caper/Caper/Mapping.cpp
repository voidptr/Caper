#include "Mapping.h"

Mapping::Mapping(string & aName, int & aIndex, Sequence* aSequence, Orientation aStrand)
{
  Name = aName;
	Index = aIndex;
	mSequence = aSequence;
  Strand = aStrand;
}

Mapping::Mapping( Mapping & aMapping )
{
  Name = aMapping.Name;
  Index = aMapping.Index;
  mSequence = new Sequence( *(aMapping.mSequence) );
  Strand = aMapping.Strand;
}

Mapping::~Mapping(void)
{
  delete mSequence;
}
