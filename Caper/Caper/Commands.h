#pragma once
#include "stdafx.h"
#include "Typedefs.h"
#include "MappingEngine.h"

class Commands
{
  static const char Colon = ':';

private:
  bool ParseReadCommand(string & aLine, Sequences & aReferenceGenome, MappingEngine * aMappingEngine );

public:
  enum Actions
  {
    //HELP,
    GETREADS,
   /* LISTCONTIGS,
    CONTIGINFO,
    QUIT,*/
  } Action;

  string ContigIdent;
  long Left;
  long Right;
  bool PrettyMode;
public:
  bool ProcessArguments(string & aLine, Sequences & aReferenceGenome, MappingEngine * aMappingEngine );
};
