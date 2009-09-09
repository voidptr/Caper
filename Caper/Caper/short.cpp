#include "Caper.h"

class Caper;

int main()
{
  string genome = "data/REL606.gmc.fa";
  string mapping = "data/REL606-maq-map.txt";
  SequenceEngine * lSequenceEngine;
  lSequenceEngine = new FASequenceEngine( genome );
  lSequenceEngine->Initialize();

  MapviewMappingsPreparer * lPrep = new MapviewMappingsPreparer( mapping );
  string lNewPath = lPrep->PrepareMappings();
  delete lPrep;

  cout << "new path:" << lNewPath << "\n";

  MappingEngine * lMappingEngine;
  lMappingEngine = new MapviewMappingEngine( lNewPath, lSequenceEngine->mSequences );

  lMappingEngine->Initialize();

  string contig = "rel606";
  int left = 0;
  int right = 50;

  Mappings * lMappings = lMappingEngine->GetReads(contig, 0, 50 );

  for ( int i = 0 ; i < lMappings->size(); i++ )
    {
      cout << "Index " << lMappings->at(i)->Index << ": " << lMappings->at(i)->mSequence->ToString() << "\n";
    }

  lMappings = lMappingEngine->GetReads(contig, 14000, 14999);

  // crashing bug
  lMappings = lMappingEngine->GetReads(contig, 0, 19999); // works
  lMappings = lMappingEngine->GetReads(contig, 0, 20000); // used to die horribly.

  // for simple memory leak test:
#if 0
  while (1) {
    std::cout << "...\n";
    for (int x = 0; x < 500; x++) {
      Mappings * lMappings = lMappingEngine->GetReads(contig, 0, 50 );

      for ( int i = 0 ; i < lMappings->size(); i++ )
	{
	  Mapping * m = lMappings->at(i);
	  delete m;
	}

      delete lMappings;
    }
    sleep(1);
  }
#endif

  return 0;
}
