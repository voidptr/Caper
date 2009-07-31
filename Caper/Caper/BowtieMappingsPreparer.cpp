#include "BowtieMappingsPreparer.h"

BowtieMappingsPreparer::BowtieMappingsPreparer(string & aPath) : 
MappingsPreparer( aPath )
{
}

string BowtieMappingsPreparer::GetContigIdent(string &aLine)
{
  return BowtieMappingUtilities::GetContigIdent( aLine );
}

int BowtieMappingsPreparer::GetIndex(string &aLine)
{
  return BowtieMappingUtilities::GetIndex( aLine );
}
