#include "Caper.h"

void Caper::UserInterface(int argc, char * const argv[] )
{
  string lUsageString = "Caper v0.1\nUsage: caper [-si] -g <referencegenome.fa> <-m|-b> <mappingsfile>";
  string lCommandString = "You can type: \"help\", \"list\", \"quit\", \"<contig ident>:<X>:<Y>\", or \"<contig ident>:<X>:<Y>:p\" (for pretty mode)";

  Arguments lArgs;
  if ( !lArgs.ProcessArguments( argc, argv ) )
  {
    cout << lUsageString << endl;
    return;
  }

  cout << "Reading Genome \"" << lArgs.GenomePath << "\"... ";
  cout.flush();

  SequenceEngine * lSequenceEngine;
  lSequenceEngine = new FASequenceEngine( lArgs.GenomePath );
  lSequenceEngine->Initialize();

  cout << "Done!" << endl;

  cout << "Preparing Mappings \"" << lArgs.MappingPath << "\"... " << endl;
  cout.flush();

  MappingEngine * lMappingEngine;
  if ( lArgs.MappingStyle == lArgs.BOWTIE )
  {
    BowtieMappingsPreparer * lPrep = new BowtieMappingsPreparer( lArgs.MappingPath );
    string lNewPath = lPrep->PrepareMappings();
    delete lPrep;

    lMappingEngine = new BowtieMappingEngine( lNewPath, lSequenceEngine->mSequences );
  }
  else if ( lArgs.MappingStyle == lArgs.MAPVIEW )
  {
    MapviewMappingsPreparer * lPrep = new MapviewMappingsPreparer( lArgs.MappingPath );
    string lNewPath = lPrep->PrepareMappings();
    delete lPrep;

    lMappingEngine = new MapviewMappingEngine( lNewPath, lSequenceEngine->mSequences );
  }

  cout << "Done!" << endl;

  cout << "Initializing Mapping Engine... " << endl;
  lMappingEngine->Initialize();
  cout << "Done!" << endl;

  cout << "> ";

  string lInput = "";
  Commands lCommands;
  while ( cin >> lInput )
  {
    if ( !lCommands.ProcessArguments( lInput ) )
    {
      cout << "Invalid Input: " << lCommandString << endl ;
      cout << "> ";
      continue;
    }

    if ( lCommands.Action == lCommands.GETREADS )
    {

      


      if ( lCommands.Right > lSequenceEngine->mSequences[ lCommands.ContigIdent ]->Length )
      {
        cout << "Your Right Parameter is bigger than your reference genome.\n";
        cout << "> ";
        continue;
      }

      Mappings * lMappings = lMappingEngine->GetReads(lCommands.ContigIdent, lCommands.Left, lCommands.Right );
      if ( lCommands.PrettyMode ) // engage pretty mode
      {    
        cout << lCommands.Left << "\n";        

        if ( lCommands.Right + 1 < lSequenceEngine->mSequences[ lCommands.ContigIdent ]->Length )
          cout << lSequenceEngine->mSequences[ lCommands.ContigIdent ]->Substring( lCommands.Left, lCommands.Right - lCommands.Left + 1 ) << endl;
        else
          cout << lSequenceEngine->mSequences[ lCommands.ContigIdent ]->Substring( lCommands.Left ) << "\n";

        for ( int i = 0 ; i < lMappings->size(); i++ ) 
        {
          string lHighlightedString = lMappings->at(i)->mSequence->ToString();
          for ( int j = 0; j < lHighlightedString.length(); j++ )
          {
            if ( lHighlightedString[j] != 
              lSequenceEngine->mSequences[ lCommands.ContigIdent ]->Substring( 
                lMappings->at(i)->Index + j, 1 )[0] )
            {
              lHighlightedString[j] = toupper( lHighlightedString[j] );
            }
          }

          cout << PadLeft( lMappings->at(i)->Index - lCommands.Left ) << lHighlightedString << "\n"; 
        }
      }
      else
      {
        for ( int i = 0 ; i < lMappings->size(); i++ )
        {
          cout << "Index " << lMappings->at(i)->Index << ": " << lMappings->at(i)->mSequence->ToString() << "\n";
        }
      }

      delete lMappings;
    }

    cout << "> ";
  }
}


string Caper::PadLeft( int aCount )
{
  string lThing = "";

  for (int i = 0; i < aCount; i++ )
    lThing.append(" ");

  return lThing;
}



