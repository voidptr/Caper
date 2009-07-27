#pragma once
#include "mappingspreparer.h"
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
