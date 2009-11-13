
#include "stdafx.h"

#include "FASequenceEngine.h"
#include "BowtieMappingEngine.h"
#include "MapviewMappingEngine.h"
#include "BowtieMappingsPreparer.h"
#include "MapviewMappingsPreparer.h"
#include "Arguments.h"
#include "Commands.h"

class Caper
{
public:
  static void UserInterface(int argc, char * const argv[] );
  static string PadLeft( int aCount );
  static string PadLeft( int aCount, string aPadChar );
};
