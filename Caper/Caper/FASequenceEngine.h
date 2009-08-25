#pragma once
#include "SequenceEngine.h"
#include "Typedefs.h"

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
  //  void Initialize( char * aIndexPath ) { string i(aIndexPath); Initialize(i); }
  void SaveIndex( string aSavePath );
};
