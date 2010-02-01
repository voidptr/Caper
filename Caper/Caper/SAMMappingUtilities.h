#pragma once
#include "mappingutilities.h"

class SAMMappingUtilities :
  public MappingUtilities
{
public:
	string GetSequence( string & aLine );
  int GetIndex( string & aLine );    
  string GetName( string & aLine );
  string GetContigIdent( string & aLine );	
  string GetStrand( string & aLine );	
};
