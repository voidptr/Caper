#include "MapviewMappingUtilities.h"

string MapviewMappingUtilities::GetContigIdent( string & aLine )
{
	return GetLineItem( 1, aLine );	
}

long long MapviewMappingUtilities::GetIndex( string & aLine )
{
	return atoi( GetLineItem( 2, aLine ).c_str() ) - 1;
}

string MapviewMappingUtilities::GetSequence( string & aLine )
{
	return GetLineItem( 14, aLine );
}

string MapviewMappingUtilities::GetName( string & aLine )
{
	return GetLineItem( 0, aLine );
}

string MapviewMappingUtilities::GetStrand( string & aLine )
{
  return GetLineItem( 3, aLine );
}

