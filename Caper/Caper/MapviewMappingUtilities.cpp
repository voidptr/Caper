#include "MapviewMappingUtilities.h"

string MapviewMappingUtilities::GetContigIdent( string & aLine )
{
	return GetLinePieces( aLine )[1];	
}

int MapviewMappingUtilities::GetIndex( string & aLine )
{
	return atoi( GetLinePieces( aLine )[2].c_str() ) - 1;
}

string MapviewMappingUtilities::GetSequence( string & aLine )
{
	return GetLinePieces( aLine )[14];
}

string MapviewMappingUtilities::GetName( string & aLine )
{
	return GetLinePieces( aLine )[0];
}