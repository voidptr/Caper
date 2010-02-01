// InteractiveArguments.cpp
#include "InteractiveArguments.h"

// Constructor
// Initializes member variables.
InteractiveArguments::InteractiveArguments() : ModeArguments()
{
}

// Processes arguments.
bool InteractiveArguments::ProcessArguments( int argc, char * const argv[] )
{
  XGetOpt lGetOpt;

  if ( argc < 9 ) // sanity check, missing arguments.
    return false;

  int c;

  while ((c = lGetOpt.GetOpt(argc, argv, "f:i:g:m:")) != EOF)
  {
    switch (c)
    {
    case 'i':
      MappingIndexPath = string(lGetOpt.optarg);
      break;

    case 'f':
      ReferenceGenomeIndexPath = string(lGetOpt.optarg);
      break;

    case 'g':
      GenomePath = string(lGetOpt.optarg);
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

  if (GenomePath.length() < 1 ||
  	MappingPath.length() < 1 ||
  	MappingIndexPath.length() < 1 ||
  	ReferenceGenomeIndexPath.length() < 1)
    return false;

  return true;
}
