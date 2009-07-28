#include "Arguments.h"

bool Arguments::ProcessArguments( int argc, char * const argv[] ) 
{
  int c;

  while ((c = getopt(argc, argv, "sig:m:b:")) != EOF)
  {
    switch (c)
    {
    case 's':
      SaveSortedMapping = true;      
      break;
    case 'i':
      SaveIndex = true;
      break;

    case 'g':
      GenomePath = string(optarg);
      break;

    case 'm':
      MappingStyle = MAPVIEW;
      MappingPath = string(optarg);
      break;

    case 'b':
      MappingStyle = BOWTIE;
      MappingPath = string(optarg);                      
      break;

    case '?':                             
      return false;                       
      break;
                   
    default:
      return false;                       
      break;               
    }           
  }                  
  return true;
}
