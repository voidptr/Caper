#include "BowtieMappingUtilities.h"
 
string BowtieMappingUtilities::GetContigIdent( string & aLine )
{
return GetLineItem( 2, aLine );
}
 
long long BowtieMappingUtilities::GetIndex( string & aLine )
{
return atoi( GetLineItem( 3, aLine ).c_str() ); // could be an overflow. :/
}
 
string BowtieMappingUtilities::GetSequence( string & aLine )
{
return GetLineItem( 4, aLine );
}
 
string BowtieMappingUtilities::GetName( string & aLine )
{
return GetLineItem( 0, aLine );
}
 
string BowtieMappingUtilities::GetStrand( string & aLine )
{
  return GetLineItem( 1, aLine );
}
 