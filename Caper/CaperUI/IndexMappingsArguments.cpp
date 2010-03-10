// IndexMappingsArguments.cpp
#include "IndexMappingsArguments.h"

// Constructor
// Initializes member variables.
IndexMappingsArguments::IndexMappingsArguments() : ModeArguments()
{
  Bundle = false;
}

// Processes arguments.
bool IndexMappingsArguments::ProcessArguments( int argc, char * const argv[] )
{
  XGetOpt lGetOpt;

  if ( argc < 7 ) // sanity check, missing arguments.
    return false;

  int c;

  while ((c = lGetOpt.GetOpt(argc, argv, "o:m:t:b")) != EOF)
  {
    switch (c)
    {
    case 'b':
      Bundle = true;
      break;

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
