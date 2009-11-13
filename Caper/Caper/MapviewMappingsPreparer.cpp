#include "MapviewMappingsPreparer.h"

MapviewMappingsPreparer::MapviewMappingsPreparer(string aPath) 
: MappingsPreparer( aPath )
{
}

string MapviewMappingsPreparer::GetContigIdent(string &aLine)
{
  return MapviewMappingUtilities::GetContigIdent( aLine );
}

int MapviewMappingsPreparer::GetIndex(string &aLine)
{
  return MapviewMappingUtilities::GetIndex( aLine );
}
