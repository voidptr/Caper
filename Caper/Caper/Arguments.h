// Arguments.h
#pragma once

#include "stdafx.h"
#include "XGetopt.h"

// Parses command line arguments and sets member variables 
class Arguments
{
private:
  // Passed in argument count
  int mArgC;
  // Passed in arguments
  char * mArgV;
public:
  enum MappingStyle
  {
    MAPVIEW,
    BOWTIE
  } MappingStyle; // Mapview Mappings or BowtieMappings

  string IndexPath; // Loaded Mapping Index Path
  string GenomePath; // Reference Genome Path
  string MappingPath; // Mapping File Path
  string SavePath; // When saving, save to this path
  string ReferenceGenomeIndexPath; // Loaded Reference Genome Index Path

  bool SaveIndexes; // Save your indexes
  bool LoadMappingIndex; // Load your mapping indexe
  bool MappingsSorted; // Files are already sorted and formatted
  bool GenomePrepared; // Genome is already processed.
  bool LoadReferenceGenomeIndex; // Load your ref genome index

  bool ValidArguments; // All arguments are valid
public: 
  // Process your arguments
  bool ProcessArguments( int argc, char * const argv[] );
  
  // Constructor
  Arguments(void);
};
