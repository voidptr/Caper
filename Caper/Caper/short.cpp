#include "Caper.h"

class Caper;

int main()
{
  string genome = "REL606.gmc.fa";
  string mapping = "head.txt";
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
  return 0;
}
