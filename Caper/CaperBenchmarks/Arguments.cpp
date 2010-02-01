// Arguments.cpp
#include "Arguments.h"

// Constructor
// Initializes member variables.
Arguments::Arguments()
{
  SaveIndexes = false;
  LoadMappingIndex = false;
  MappingsSorted = false;
  LoadReferenceGenomeIndex = false;
  GenomePrepared = false;
  LoadMappings = false;
  InteractiveMode = false;
}

// Processes arguments.
bool Arguments::ProcessArguments( int argc, char * const argv[] ) 
{
  XGetOpt lGetOpt;
  
// commented out so that we can engage items in succession.
//  if ( argc < 5 ) // sanity check, missing arguments.
//    return false;

  int c;

  while ((c = lGetOpt.GetOpt(argc, argv, "s:f:i:g:G:m:M:b:B:x")) != EOF)
  {
    switch (c)
    {
    case 's':
      SaveIndexes = true;      
      SavePath = string(lGetOpt.optarg);
      break;

    case 'i':
      LoadMappingIndex = true;
      IndexPath = string(lGetOpt.optarg);
      break;

    case 'f':
      LoadReferenceGenomeIndex = true;
      ReferenceGenomeIndexPath = string(lGetOpt.optarg);
      break;

    case 'G':
      GenomePrepared = true;
    case 'g':
      GenomePath = string(lGetOpt.optarg);
      break;

    case 'M':
      MappingsSorted = true;
    case 'm':
      MappingStyle = MAPVIEW;
      LoadMappings = true;
      MappingPath = string(lGetOpt.optarg);
      break;

    case 'B':
      MappingsSorted = true;
    case 'b':
      MappingStyle = BOWTIE;
      LoadMappings = true;
      MappingPath = string(lGetOpt.optarg);                      
      break;

    case 'x':
      InteractiveMode = true;
      break;

    case '?':                             
      return false;                       
      break;
                   
    default:
      return false;                       
      break;               
    }           
  }                  

  if ( GenomePrepared ^ LoadReferenceGenomeIndex )
    return false;
  
  if (!(GenomePath.length() > 0) )
    return false; 

  if ( LoadMappings && !(MappingPath.length() > 0))
    return false;

  return true;
}
