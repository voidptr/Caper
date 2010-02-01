#pragma once

#include "stdafx.h"
#include "MappingUtilities.h"

class BowtieMappingUtilities : public MappingUtilities
{
public:	
	string GetSequence( string & aLine );
  int GetIndex( string & aLine );    
  string GetName( string & aLine );
  string GetContigIdent( string & aLine );	
  string GetStrand( string & aLine );	

};
