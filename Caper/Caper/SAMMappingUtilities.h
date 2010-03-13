#pragma once
#include "MappingUtilities.h"

class SAMMappingUtilities :
  public MappingUtilities
{
public:
	string GetSequence( string & aLine );
  long long GetIndex( string & aLine );    
  string GetName( string & aLine );
  string GetContigIdent( string & aLine );	
  string GetStrand( string & aLine );	
};
