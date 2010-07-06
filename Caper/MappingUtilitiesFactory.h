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
/*  static MappingUtilities * BuildMappingUtilities( MappingFileFormat aFormat );
  static MappingUtilities * BuildMappingUtilities( string aFormat );
  static MappingFileFormat ToFormat( string aFormat );
  static string ToString( MappingFileFormat aFormat );
 */ 
  static MappingUtilities * BuildMappingUtilities( MappingFileFormat aFormat )
  {
    if ( aFormat == BOWTIE )
      return new BowtieMappingUtilities();
    else if ( aFormat == MAPVIEW )
      return new MapviewMappingUtilities();
    else if ( aFormat == SAM )
      return new SAMMappingUtilities();
    else
      throw string("could not determine mapping file format"); // replace this with a static string (above)
  }

  static MappingUtilities * BuildMappingUtilities( string aFormat )
  {
    return BuildMappingUtilities( ToFormat( aFormat ) );
  }

  static MappingFileFormat ToFormat( string aFormat )
  {
    string lFormat = Utilities::ToUpper( aFormat );

    if ( lFormat == "BOWTIE" )
      return BOWTIE;
    else if ( lFormat == "MAPVIEW" )
      return MAPVIEW;
    else if ( lFormat == "SAM" )
      return SAM;
    else
      throw string("could not determine mapping file format");
  }

  static string ToString( MappingFileFormat aFormat )
  {
    switch ( aFormat )
    {
    case BOWTIE:
      return "BOWTIE";
      break;
    case MAPVIEW:
      return "MAPVIEW";
      break;
    case SAM:
      return "SAM";
      break;
    default:
      throw string("could not determine mapping file format");
      break;
    }
  }

};
