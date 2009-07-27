#include "BowtieMappingUtilities.h"

string BowtieMappingUtilities::GetContigIdent( string & aLine )
{
	return GetLinePieces( aLine )[2];	
}

int BowtieMappingUtilities::GetIndex( string & aLine )
{
	return atoi( GetLinePieces( aLine )[3].c_str() );
}

string BowtieMappingUtilities::GetSequence( string & aLine )
{
	return GetLinePieces( aLine )[4];
}
