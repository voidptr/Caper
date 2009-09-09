#pragma once
#include "MappingEngine.h"
#include "BowtieMappingUtilities.h"

class BowtieMappingEngine : public MappingEngine, public BowtieMappingUtilities
{
public:
  BowtieMappingEngine( string aPath, Sequences * aReferenceGenome );

	string GetSequence( string & aLine );
  int GetIndex( string & aLine );   
  string GetName( string & aLine );
  string GetContigIdent( string & aLine );	
};
