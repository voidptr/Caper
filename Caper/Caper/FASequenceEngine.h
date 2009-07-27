#pragma once
#include "sequenceengine.h"
#include "Sequences.h"

class FASequenceEngine :
  public SequenceEngine
{
private:
  static const char GreaterThan = '>';
  static const char NewLine = '\n';
public:
  FASequenceEngine(string & aPath);
  void Initialize();
};
