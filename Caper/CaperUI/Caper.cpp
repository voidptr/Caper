#include "Caper.h"

void Caper::UserInterface(int argc, char * const argv[] )
{
  string lUsageString = "Caper v0.4.1\nUsage:\n caper indexgenome <-g referencegenome.fa> <-o savepath>\n caper indexmappings <-m mappingfile> <-t bowtie|mapview|sam> <-o savepath>\n caper interactive <-b|-v> <-g indexedreferencegenome.fa> <-f referencegenomeindexfile> <-m indexedmappingfile> <-i mappingfileindex>\n";

  try
  {
    Arguments lArgs;
    if ( !lArgs.ProcessArguments( argc, argv ) )
    {
      cout << lUsageString << endl;
      return;
    }

    if ( lArgs.Mode == lArgs.INDEXGENOME )
      IndexGenome( lArgs );
		else if ( lArgs.Mode == lArgs.INDEXMAPPINGS )
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

void Caper::IndexGenome( Arguments lArgs )
{
  IndexGenomeArguments * lModeArgs = (IndexGenomeArguments*) lArgs.ModeArgs;

  cout << "Reading Genome \"" << lModeArgs->GenomePath << "\"... ";
  cout.flush();

  SequenceIndexer * lSequenceIndexer = new FASequenceIndexer( lModeArgs->GenomePath, lModeArgs->SavePath );
  lSequenceIndexer->Index();
}

void Caper::IndexMappings( Arguments lArgs )
{
  IndexMappingsArguments * lModeArgs = (IndexMappingsArguments *) lArgs.ModeArgs;

  //SequenceEngine * lSequenceEngine = new FASequenceEngine( lModeArgs->GenomePath, lModeArgs->ReferenceGenomeIndexPath );
  //lSequenceEngine->Initialize();

  cout << "Preparing Mappings \"" << lModeArgs->MappingPath << "\"... " << endl;
  cout.flush();

  MappingsIndexer lIndexer( lModeArgs->MappingPath, lModeArgs->MappingStyle, lModeArgs->SavePath );
  lIndexer.IndexMappingsAndSave();
 
}

void Caper::Interactive( Arguments lArgs )
{
  string lCommandString = "You can type: \"<contig ident>:<X>:<Y>\", or \"<contig ident>:<X>:<Y>:p\" (for pretty mode)";

  InteractiveArguments * lModeArgs = (InteractiveArguments*) lArgs.ModeArgs;

  SequenceEngine * lSequenceEngine = new FASequenceEngine( lModeArgs->GenomePath, lModeArgs->ReferenceGenomeIndexPath );
  lSequenceEngine->Initialize();

  MappingEngine * lMappingEngine = new MappingEngine( lModeArgs->MappingPath, lModeArgs->MappingIndexPath );
  lMappingEngine->Initialize();

  cout << "> ";

  string lInput = "";
  Commands lCommands;

  while ( cin >> lInput )
  {
    if ( !lCommands.ProcessArguments( lInput, lSequenceEngine->mSequences, lMappingEngine ) )
    {
      cout << "Invalid Input: " << lCommandString << endl << "> " ;
      continue;
    }

    if ( lCommands.Action == lCommands.GETREADS )
    {
      Mappings * lMappings = lMappingEngine->GetReads(lCommands.ContigIdent, lCommands.Left, lCommands.Right );
      if ( lCommands.PrettyMode ) // engage pretty mode
      {
        cout << PadLeft( lMappingEngine->ReadLength ) << lCommands.Left + lMappingEngine->ReadLength << endl;
        cout << PadLeft( lMappingEngine->ReadLength ) << PadLeft( lCommands.Right - lCommands.Left, "*") << endl;

        string lGenome = "";

        Sequence * lContig = (*lSequenceEngine->mSequences)[ lCommands.ContigIdent ];

        int lTargetGenomeWidth = lCommands.Right - lCommands.Left + 1 + lMappingEngine->ReadLength;
        if ( lTargetGenomeWidth < lContig->Length )
          lGenome = lContig->Substring( lCommands.Left, lTargetGenomeWidth );
        else
          lGenome = lContig->Substring( lCommands.Left );

        cout << lGenome << endl;

        int lGenomeLength = lGenome.length();
        for ( int i = 0 ; i < lMappings->size(); i++ )
        {
          string lHighlightedString = lMappings->at(i)->mSequence->ToString();
          for ( int j = 0; j < lHighlightedString.length(); j++ )
          {
            int lTargetLocalIndexOnGenome = lMappings->at(i)->Index - lCommands.Left + j;
            if ( lTargetLocalIndexOnGenome < lGenome.length() &&
              lHighlightedString[j] != lGenome[ lTargetLocalIndexOnGenome ] )
            {
              if ( islower(lHighlightedString[j]) )
                lHighlightedString[j] = toupper( lHighlightedString[j] );
              else
                lHighlightedString[j] = tolower( lHighlightedString[j] );
            }
          }

          cout << PadLeft( lMappings->at(i)->Index - lCommands.Left ) << lHighlightedString << "\n";
        }
      }
      else
      {
        for ( int i = 0 ; i < lMappings->size(); i++ )
        {
          cout << "Index " << lMappings->at(i)->Index << ": " << lMappings->at(i)->Name << " - " << lMappings->at(i)->mSequence->ToString() << "\n";
        }
      }

      delete lMappings;
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


