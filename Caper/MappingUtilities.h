#pragma once
#include "stdafx.h"

class MappingUtilities
{
public:
	virtual vector<string> GetLinePieces( string & aLine );
  virtual string GetLineItem( int aColumn, string & aLine );

  virtual string GetSequence( string & aLine ) = 0;
  virtual long long GetIndex( string & aLine ) = 0;    
  virtual string GetName( string & aLine ) = 0;
  virtual string GetContigIdent( string & aLine ) = 0;	
  virtual string GetStrand( string & aLine ) = 0;	
};
