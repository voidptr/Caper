#include "SAMMappingUtilities.h"

string SAMMappingUtilities::GetContigIdent( string & aLine )
{
return GetLineItem( 2, aLine );
}
 
long long SAMMappingUtilities::GetIndex( string & aLine )
{
return atoi( GetLineItem( 3, aLine ).c_str() );
}
 
string SAMMappingUtilities::GetSequence( string & aLine )
{
return GetLineItem( 9, aLine );
}
 
string SAMMappingUtilities::GetName( string & aLine )
{
return GetLineItem( 0, aLine );
}
 
string SAMMappingUtilities::GetStrand( string & aLine )
{
  string lFlag = GetLineItem( 1, aLine );
  unsigned int lFlagInt = atoi( lFlag.c_str() );

  if ( (lFlagInt & 0x010) == 0 )
    return "+";
  else
    return "-";
}
