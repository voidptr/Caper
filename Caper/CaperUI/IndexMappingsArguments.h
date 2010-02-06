// IndexMappingsArguments.h
#pragma once

#include "stdafx.h"
#include "XGetopt.h"
#include "ModeArguments.h"
#include "MappingUtilitiesFactory.h"
#include "Typedefs.h"

// Parses command line arguments and sets member variables
class IndexMappingsArguments :
	public ModeArguments
{
private:
  // Passed in argument count
  int mArgC;
  // Passed in arguments
  char * mArgV;
public:
  MappingFileFormat MappingStyle; // Mapview Mappings or BowtieMappings

  string MappingPath; // Mapping File Path
  string SavePath; // When saving, save to this path

  //string ReferenceGenomeIndexPath; // Loaded Reference Genome Index Path
  //string GenomePath; // Reference Genome Path

public:
  // Process your arguments
  bool ProcessArguments( int argc, char * const argv[] );

  // Constructor
  IndexMappingsArguments(void);
};
