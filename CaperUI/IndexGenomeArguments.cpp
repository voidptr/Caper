// IndexGenomeArguments.cpp
#include "IndexGenomeArguments.h"

// Constructor
// Initializes member variables.
IndexGenomeArguments::IndexGenomeArguments() : ModeArguments()
{
}

// Processes arguments.
bool IndexGenomeArguments::ProcessArguments( int argc, char * const argv[] )
{
  XGetOpt lGetOpt;

  if ( argc < 5 ) // sanity check, missing arguments.
    return false;

  int c;

  while ((c = lGetOpt.GetOpt(argc, argv, "o:g:")) != EOF)
  {
    switch (c)
    {
    case 'o':
      SavePath = string(lGetOpt.optarg);
      break;

    case 'g':
      GenomePath = string(lGetOpt.optarg);
      break;

    case '?':
      return false;
      break;

    default:
      return false;
      break;
    }
  }

  if (GenomePath.length() < 1 || SavePath.length() < 1)
    return false;

  return true;
}
