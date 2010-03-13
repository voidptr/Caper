#pragma once

#include "MappingUtilities.h"
#include "MapviewMappingUtilities.h"
#include "BowtieMappingUtilities.h"
#include "SAMMappingUtilities.h"
#include "Typedefs.h"
#include "Utilities.h"

class MappingUtilitiesFactory
{
public:
  static MappingUtilities * BuildMappingUtilities( MappingFileFormat aFormat );
  static MappingUtilities * BuildMappingUtilities( string aFormat );
  static MappingFileFormat ToFormat( string aFormat );
  static string ToString( MappingFileFormat aFormat );
};
