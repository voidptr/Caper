#include "Commands.h"

bool Commands::ProcessArguments(string & aLine, Sequences & aReferenceGenome, MappingEngine * aMappingEngine )
{
  /*if ( aLine == "help" )  
    Action = HELP;  
  else if (  aLine == "quit" ||  aLine == "exit" )  
    Action = QUIT;
  else if (  aLine ==  "list" )
    Action = LISTCONTIGS;    
  else if (  aLine ==  "info")
    Action = CONTIGINFO;    
  else */if ( aLine.length() > 0 )
  {
    Action = GETREADS;
    if (! ParseReadCommand( aLine, aReferenceGenome, aMappingEngine ) )
      return false;
  }

  return true;
}

bool Commands::ParseReadCommand(string & aLine, Sequences & aReferenceGenome, MappingEngine * aMappingEngine )
{
  PrettyMode = false;
  Left = -1;
  Right = -1;  
  ContigIdent = "";

  istringstream lStringStream( aLine );
  vector<string> lBits;

  while ( lStringStream.peek() > 0 )
  {
    string lBit;
    getline( lStringStream, lBit, Colon );
    lBits.push_back( lBit );
  }

  if ( lBits.size() < 2 )
  {
    cerr << "Too few parameters." << endl;
    return false;
  }

  ContigIdent = lBits[0];
  Left = atoi( lBits[1].c_str() );
  
  if ( lBits.size() > 2 )
  {
    Right = atoi( lBits[2].c_str() );
    if ( lBits.size() > 3 )    
      PrettyMode = true;    
  }
  else
    Right = Left;
  
  if ( Left - aMappingEngine->ReadLength < 0 )
    Left = 0;
  else
    Left -= aMappingEngine->ReadLength;


  if ( Left > Right || Left < 0 || Right < 0 )
  {
    cerr << "Incorrect parameters. Left must be less than Right, and both must be positive." << endl;
    return false;
  }

  if ( aReferenceGenome.find( ContigIdent ) == aReferenceGenome.end() )
  {
    cerr << "ContigID defined not found in reference genome." << endl;
    return false;
  }

  if ( Right > aReferenceGenome[ ContigIdent ]->Length )
  {
    cerr << "Right parameter is greater than contig length." << endl;
    return false;
  }

  return true;
}
