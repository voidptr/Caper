#pragma once

#include "stdafx.h"

class MappingUtilities
{
public:
//	virtual string GetSequence( string aLine )=0;
//    virtual int GetIndex( string aLine )=0;    
//    virtual string GetContigIdent( string aLine )=0;
	void GetLinePieces( string & aLine, string * aPieces );
};
