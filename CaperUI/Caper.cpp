#include "Caper.h"

void Caper::UserInterface(int argc, char * const argv[] )
{
  string lUsageString =
    "Caper v0.5.0\nUsage:\n caper indexmappings [-b] <-m mappingfile> <-t bowtie|mapview|sam> <-o savepath>\n caper interactive <-m indexedmappingfile -i mappingfileindex|-b compressedmappingbundle>\n";

  try
  {
    Arguments lArgs;
    if ( !lArgs.ProcessArguments( argc, argv ) )
    {
      cout << lUsageString << endl;
      return;
    }

    if ( lArgs.Mode == lArgs.INDEXMAPPINGS )
      IndexMappings( lArgs );
		else if ( lArgs.Mode == lArgs.INTERACTIVE )
      Interactive( lArgs );

    cout << "DONE!" << endl;
  }
  catch( string lException )
  {
    cerr << "ERROR: " << lException << endl;
    return;
  }
}

void Caper::IndexMappings( Arguments lArgs )
{
  IndexMappingsArguments * lModeArgs = (IndexMappingsArguments *) lArgs.ModeArgs;

  MappingIndexer lIndexer( lModeArgs->MappingPath, lModeArgs->MappingStyle, lModeArgs->SavePath, lModeArgs->Bundle );
  lIndexer.CreateIndex();

}

void Caper::Interactive( Arguments lArgs )
{
  string lCommandString = "You can type: \"<contig ident>:<X>:<Y>\", or \"<contig ident>:<X>:<Y>:p\" (for pretty mode)";

  InteractiveArguments * lModeArgs = (InteractiveArguments*) lArgs.ModeArgs;

  MappingEngine * lMappingEngine;
  if ( lModeArgs->BundlePath.length() > 0 )
    lMappingEngine = new MappingEngine( lModeArgs->BundlePath );
  else
    lMappingEngine = new MappingEngine( lModeArgs->MappingPath, lModeArgs->MappingIndexPath );
  lMappingEngine->Initialize();

  cout << "> ";

  string lInput = "";
  InteractiveCommands lCommands;

  while ( getline(cin, lInput) )
  {
	  // debug code
    //cout << "You typed: \"" << lInput << "\"" << endl;

    if ( !lCommands.ProcessArguments( lInput, lMappingEngine ) )
    {
      cout << "Invalid Input: " << lCommandString << endl << "> " ;
      continue;
    }

    try
    {
      if ( lCommands.Action == lCommands.READS )
      {
        MappingEngine::iterator lReadsIt = lMappingEngine->At( lCommands.ContigIdent, lCommands.Left );
        ReadsAtIndex * lMappings = lReadsIt.GetReads();

        for (ReadsAtIndex::iterator lIt = lMappings->begin(); lIt != lMappings->end(); ++lIt )
        {
          cout << (*lIt)->Index << ": " << (*lIt)->Name << " - " << (*lIt)->mSequence->ToString() << "\n";
        }

        lMappings->Destroy(); // this is supposed to decrement the reference count... ?
      }
      else if ( lCommands.Action == lCommands.INTERSECT )
      {
        MappingEngine::iterator lReadsIt = lMappingEngine->At( lCommands.ContigIdent, lCommands.Left );
        IndexedMappings * lMappings = lReadsIt.Intersect();

        for ( IndexedMappings::iterator lInterval = lMappings->begin(); lInterval != lMappings->end(); ++lInterval )
          for (ReadsAtIndex::iterator lIt = lInterval->second->begin(); lIt != lInterval->second->end(); ++lIt )
          {
            cout << (*lIt)->Index << ": " << (*lIt)->Name << " - " << (*lIt)->mSequence->ToString() << "\n";          }

        delete lMappings; // this is supposed to decrement the reference count... ?
        // don't know if I need to implement a Destroy method on IndexedMappings so it doesn't wipe out everything, or that it at least calls --ReferenceCount correctly, etc.
      }
      else if ( lCommands.Action == lCommands.INTERVAL )
      {
        IndexedMappings * lMappings = lMappingEngine->GetIntersection( lCommands.ContigIdent, lCommands.Left, lCommands.Right );

        for ( IndexedMappings::iterator lInterval = lMappings->begin(); lInterval != lMappings->end(); ++lInterval )
          for (ReadsAtIndex::iterator lIt = lInterval->second->begin(); lIt != lInterval->second->end(); ++lIt )
          {
            cout << (*lIt)->Index << ": " << (*lIt)->Name << " - " << (*lIt)->mSequence->ToString() << "\n";
          }

        delete lMappings; // this is supposed to decrement the reference count... ?
        // don't know if I need to implement a Destroy method on IndexedMappings so it doesn't wipe out everything, or that it at least calls --ReferenceCount correctly, etc.
      }
    }
    catch ( string aError )
    {
      cout << "Error: " << aError << endl;
    }

    cout << "> ";
  }
}


string Caper::PadLeft( int aCount )
{
  return PadLeft( aCount, " " );
}

string Caper::PadLeft( int aCount, string aPad )
{
  string lThing = "";

  for (int i = 0; i < aCount; i++ )
    lThing.append(aPad);

  return lThing;
}


