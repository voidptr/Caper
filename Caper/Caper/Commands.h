#pragma once
#include "stdafx.h"

class Commands
{
  static const char Colon = ':';

private:
  bool ParseReadCommand( string & aLine );

public:
  enum Actions
  {
    HELP,
    GETREADS,
    LISTCONTIGS,
    CONTIGINFO,
    QUIT,
  } Action;

  string ContigIdent;
  long Left;
  long Right;
  bool PrettyMode;
public:
  bool ProcessArguments( string & aLine );
};
