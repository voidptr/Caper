#pragma once

#include "Typedefs.h"

class SequenceEngine
{
protected:
  string mPath;

public:
  Sequences mSequences;
public:
  SequenceEngine(string & aPath);
  virtual void Initialize() = 0;
};
