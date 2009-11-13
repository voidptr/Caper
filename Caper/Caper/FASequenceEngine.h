#pragma once
#include "SequenceEngine.h"
#include "Typedefs.h"

#include <algorithm>

class FASequenceEngine :
  public SequenceEngine
{
private:
  static const char GreaterThan = '>';
  static const char NewLine = '\n';
public:
  FASequenceEngine(string & aPath);
  FASequenceEngine(char * aPath);
  void Initialize();
  void Initialize( string aIndexPath );  
  void SaveIndex( string aSavePath );
};
