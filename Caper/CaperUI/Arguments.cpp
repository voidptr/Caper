// Arguments.cpp
#include "Arguments.h"

// Constructor
// Initializes member variables.
Arguments::Arguments()
{
}

// Processes arguments.
bool Arguments::ProcessArguments( int argc, char * const argv[] )
{
  XGetOpt lGetOpt;

  string lMode = argv[1];

  if ( argc < 2 ) // sanity check, missing arguments.
    return false;

  if ( lMode == "interactive" )
  {
    Mode = INTERACTIVE;
    ModeArgs = new InteractiveArguments();
    return ModeArgs->ProcessArguments( argc - 1, argv + 1 );
  }
  else if ( lMode == "indexmappings" )
  {
    Mode = INDEXMAPPINGS;
    ModeArgs = new IndexMappingsArguments();
    return ModeArgs->ProcessArguments( argc - 1, argv + 1 );
  }
  else if ( lMode == "indexgenome")
  {
    Mode = INDEXGENOME;
    ModeArgs = new IndexGenomeArguments();
    return ModeArgs->ProcessArguments( argc - 1, argv + 1 );
  }


  return false;
}
