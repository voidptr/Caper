#pragma once
#include "MappingsPreparer.h"
#include "BowtieMappingUtilities.h"

class BowtieMappingsPreparer :
  public MappingsPreparer, public BowtieMappingUtilities
{
protected:
  int GetIndex( string & aLine );
  string GetContigIdent( string & aLine );

public:
  BowtieMappingsPreparer(string & aPath);
};
