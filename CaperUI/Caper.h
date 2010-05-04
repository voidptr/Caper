
#include "stdafx.h"

#include "FASequenceEngine.h"
#include "FASequenceIndexer.h"

#include "MappingEngine.h"

#include "MappingIndexer.h"

#include "Arguments.h"
#include "Commands.h"

#include "Typedefs.h"

class Caper
{
public:
  static void UserInterface(int argc, char * const argv[] );
  static void IndexGenome( Arguments lArgs );
  static void IndexMappings( Arguments lArgs );
  static void Interactive( Arguments lArgs );
  static string PadLeft( int aCount );
  static string PadLeft( int aCount, string aPadChar );
};
