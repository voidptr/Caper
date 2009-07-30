#include "Commands.h"

bool Commands::ProcessArguments(string & aLine)
{
  if ( aLine == "help" )  
    Action = HELP;  
  else if (  aLine == "quit" ||  aLine == "exit" )  
    Action = QUIT;
  else if (  aLine ==  "list" )
    Action = LISTCONTIGS;    
  else if (  aLine ==  "info")
    Action = CONTIGINFO;    
  else if ( aLine.length() > 0 )
  {
    Action = GETREADS;
    if (! ParseReadCommand( aLine ) )
      return false;
  }

  return true;
}

bool Commands::ParseReadCommand(string & aLine)
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
    return false;

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

  if ( Left > Right || Left < 0 || Right < 0 )
    return false;


  return true;
}
