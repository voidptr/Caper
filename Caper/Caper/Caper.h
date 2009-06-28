
#include "stdafx.h"

#include "SequenceReader.h"
#include "MapviewMappingEngine.h"

class Caper
{
private:
  static const int ReadLength = 36;
  static const char Colon = ':';
public:
  static void UserInterface(int argc, char * const argv[] );
  static bool IsValid( string lInput );
  static void GetParameters( string aLine, string* aBits );
  static string Caper::PadLeft( int aCount );
};