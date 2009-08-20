#include "BowtieMappingEngine.h"

BowtieMappingEngine::BowtieMappingEngine(string & aPath, Sequences & aGenome) 
: MappingEngine(aPath, aGenome), BowtieMappingUtilities()
{
}

string BowtieMappingEngine::GetContigIdent( string & aLine )
{
  return BowtieMappingUtilities::GetContigIdent(aLine);
}

int BowtieMappingEngine::GetIndex( string & aLine )
{
  return BowtieMappingUtilities::GetIndex(aLine);
}

string BowtieMappingEngine::GetName( string & aLine )
{
  return BowtieMappingUtilities::GetName(aLine);
}

string BowtieMappingEngine::GetSequence( string & aLine )
{
  return BowtieMappingUtilities::GetSequence(aLine);
}
