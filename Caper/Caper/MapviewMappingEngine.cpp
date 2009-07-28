#include "MapviewMappingEngine.h"

MapviewMappingEngine::MapviewMappingEngine(string & aPath, Sequences & aGenome) 
: MappingEngine(aPath, aGenome), MapviewMappingUtilities()
{
}

string MapviewMappingEngine::GetContigIdent( string & aLine )
{
  return MapviewMappingUtilities::GetContigIdent(aLine);
}

int MapviewMappingEngine::GetIndex( string & aLine )
{
  return MapviewMappingUtilities::GetIndex(aLine);
}

string MapviewMappingEngine::GetSequence( string & aLine )
{
  return MapviewMappingUtilities::GetSequence(aLine);
}
