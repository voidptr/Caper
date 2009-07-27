
#include "stdafx.h"

#include "FASequenceEngine.h"
#include "BowtieMappingEngine.h"
#include "MapviewMappingEngine.h"
#include "BowtieMappingsPreparer.h"
#include "MapviewMappingsPreparer.h"

class Caper
{
private:
  static const int ReadLength = 36;
  static const char Colon = ':';
public:
  static void UserInterface(int argc, char * const argv[] );
  static bool IsValid( string lInput );
  static void GetParameters( string aLine, string* aBits );
  static string PadLeft( int aCount );
};
