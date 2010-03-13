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

  if ( argc < 7 ) // sanity check, missing arguments.
    return false;

  int c;

  while ((c = lGetOpt.GetOpt(argc, argv, "f:i:g:m:b:")) != EOF)
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

    case 'b':
      BundlePath = string(lGetOpt.optarg);
      break;

    case '?':
      return false;
      break;

    default:
      return false;
      break;
    }
  }

  // genome path is required.
  if (GenomePath.length() < 1 ||
  	ReferenceGenomeIndexPath.length() < 1)
    return false;

  // have to have at least one fully set
  if ( (MappingPath.length() < 1 ||
  	    MappingIndexPath.length() < 1 ) &&
        BundlePath.length() < 1 )
    return false;

  // can't have both at the same time.
  if ( BundlePath.length() > 0 && ( MappingPath.length() > 0 || MappingIndexPath.length() > 0 ) )
    return false;

  return true;
}
