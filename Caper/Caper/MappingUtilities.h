#pragma once
#include "stdafx.h"

class MappingUtilities
{	
public:
	vector<string> GetLinePieces( string & aLine );
	string GetLineItem( int aIndex, string & aLine );
};
