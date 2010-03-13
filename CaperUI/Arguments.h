// Arguments.h
#pragma once

#include "stdafx.h"
#include "XGetopt.h"
#include "ModeArguments.h"
#include "IndexGenomeArguments.h"
#include "IndexMappingsArguments.h"
#include "InteractiveArguments.h"

// Parses command line arguments and sets member variables
class Arguments
{
private:
  // Passed in argument count
  int mArgC;
  // Passed in arguments
  char * mArgV;


public:
  enum Modes
  {
    INTERACTIVE,
    INDEXGENOME,
    INDEXMAPPINGS
  } Mode;

  ModeArguments * ModeArgs;

public:
  // Process your arguments
  bool ProcessArguments( int argc, char * const argv[] );

  // Constructor
  Arguments(void);
};
