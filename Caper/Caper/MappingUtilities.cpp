#include "MappingUtilities.h"

vector<string> MappingUtilities::GetLinePieces( string & aLine )
{
	vector<string> lPieces;

	string lBit;
	istringstream lStream( aLine );
	do
	{
    lBit = "";
		lStream >> lBit;

		if ( lBit.length() > 0 )
			lPieces.push_back( lBit );
		else
			break;

	} while (true);
		
	return lPieces;
}