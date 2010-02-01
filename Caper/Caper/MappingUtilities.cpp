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

string MappingUtilities::GetLineItem( int aIndex, string & aLine )
{
  string lBit;
  istringstream lStream( aLine );
  for (int i = 0; i <= aIndex; i++)
  {
    lBit = "";
    lStream >> lBit;

    assert( lBit.length() > 0 ); // line doesn't have enough elements
  }
  return lBit;
}
