#pragma once
#include "SequenceIndexer.h"
#include "Utilities.h"
#include "Typedefs.h"

#include <algorithm>

#include "Path.h"

class FASequenceIndexer :
  public SequenceIndexer
{
private:
  static const char GreaterThan = '>';
  static const char NewLine = '\n';
  void SaveIndex();

public:
  FASequenceIndexer(string & aPath, string & aSavePath);
  FASequenceIndexer(char * aPath, char * aSavePath);
  void Index();
};
