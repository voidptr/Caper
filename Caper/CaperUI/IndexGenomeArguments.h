// IndexGenomeArguments.h
#pragma once

#include "stdafx.h"
#include "XGetopt.h"
#include "ModeArguments.h"

// Parses command line arguments and sets member variables
class IndexGenomeArguments :
  public ModeArguments
{
private:
  // Passed in argument count
  int mArgC;
  // Passed in arguments
  char * mArgV;
public:

  string GenomePath; // Reference Genome Path
  string SavePath; // When saving, save to this path

public:
  // Process your arguments
  bool ProcessArguments( int argc, char * const argv[] );

  // Constructor
  IndexGenomeArguments(void);
};
