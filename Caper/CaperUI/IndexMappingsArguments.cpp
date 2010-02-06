// IndexMappingsArguments.cpp
#include "IndexMappingsArguments.h"

// Constructor
// Initializes member variables.
IndexMappingsArguments::IndexMappingsArguments() : ModeArguments()
{
}

// Processes arguments.
bool IndexMappingsArguments::ProcessArguments( int argc, char * const argv[] )
{
  XGetOpt lGetOpt;

  if ( argc < 7 ) // sanity check, missing arguments.
    return false;

  int c;

  while ((c = lGetOpt.GetOpt(argc, argv, "o:m:t:")) != EOF)
  {
    switch (c)
    {
    //case 'f':
    //  ReferenceGenomeIndexPath = string(lGetOpt.optarg);
    //  break;

    //case 'g':
    //  GenomePath = string(lGetOpt.optarg);
    //  break;

    case 'o':
      SavePath = string(lGetOpt.optarg);
      break;

    case 't': 
      MappingStyle = MappingUtilitiesFactory::ToFormat( string(lGetOpt.optarg) );
      break;

    case 'm':
      MappingPath = string(lGetOpt.optarg);
      break;

    case '?':
      return false;
      break;

    default:
      return false;
      break;
    }
  }

  if (MappingPath.length() < 1 && SavePath.length() < 1)
    return false;

  return true;
}
