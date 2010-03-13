#include "Tests.h"



int Tests::RunTests(int argc, char * const argv[] )
{
  cout << "Caper Automated Tests " << endl << endl;

  // A. top to bottom tests
  // ----------------------
  cout << "A. Front to Back tests" << endl;

  cout << "A1. indexgenome" << endl;
  // A1. indexgenome
  // set up the test directory 
  // NOTHING HERE YET, TODO. Gotta check for the presence of the files, though.
  char * lMappingFile = "testmappings.sam";
  char * lOutDir = "out";
  char * lMappingType = "SAM";
  // test for the presence
  Path lMappingFilePath( lMappingFile );
  Path lOutDirPath( lOutDir );
  if ( !( lMappingFilePath.IsFile() && lOutDirPath.IsDirectory() ) )
  {
    cout << "A1a. Test Files Present: FAIL" << endl;
    return -1; // test directory is not properly set up.
  }
  else
   cout << "A1a. Test Files Present: PASS" << endl;

  // set up the parameters
  int lParameterCount = 9;
  char * lParameters[9];
  lParameters[0] = argv[0]; // identify ourselves as test. This shouldn't matter for anything.
  lParameters[1] = "indexmappings";
  lParameters[2] = "-b";
  lParameters[3] = "-t";
  lParameters[4] = lMappingType;
  lParameters[5] = "-m";
  lParameters[6] = lMappingFile; // mapping file residing in current directory
  lParameters[7] = "-o";
  lParameters[8] = lOutDir; // out directory directly beneath the current one.



  try
  {
    cout << "A1b. indexgenome: bundle test BEGIN" << endl << endl;

    Caper::UserInterface( lParameterCount, lParameters ); 

    cout << endl << "A1b. indexgenome: bundle test: COMPLETE" << endl;

  }
  catch( string lError )
  {
    cout << endl << "A1b. indexgenome: bundle test: FAIL" << endl;
    cerr << " Error Message: " << lError << endl;
    return -1;
  }
  
  Path lBundlePath = lOutDirPath / (string(lMappingFile) + ".bundle");
  if ( lBundlePath.Exists() )
    cout << endl << "A1b. indexgenome: bundle test: PASS" << endl;
  else
  {
    cout << endl << "A1b. indexgenome: bundle test: FAIL" << endl;
    cerr << " Output bundle file not found." << endl;
    return -1;
  }

  //string lUsageString = 
  //  "Caper v0.4.1\nUsage:\n Tests \n";

  //try
  //{
  //  Arguments lArgs;
  //  if ( !lArgs.ProcessArguments( argc, argv ) )
  //  {
  //    cout << lUsageString << endl;
  //    return;
  //  }

  //  if ( lArgs.Mode == lArgs.INDEXGENOME )
  //    IndexGenome( lArgs );
		//else if ( lArgs.Mode == lArgs.INDEXMAPPINGS )
  //    IndexMappings( lArgs );
		//else if ( lArgs.Mode == lArgs.INTERACTIVE )
  //    Interactive( lArgs );

  //  cout << "DONE!" << endl;
  //}
  //catch( string lException )
  //{
  //  cerr << "ERROR: " << lException << endl;
  //  return;
  //}

  return 0;
}

//void Tests::IndexGenome( Arguments lArgs )
//{
//  IndexGenomeArguments * lModeArgs = (IndexGenomeArguments*) lArgs.ModeArgs;
//
//  cout << "Reading Genome \"" << lModeArgs->GenomePath << "\"... ";
//  cout.flush();
//
//  SequenceIndexer * lSequenceIndexer = new FASequenceIndexer( lModeArgs->GenomePath, lModeArgs->SavePath );
//  lSequenceIndexer->Index();
//}
//
//void Tests::IndexMappings( Arguments lArgs )
//{
//  IndexMappingsArguments * lModeArgs = (IndexMappingsArguments *) lArgs.ModeArgs;
//
//  //SequenceEngine * lSequenceEngine = new FASequenceEngine( lModeArgs->GenomePath, lModeArgs->ReferenceGenomeIndexPath );
//  //lSequenceEngine->Initialize();
//
//  cout << "Preparing Mappings \"" << lModeArgs->MappingPath << "\"... " << endl;
//  cout.flush();
//
//  MappingsIndexer lIndexer( lModeArgs->MappingPath, lModeArgs->MappingStyle, lModeArgs->SavePath, lModeArgs->Bundle );
//  lIndexer.IndexMappingsAndSave();
// 
//}
//
//void Tests::Interactive( Arguments lArgs )
//{
//  string lCommandString = "You can type: \"<contig ident>:<X>:<Y>\", or \"<contig ident>:<X>:<Y>:p\" (for pretty mode)";
//
//  InteractiveArguments * lModeArgs = (InteractiveArguments*) lArgs.ModeArgs;
//
//  SequenceEngine * lSequenceEngine = new FASequenceEngine( lModeArgs->GenomePath, lModeArgs->ReferenceGenomeIndexPath );
//  lSequenceEngine->Initialize();
//
//  MappingEngine * lMappingEngine;
//  if ( lModeArgs->BundlePath.length() > 0 )
//    lMappingEngine = new MappingEngine( lModeArgs->BundlePath );
//  else
//    lMappingEngine = new MappingEngine( lModeArgs->MappingPath, lModeArgs->MappingIndexPath );
//  lMappingEngine->Initialize();
//
//  cout << "> ";
//
//  string lInput = "";
//  Commands lCommands;
//
//  while ( cin >> lInput )
//  {
//    if ( !lCommands.ProcessArguments( lInput, lSequenceEngine->mSequences, lMappingEngine ) )
//    {
//      cout << "Invalid Input: " << lCommandString << endl << "> " ;
//      continue;
//    }
//
//    if ( lCommands.Action == lCommands.GETREADS )
//    {
//      Mappings * lMappings = lMappingEngine->GetReads(lCommands.ContigIdent, lCommands.Left, lCommands.Right );
//      if ( lCommands.PrettyMode ) // engage pretty mode
//      {
//        cout << PadLeft( lMappingEngine->GetReadLength() ) << lCommands.Left + lMappingEngine->GetReadLength() << endl;
//        cout << PadLeft( lMappingEngine->GetReadLength() ) << PadLeft( lCommands.Right - lCommands.Left, "*") << endl;
//
//        string lGenome = "";
//
//        Sequence * lContig = (*lSequenceEngine->mSequences)[ lCommands.ContigIdent ];
//
//        int lTargetGenomeWidth = lCommands.Right - lCommands.Left + 1 + lMappingEngine->GetReadLength(); // FIX THIS LATER
//        if ( lTargetGenomeWidth < lContig->Length )
//          lGenome = lContig->Substring( lCommands.Left, lTargetGenomeWidth );
//        else
//          lGenome = lContig->Substring( lCommands.Left );
//
//        cout << lGenome << endl;
//
//        int lGenomeLength = lGenome.length();
//        for ( int i = 0 ; i < lMappings->size(); i++ )
//        {
//          string lHighlightedString = lMappings->at(i)->mSequence->ToString();
//          for ( int j = 0; j < lHighlightedString.length(); j++ )
//          {
//            int lTargetLocalIndexOnGenome = lMappings->at(i)->Index - lCommands.Left + j;
//            if ( lTargetLocalIndexOnGenome < lGenome.length() &&
//              lHighlightedString[j] != lGenome[ lTargetLocalIndexOnGenome ] )
//            {
//              if ( islower(lHighlightedString[j]) )
//                lHighlightedString[j] = toupper( lHighlightedString[j] );
//              else
//                lHighlightedString[j] = tolower( lHighlightedString[j] );
//            }
//          }
//
//          cout << PadLeft( lMappings->at(i)->Index - lCommands.Left ) << lHighlightedString << "\n";
//        }
//      }
//      else
//      {
//        for ( int i = 0 ; i < lMappings->size(); i++ )
//        {
//          cout << "Index " << lMappings->at(i)->Index << ": " << lMappings->at(i)->Name << " - " << lMappings->at(i)->mSequence->ToString() << "\n";
//        }
//      }
//
//      delete lMappings;
//    }
//
//    cout << "> ";
//  }
//}


//string Tests::PadLeft( int aCount )
//{
//  return PadLeft( aCount, " " );
//}
//
//string Tests::PadLeft( int aCount, string aPad )
//{
//  string lThing = "";
//
//  for (int i = 0; i < aCount; i++ )
//    lThing.append(aPad);
//
//  return lThing;
//}


