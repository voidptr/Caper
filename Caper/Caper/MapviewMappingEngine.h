#pragma once
#include "MappingEngine.h"
#include "MapviewMappingUtilities.h"

class MapviewMappingEngine : public MappingEngine, public MapviewMappingUtilities
{
public:
  MapviewMappingEngine( string & aPath, Sequences & aReferenceGenome );

  string GetSequence( string & aLine );
  int GetIndex( string & aLine );    
  string GetName( string & aLine );
  string GetContigIdent( string & aLine );
};
