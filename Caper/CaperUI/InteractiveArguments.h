// InteractiveArguments.h
#pragma once

#include "stdafx.h"
#include "XGetopt.h"
#include "ModeArguments.h"
#include "Typedefs.h"

// Parses command line arguments and sets member variables
class InteractiveArguments :
	public ModeArguments
{
private:
  // Passed in argument count
  int mArgC;
  // Passed in arguments
  char * mArgV;
public:
	string ReferenceGenomeIndexPath; // Loaded Reference Genome Index Path
  string GenomePath; // Reference Genome Path

  string MappingIndexPath; // Loaded Mapping Index Path
  string MappingPath; // Mapping File Path
  string BundlePath; // Bundle Path

public:
  // Process your arguments
  bool ProcessArguments( int argc, char * const argv[] );

  // Constructor
  InteractiveArguments(void);
};
