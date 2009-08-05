#include "Arguments.h"

Arguments::Arguments()
{
  SaveSortedMapping = false;
  LoadIndex = false;
}

bool Arguments::ProcessArguments( int argc, char * const argv[] ) 
{
  if ( argc < 5 )
    return false;

  int c;

  while ((c = getopt(argc, argv, "s:i:g:m:b:")) != EOF)
  {
    switch (c)
    {
    case 's':
      SaveSortedMapping = true;      
      SavePath = string(optarg);
      break;

    case 'i':
      LoadIndex = true;
      IndexPath = string(optarg);
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
