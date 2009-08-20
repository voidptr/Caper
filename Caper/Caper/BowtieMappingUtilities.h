#pragma once

#include "stdafx.h"
#include "MappingUtilities.h"

class BowtieMappingUtilities : public MappingUtilities
{
public:	
	string virtual GetSequence( string & aLine );
  int virtual GetIndex( string & aLine );    
  string virtual GetName( string & aLine );
  string virtual GetContigIdent( string & aLine );	
};
