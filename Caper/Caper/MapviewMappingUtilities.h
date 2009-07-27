#pragma once

#include "stdafx.h"
#include "MappingUtilities.h"

class MapviewMappingUtilities : public MappingUtilities
{
public:	
	string GetSequence( string & aLine );
    int GetIndex( string & aLine );    
    string GetContigIdent( string & aLine );	
};
