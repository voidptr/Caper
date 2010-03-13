
#include "stdafx.h"

#include "FASequenceEngine.h"
#include "BowtieMappingEngine.h"
#include "MapviewMappingEngine.h"
#include "BowtieMappingsPreparer.h"
#include "MapviewMappingsPreparer.h"
#include "Arguments.h"
#include "Commands.h"
#include <time.h>

class CaperBenchmarks
{
public:
  static void Start(int argc, char * const argv[] );
  static string PadLeft( int aCount );
  static string PadLeft( int aCount, string aPadChar );
};
