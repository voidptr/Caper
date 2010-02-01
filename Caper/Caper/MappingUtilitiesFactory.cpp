
#include "MappingUtilitiesFactory.h"

MappingUtilities * MappingUtilitiesFactory::BuildMappingUtilities( MappingFileFormat aFormat )
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

MappingUtilities * MappingUtilitiesFactory::BuildMappingUtilities( string aFormat )
{
  return BuildMappingUtilities( ToFormat( aFormat ) );
}

MappingFileFormat MappingUtilitiesFactory::ToFormat( string aFormat )
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

string MappingUtilitiesFactory::ToString( MappingFileFormat aFormat )
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