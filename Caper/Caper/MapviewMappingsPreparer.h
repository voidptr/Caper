#pragma once
#include "MappingsPreparer.h"
#include "MapviewMappingUtilities.h"

class MapviewMappingsPreparer :
  public MappingsPreparer, MapviewMappingUtilities
{
protected:
  int GetIndex( string & aLine );
  string GetContigIdent( string & aLine );
  string holder;

public:
  MapviewMappingsPreparer(string aPath);
  const char * PrepareMappingsPy() {
    holder = PrepareMappings(); return holder.c_str();
  }
};
