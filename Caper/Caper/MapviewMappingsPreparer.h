#pragma once
#include "MappingsPreparer.h"
#include "MapviewMappingUtilities.h"

class MapviewMappingsPreparer :
  public MappingsPreparer, MapviewMappingUtilities
{
protected:
  int GetIndex( string & aLine );
  string GetContigIdent( string & aLine );

public:
  MapviewMappingsPreparer(string & aPath);
};
