#pragma once
#include "stdafx.h"
#include "Typedefs.h"
#include "MappingEngine.h"

class InteractiveCommands
{
  static const char Colon = ':';

private:
  bool ParseReadCommand(vector<string> & lCommands, MappingEngine * aMappingEngine )
  {

    Left = -1;
    Right = -1;  
    ContigIdent = "";

    if ( lCommands.size() < 3 )
    {
      cerr << "Too few parameters." << endl;
      return false;
    }

    ContigIdent = lCommands[1];
    Left = atoi( lCommands[2].c_str() );
  
    if ( lCommands.size() > 3 )
    {
      Right = atoi( lCommands[3].c_str() );
    }
    else
      Right = Left;
  
    if ( Left > Right || Left < 0 || Right < 0 )
    {
      cerr << "Incorrect parameters. Left must be less than Right, and both must be positive." << endl;
      return false;
    }

    return true;
  }

  void SplitCommands( vector<string> * aCommandsTarget, string & aLine )
  {
    istringstream lStringStream( aLine );

    while ( lStringStream.peek() > 0 )
    {
      string lBit;
      //getline( lStringStream, lBit, Colon );
      lStringStream >> lBit;
      aCommandsTarget->push_back( lBit );
    }
  }

public:
  enum Actions
  {
    //HELP,
    READS,
    INTERSECT,
    INTERVAL
   /* LISTCONTIGS,
    CONTIGINFO,
    QUIT,*/
  } Action;

  string ContigIdent;
  long long Left;
  long long Right;
public:
  bool ProcessArguments(string & aLine, MappingEngine * aMappingEngine )
  {
    /*if ( aLine == "help" )  
      Action = HELP;  
    else if (  aLine == "quit" ||  aLine == "exit" )  
      Action = QUIT;
    else if (  aLine ==  "list" )
      Action = LISTCONTIGS;    
    else if (  aLine ==  "info")
      Action = CONTIGINFO;    */

    vector<string> lCommands;

    if ( aLine.length() > 0 )
      SplitCommands( &lCommands, aLine );

    if ( lCommands.size() > 0 && lCommands[0] == "reads" )
    {
      Action = READS;
      if (! ParseReadCommand( lCommands, aMappingEngine ) )
        return false;
    }
    else if ( lCommands.size() > 0 && lCommands[0] == "intersect" )
    {
      Action = INTERSECT;
      if (! ParseReadCommand( lCommands, aMappingEngine ) )
        return false;
    }
    if ( lCommands.size() > 0 && lCommands[0] == "interval" )
    {
      Action = INTERVAL;
      if (! ParseReadCommand( lCommands, aMappingEngine ) )
        return false;
    }

    return true;
  }


};
